################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/my_Board/stm32f429i_discovery.c \
../Drivers/BSP/my_Board/stm32f429i_discovery_io.c \
../Drivers/BSP/my_Board/stm32f429i_discovery_lcd.c \
../Drivers/BSP/my_Board/stm32f429i_discovery_sdram.c \
../Drivers/BSP/my_Board/stm32f429i_discovery_ts.c 

C_DEPS += \
./Drivers/BSP/my_Board/stm32f429i_discovery.d \
./Drivers/BSP/my_Board/stm32f429i_discovery_io.d \
./Drivers/BSP/my_Board/stm32f429i_discovery_lcd.d \
./Drivers/BSP/my_Board/stm32f429i_discovery_sdram.d \
./Drivers/BSP/my_Board/stm32f429i_discovery_ts.d 

OBJS += \
./Drivers/BSP/my_Board/stm32f429i_discovery.o \
./Drivers/BSP/my_Board/stm32f429i_discovery_io.o \
./Drivers/BSP/my_Board/stm32f429i_discovery_lcd.o \
./Drivers/BSP/my_Board/stm32f429i_discovery_sdram.o \
./Drivers/BSP/my_Board/stm32f429i_discovery_ts.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/my_Board/%.o Drivers/BSP/my_Board/%.su Drivers/BSP/my_Board/%.cyclo: ../Drivers/BSP/my_Board/%.c Drivers/BSP/my_Board/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/BSP -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/MIDI/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc -I../Drivers/BSP/my_Board -I../Drivers/BSP/Components -I../Utilities -I../Drivers/BSP/Components/Common -I../Drivers/BSP/Components/ili9341 -I../Drivers/BSP/Components/stmpe811 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-my_Board

clean-Drivers-2f-BSP-2f-my_Board:
	-$(RM) ./Drivers/BSP/my_Board/stm32f429i_discovery.cyclo ./Drivers/BSP/my_Board/stm32f429i_discovery.d ./Drivers/BSP/my_Board/stm32f429i_discovery.o ./Drivers/BSP/my_Board/stm32f429i_discovery.su ./Drivers/BSP/my_Board/stm32f429i_discovery_io.cyclo ./Drivers/BSP/my_Board/stm32f429i_discovery_io.d ./Drivers/BSP/my_Board/stm32f429i_discovery_io.o ./Drivers/BSP/my_Board/stm32f429i_discovery_io.su ./Drivers/BSP/my_Board/stm32f429i_discovery_lcd.cyclo ./Drivers/BSP/my_Board/stm32f429i_discovery_lcd.d ./Drivers/BSP/my_Board/stm32f429i_discovery_lcd.o ./Drivers/BSP/my_Board/stm32f429i_discovery_lcd.su ./Drivers/BSP/my_Board/stm32f429i_discovery_sdram.cyclo ./Drivers/BSP/my_Board/stm32f429i_discovery_sdram.d ./Drivers/BSP/my_Board/stm32f429i_discovery_sdram.o ./Drivers/BSP/my_Board/stm32f429i_discovery_sdram.su ./Drivers/BSP/my_Board/stm32f429i_discovery_ts.cyclo ./Drivers/BSP/my_Board/stm32f429i_discovery_ts.d ./Drivers/BSP/my_Board/stm32f429i_discovery_ts.o ./Drivers/BSP/my_Board/stm32f429i_discovery_ts.su

.PHONY: clean-Drivers-2f-BSP-2f-my_Board

