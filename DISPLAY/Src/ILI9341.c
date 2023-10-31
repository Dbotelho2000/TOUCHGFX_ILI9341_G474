/*
 * ILI9341.h
 *
 *  Created on: May 19, 2023
 *      Author: botel
 */

/* Private Includes */
#include "ILI9341.h"


/* Private Defines */


/* Private Macros */
#define DISP_CS_LOW()			DISPLAY_CS_PORT->BRR |= DISPLAY_CS_PIN
#define DISP_CS_HIGH()			DISPLAY_CS_PORT->BSRR |= DISPLAY_CS_PIN

#define DISP_DCX_LOW()			DISPLAY_DCX_PORT->BRR |= DISPLAY_DCX_PIN
#define DISP_DCX_HIGH()			DISPLAY_DCX_PORT->BSRR |= DISPLAY_DCX_PIN

#define DISP_NRST_LOW()			DISPLAY_NRST_PORT->BRR |= DISPLAY_NRST_PIN
#define DISP_NRST_HIGH()		DISPLAY_NRST_PORT->BSRR |= DISPLAY_NRST_PIN


/* Private Global Variables */
volatile uint8_t __isTransmittingBlock = RESET;
volatile uint16_t TE = 0;

/* Private Function Prototypes */
void Display_Reset(void);
void Display_On(void);
void Display_Off(void);
void Display_DCS_Send(uint8_t Command);
void Display_DCS_Send_With_Data(uint8_t Command, uint8_t* data, uint8_t size);
void touchgfxSignalVSync(void);

/* Private Functions */
void Display_Reset(void)
{
	DISP_NRST_LOW();
	HAL_Delay(100);
	DISP_NRST_HIGH();
	HAL_Delay(100);
}

void Display_On(void)
{
	/* Turn the Display ON */
	Display_DCS_Send(DCS_SET_DISPLAY_ON);
	HAL_Delay(100);
}

void Display_Off(void)
{
	/* Turn the Display OFF */
	Display_DCS_Send(DCS_SET_DISPLAY_OFF);
	HAL_Delay(100);
}

void Display_DCS_Send(uint8_t Command)
{
	/* Enable the SPI Peripheral */
	__HAL_SPI_ENABLE(&DISPLAY_SPI_HANDLER);

	/* Reset the CS and DCX Pins */
	DISP_CS_LOW();
	DISP_DCX_LOW();

	/* Send the command */
	*((__IO uint8_t *)&DISPLAY_SPI_HANDLER.Instance->DR) = Command;

	/* Wait until the bus is not busy before changing configuration */
	while(((hspi1.Instance->SR) & SPI_FLAG_BSY) != RESET);

	/* Set the CS and DCX Pin */
	DISP_DCX_HIGH();
	DISP_CS_HIGH();

	/* Disable the SPI */
	__HAL_SPI_DISABLE(&DISPLAY_SPI_HANDLER);
}

void Display_DCS_Send_With_Data(uint8_t Command, uint8_t* data, uint8_t size)
{
	/* Enable the SPI Peripheral */
	__HAL_SPI_ENABLE(&DISPLAY_SPI_HANDLER);

	/* Reset the CS and DCX Pins */
	DISP_CS_LOW();
	DISP_DCX_LOW();

	/* Send the command */
	*((__IO uint8_t *)&DISPLAY_SPI_HANDLER.Instance->DR) = Command;

	/* Wait until the bus is not busy before changing configuration */
	while(((hspi1.Instance->SR) & SPI_FLAG_BSY) != RESET);

	/* Set the DCX Pin */
	DISP_DCX_HIGH();

	/* Looping for sending all data */
	while(size > 0U)
	{
		/* Send Data */
		*((__IO uint8_t*)&DISPLAY_SPI_HANDLER.Instance->DR) = *data;

		/* Increment the data pointer and decrement the size */
		data++;
		size--;

		/* Wait until TXE flag is set to send data */
		while(!__HAL_SPI_GET_FLAG(&DISPLAY_SPI_HANDLER, SPI_FLAG_TXE));
	}

	/* Wait until the bus is not busy before changing configuration */
	while(__HAL_SPI_GET_FLAG(&DISPLAY_SPI_HANDLER, SPI_FLAG_BSY));

	/* Set the CS and DCX Pin */
	DISP_DCX_HIGH();
	DISP_CS_HIGH();

	/* Disable the SPI */
	__HAL_SPI_DISABLE(&DISPLAY_SPI_HANDLER);
}

