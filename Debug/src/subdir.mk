################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/TIMhelper.c \
../src/commande.c \
../src/configurations.c \
../src/custom_can.c \
../src/custom_codeur.c \
../src/custom_gpio.c \
../src/custom_handler.c \
../src/custom_nvic.c \
../src/debugger.c \
../src/main.c \
../src/state.c \
../src/stm32f4xx_it.c \
../src/struct_can_msg.c \
../src/syscalls.c \
../src/system_stm32f4xx.c 

OBJS += \
./src/TIMhelper.o \
./src/commande.o \
./src/configurations.o \
./src/custom_can.o \
./src/custom_codeur.o \
./src/custom_gpio.o \
./src/custom_handler.o \
./src/custom_nvic.o \
./src/debugger.o \
./src/main.o \
./src/state.o \
./src/stm32f4xx_it.o \
./src/struct_can_msg.o \
./src/syscalls.o \
./src/system_stm32f4xx.o 

C_DEPS += \
./src/TIMhelper.d \
./src/commande.d \
./src/configurations.d \
./src/custom_can.d \
./src/custom_codeur.d \
./src/custom_gpio.d \
./src/custom_handler.d \
./src/custom_nvic.d \
./src/debugger.d \
./src/main.d \
./src/state.d \
./src/stm32f4xx_it.d \
./src/struct_can_msg.d \
./src/syscalls.d \
./src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c11 -DSTM32F4DISCOVERY -DSTM32F407VGTx -DSTM32 -DSTM32F4 -DDEBUG -DUSE_STDPERIPH_DRIVER -DSTM32F40XX -DSTM32F40_41xxx -I"/home/pooki/Documents/code/Mips/STM32 PMSM FOC LIB/stm32f4discovery_stdperiph_lib" -I"/home/pooki/Documents/code/Mips/STM32 PMSM FOC LIB/Rebuild/inc" -I"/home/pooki/Documents/code/Mips/STM32 PMSM FOC LIB/stm32f4discovery_stdperiph_lib/StdPeriph_Driver/inc" -I"/home/pooki/Documents/code/Mips/STM32 PMSM FOC LIB/stm32f4discovery_stdperiph_lib/Utilities" -I"/home/pooki/Documents/code/Mips/STM32 PMSM FOC LIB/stm32f4discovery_stdperiph_lib/CMSIS/core" -I"/home/pooki/Documents/code/Mips/STM32 PMSM FOC LIB/stm32f4discovery_stdperiph_lib/CMSIS/device" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


