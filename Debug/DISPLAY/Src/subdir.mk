################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DISPLAY/Src/ILI9341.c 

C_DEPS += \
./DISPLAY/Src/ILI9341.d 

OBJS += \
./DISPLAY/Src/ILI9341.o 


# Each subdirectory must supply rules for building sources it contributes
DISPLAY/Src/%.o DISPLAY/Src/%.su DISPLAY/Src/%.cyclo: ../DISPLAY/Src/%.c DISPLAY/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I"C:/Users/botelhod/STM32CubeIDE/TouchGFX_SPI_Display/TOUCHGFX_ILI9341_G474/DISPLAY/Inc" -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DISPLAY-2f-Src

clean-DISPLAY-2f-Src:
	-$(RM) ./DISPLAY/Src/ILI9341.cyclo ./DISPLAY/Src/ILI9341.d ./DISPLAY/Src/ILI9341.o ./DISPLAY/Src/ILI9341.su

.PHONY: clean-DISPLAY-2f-Src