/* Exported Functions */
void ILI9341_Init(void)
{
	/* Local Variables */
	uint8_t arguments[4];

	/* Set the CS and NRST pin */
	DISP_CS_HIGH();
	DISP_NRST_HIGH();

	/* Disable the transfer half complete interrupt */
	__HAL_DMA_DISABLE_IT(&DISPLAY_DMA_HANDLER, DMA_IT_HT);

	/* Enable the transfer complete interrupt */
	__HAL_DMA_ENABLE_IT(&DISPLAY_DMA_HANDLER, DMA_IT_TC | DMA_IT_TE);

	/* Reset the Display */
	Display_Reset();

	/* Sleep Out */
	Display_DCS_Send(DCS_EXIT_SLEEP_MODE);
	HAL_Delay(100);

	/* Display Normal mode */
	Display_DCS_Send(DCS_ENTER_NORMAL_MODE);
	HAL_Delay(100);

	/* MADCTL: Mirror X */
	arguments[0] = 0x48;
	Display_DCS_Send_With_Data(DCS_SET_ADDRESS_MODE, arguments, 1);
	HAL_Delay(100);

	/* Pixel Format */
	arguments[0] = 0x05; // RGB565
	Display_DCS_Send_With_Data(DCS_SET_PIXEL_FORMAT, arguments, 1);
	HAL_Delay(100);

	/* Tearing effect line on */
	arguments[0] = 0; //0x00;
	Display_DCS_Send_With_Data(DCS_SET_TEAR_ON, arguments, 1);
	HAL_Delay(100);

	/* Tearing effect scan line */
	arguments[0] = 0;
	arguments[1] = 0;
	Display_DCS_Send_With_Data(DCS_SET_TEAR_SCANLINE, arguments, 2);
	HAL_Delay(100);

	/* Turn display on */
	Display_On();
}

void ILI9341_Set_Area(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	/* Local Variables */
	uint8_t arguments[4];

	/* Set columns */
	arguments[0] = x0 >> 8;
	arguments[1] = x0 & 0xFF;
	arguments[2] = x1 >> 8;
	arguments[3] = x1 & 0xFF;

	/* Send Command */
	Display_DCS_Send_With_Data(DCS_SET_COLUMN_ADDRESS, arguments, 4);

	/* Set rows */
	arguments[0] = y0 >> 8;
	arguments[1] = y0 & 0xFF;
	arguments[2] = y1 >> 8;
	arguments[3] = y1 & 0xFF;

	/* Send Command */
	Display_DCS_Send_With_Data(DCS_SET_PAGE_ADDRESS, arguments, 4);
}

void ILI9341_Draw_Square(uint16_t color, uint16_t posx, uint16_t posy, uint16_t sizex, uint16_t sizey)
{
	/* Local Variables */
	uint8_t Command = DCS_WRITE_MEMORY_START;
	uint32_t size = sizex * sizey;

	/* Define the Display Area */
	ILI9341_Set_Area(posx, posy, posx + sizex - 1, posy + sizey - 1);

	/* Reset the CS and DCX Pins */
	DISP_CS_LOW();
	DISP_DCX_LOW();

	/* Enable the SPI */
	__HAL_SPI_ENABLE(&DISPLAY_SPI_HANDLER);

	/* Send the command */
	*((__IO uint8_t *)&DISPLAY_SPI_HANDLER.Instance->DR) = Command;

	/* Wait until the bus is not busy before changing configuration */
	while(((hspi1.Instance->SR) & SPI_FLAG_BSY) != RESET);

	/* Set the DCX Pin */
	DISP_DCX_HIGH();

	/* Set the SPI in 16-bit mode to match endianess */
	DISPLAY_SPI_HANDLER.Instance->CR2 = SPI_DATASIZE_16BIT;

	while(size > 0U)
	{
		/* Send Data */
		*((__IO uint16_t*)&DISPLAY_SPI_HANDLER.Instance->DR) = color;

		/* Decrement the size */
		size--;

		/* Wait until TXE flag is set to send data */
		while(!__HAL_SPI_GET_FLAG(&DISPLAY_SPI_HANDLER, SPI_FLAG_TXE));
	}

	/* Wait until the bus is not busy before changing configuration */
	while(__HAL_SPI_GET_FLAG(&DISPLAY_SPI_HANDLER, SPI_FLAG_BSY));

	/* Set the CS and DCX Pin */
	DISP_DCX_HIGH();
	DISP_CS_HIGH();

	/* Go back to 8-bit mode */
	DISPLAY_SPI_HANDLER.Instance->CR2 = SPI_DATASIZE_8BIT;

	/* Disable the SPI */
	__HAL_SPI_DISABLE(&DISPLAY_SPI_HANDLER);
}

