/*
 * ILI9341_conf.h
 *
 *  Created on: May 19, 2023
 *      Author: botel
 */

#ifndef INC_ILI9341_CONF_H_
#define INC_ILI9341_CONF_H_

/* Exported Includes */
#include "main.h"

/* Display Parameters */
#define DISPLAY_PIXELS_V				240
#define DISPLAY_PIXELS_H				320
#define DISPLAY_PIXELS_NBR				DISPLAY_PIXELS_H * DISPLAY_PIXELS_V
#define DISPLAY_PARTIAL_BUFFER_RATIO	8
#define DISPLAY_PARTIAL_BUFFER_SIZE		DISPLAY_PIXELS_NBR/DISPLAY_PARTIAL_BUFFER_RATIO

/* Display Pinout */
#define DISPLAY_MISO_PORT				DISP_MISO_GPIO_Port
#define DISPLAY_MISO_PIN				DISP_MISO_Pin
#define DISPLAY_MOSI_PORT				DISP_MOSI_GPIO_Port
#define DISPLAY_MOSI_PIN				DISP_MISO_Pin
#define DISPLAY_SCK_PORT				DISP_SCK_GPIO_Port
#define DISPLAY_SCK_PIN					DISP_SCK_Pin
#define DISPLAY_CS_PORT					DISP_NCS_GPIO_Port
#define DISPLAY_CS_PIN					DISP_NCS_Pin
#define DISPLAY_NRST_PORT				DISP_NRESET_GPIO_Port
#define DISPLAY_NRST_PIN				DISP_NRESET_Pin
#define DISPLAY_TE_PORT					DISP_TE_GPIO_Port
#define DISPLAY_TE_PIN					DISP_TE_Pin
#define DISPLAY_DCX_PORT				DISP_DCX_GPIO_Port
#define DISPLAY_DCX_PIN					DISP_DCX_Pin

/* Peripheral handlers */
#define DISPLAY_SPI_HANDLER				hspi1
#define DISPLAY_DMA_HANDLER				hdma_spi1_tx
#define DISPLAY_TIM_HANDLER				htim6

#endif /* INC_ILI9341_CONF_H_ */