void ILI9341_Draw_Square_DMA(uint32_t* color, uint16_t posx, uint16_t posy, uint16_t sizex, uint16_t sizey)
{
	/* Local Variables */
	uint8_t Command = DCS_WRITE_MEMORY_START;
	uint32_t size = sizex * sizey;
	__isTransmittingBlock = SET;

	/* Define the Display Area */
	ILI9341_Set_Area(posx, posy, posx + sizex - 1, posy + sizey - 1);

	/* Reset the CS and DCX Pins */
	DISP_CS_LOW();
	DISP_DCX_LOW();

	/* Enable the SPI */
	__HAL_SPI_ENABLE(&DISPLAY_SPI_HANDLER);

	/* Send the command */
	*((__IO uint8_t *)&DISPLAY_SPI_HANDLER.Instance->DR) = Command;

	/* Wait until the bus is not busy before changing configuration */
	while(((hspi1.Instance->SR) & SPI_FLAG_BSY) != RESET);

	/* Set the DCX Pin */
	DISP_DCX_HIGH();

	/* Set the SPI in 16-bit mode to match endianess */
	DISPLAY_SPI_HANDLER.Instance->CR2 = SPI_DATASIZE_16BIT;

	/* Disable SPI Peripherals */
	__HAL_SPI_DISABLE(&DISPLAY_SPI_HANDLER);
	__HAL_DMA_DISABLE(&DISPLAY_DMA_HANDLER);

	/* Clear the last DMA Tx flag */
	CLEAR_BIT(DISPLAY_SPI_HANDLER.Instance->DR, SPI_CR2_LDMATX);

	/* Set the DMA Channel data length */
	DISPLAY_DMA_HANDLER.Instance->CNDTR = size;

	/* Set the DMA channel destination address */
	DISPLAY_DMA_HANDLER.Instance->CPAR = (uint32_t)&DISPLAY_SPI_HANDLER.Instance->DR;

	/* Set the channel source address */
	DISPLAY_DMA_HANDLER.Instance->CMAR = (uint32_t)color;

	/* Disable the memory address increment */
	DISPLAY_DMA_HANDLER.Instance->CCR &= ~DMA_CCR_MINC;

	/* Enable the peripherals */
	__HAL_DMA_ENABLE(&DISPLAY_DMA_HANDLER);
	__HAL_SPI_ENABLE(&DISPLAY_SPI_HANDLER);

	/* Enable Tx DMA Request */
	SET_BIT(DISPLAY_SPI_HANDLER.Instance->CR2, SPI_CR2_TXDMAEN);
}

void ILI9341_Draw_Bitmap(uint16_t *data, uint16_t posx, uint16_t posy, uint16_t sizex, uint16_t sizey)
{
	/* Local Variables */
	uint8_t Command = DCS_WRITE_MEMORY_START;
	uint32_t size = sizex * sizey;

	/* Define the Display Area */
	ILI9341_Set_Area(posx, posy, posx + sizex - 1, posy + sizey - 1);

	/* Reset the CS and DCX Pins */
	DISP_CS_LOW();
	DISP_DCX_LOW();

	/* Enable the SPI */
	__HAL_SPI_ENABLE(&DISPLAY_SPI_HANDLER);

	/* Send the command */
	*((__IO uint8_t *)&DISPLAY_SPI_HANDLER.Instance->DR) = Command;

	/* Wait until the bus is not busy before changing configuration */
	while(((hspi1.Instance->SR) & SPI_FLAG_BSY) != RESET);

	/* Set the DCX Pin */
	DISP_DCX_HIGH();

	/* Set the SPI in 16-bit mode to match endianess */
	DISPLAY_SPI_HANDLER.Instance->CR2 = SPI_DATASIZE_16BIT;

	while(size > 0U)
	{
		/* Send Data */
		*((__IO uint16_t*)&DISPLAY_SPI_HANDLER.Instance->DR) = *data;

		/* Decrement size and Increment data */
		size--;
		data++;

		/* Wait until TXE flag is set to send data */
		while(!__HAL_SPI_GET_FLAG(&DISPLAY_SPI_HANDLER, SPI_FLAG_TXE));
	}

	/* Wait until the bus is not busy before changing configuration */
	while(__HAL_SPI_GET_FLAG(&DISPLAY_SPI_HANDLER, SPI_FLAG_BSY));

	/* Set the CS and DCX Pin */
	DISP_DCX_HIGH();
	DISP_CS_HIGH();

	/* Go back to 8-bit mode */
	DISPLAY_SPI_HANDLER.Instance->CR2 = SPI_DATASIZE_8BIT;

	/* Disable the SPI */
	__HAL_SPI_DISABLE(&DISPLAY_SPI_HANDLER);
}

void ILI9341_Draw_Bitmap_DMA(uint16_t* data, uint16_t posx, uint16_t posy, uint16_t sizex, uint16_t sizey)
{
	/* Local Variables */
	uint8_t Command = DCS_WRITE_MEMORY_START;
	volatile uint32_t size = sizex * sizey;
	__isTransmittingBlock = SET;

	/* Define the Display Area */
	ILI9341_Set_Area(posx, posy, posx + sizex - 1, posy + sizey - 1);

	/* Reset the CS and DCX Pins */
	DISP_CS_LOW();
	DISP_DCX_LOW();

	/* Enable the SPI */
	__HAL_SPI_ENABLE(&DISPLAY_SPI_HANDLER);

	/* Send the command */
	*((__IO uint8_t *)&DISPLAY_SPI_HANDLER.Instance->DR) = Command;

	/* Wait until the bus is not busy before changing configuration */
	while(((hspi1.Instance->SR) & SPI_FLAG_BSY) != RESET);

	/* Set the DCX Pin */
	DISP_DCX_HIGH();

	/* Set the SPI in 16-bit mode to match endianess */
	DISPLAY_SPI_HANDLER.Instance->CR2 = SPI_DATASIZE_16BIT;

	/* Disable SPI Peripherals */
	__HAL_SPI_DISABLE(&DISPLAY_SPI_HANDLER);
	__HAL_DMA_DISABLE(&DISPLAY_DMA_HANDLER);

	/* Clear all flags */
	__HAL_DMA_CLEAR_FLAG(&hdma_spi1_tx, (DMA_FLAG_GL1 << (hdma_spi1_tx.ChannelIndex & 0x1cU)));

	/* Clear the last DMA Tx flag */
	CLEAR_BIT(DISPLAY_SPI_HANDLER.Instance->CR2, SPI_CR2_LDMATX);

	/* Set the DMA Channel data length */
	DISPLAY_DMA_HANDLER.Instance->CNDTR = size;

	/* Set the DMA channel destination address */
	DISPLAY_DMA_HANDLER.Instance->CPAR = (uint32_t)&DISPLAY_SPI_HANDLER.Instance->DR;

	/* Set the channel source address */
	DISPLAY_DMA_HANDLER.Instance->CMAR = (uint32_t)data;

	/* Enable the memory address increment */
	DISPLAY_DMA_HANDLER.Instance->CCR |= DMA_CCR_MINC;

	/* Disable the transfer half complete interrupt */
	__HAL_DMA_DISABLE_IT(&hdma_spi1_tx, DMA_IT_HT);
	/* Enable the transfer complete interrupt */
	__HAL_DMA_ENABLE_IT(&hdma_spi1_tx, (DMA_IT_TC | DMA_IT_TE));

	/* Enable the peripherals */
	__HAL_DMA_ENABLE(&DISPLAY_DMA_HANDLER);
	__HAL_SPI_ENABLE(&DISPLAY_SPI_HANDLER);

	/* Enable Tx DMA Request */
	SET_BIT(DISPLAY_SPI_HANDLER.Instance->CR2, SPI_CR2_TXDMAEN);
}

void ILI9341_GPIOTE_Callback(void)
{
	/* Check the TE pin state */
	if(DISPLAY_TE_PORT->IDR & DISPLAY_TE_PIN)
	{
		/* Rising Callback */
		TE++;

		/* Stop the timer and reset it counter */
		DISPLAY_TIM_HANDLER.Instance->CR1 &= ~(TIM_CR1_CEN);
		DISPLAY_TIM_HANDLER.Instance->CNT = 0;

		/* Call the Touch GFX V-SYNC */
		touchgfxSignalVSync();
	}
	else
	{
		/* Start the timer */
		DISPLAY_TIM_HANDLER.Instance->CR1 |= TIM_CR1_CEN;
	}
}

void ILI9341_DMA_Callback(void)
{
	/* Disable the transfer complete and error interrupt */
	__HAL_DMA_DISABLE_IT(&hdma_spi1_tx, DMA_IT_TE | DMA_IT_TC);

	/* Clear the transfer complete flag */
	__HAL_DMA_CLEAR_FLAG(&DISPLAY_DMA_HANDLER, DMA_FLAG_TC1);

	/* Reset the transmitting flag */
	__isTransmittingBlock = RESET;

	/* Wait until the bus is not busy */
	while(__HAL_SPI_GET_FLAG(&DISPLAY_SPI_HANDLER, SPI_FLAG_BSY));

	/* Set the Chip Select */
	DISP_CS_HIGH();

	/* Go back to 8-bit mode */
	DISPLAY_SPI_HANDLER.Instance->CR2 = SPI_DATASIZE_8BIT;

	/* Signal Transfer Complete to TouchGFX */
	DisplayDriver_TransferCompleteCallback();
}
