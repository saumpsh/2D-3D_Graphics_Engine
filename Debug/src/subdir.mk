################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/3D-Cube_v1.c \
../src/DrawLine.c \
../src/cr_startup_lpc175x_6x.c \
../src/crp.c \
../src/lcd_ssp_Operations.c \
../src/main.c \
../src/ssp.c 

OBJS += \
./src/3D-Cube_v1.o \
./src/DrawLine.o \
./src/cr_startup_lpc175x_6x.o \
./src/crp.o \
./src/lcd_ssp_Operations.o \
./src/main.o \
./src/ssp.o 

C_DEPS += \
./src/3D-Cube_v1.d \
./src/DrawLine.d \
./src/cr_startup_lpc175x_6x.d \
./src/crp.d \
./src/lcd_ssp_Operations.d \
./src/main.d \
./src/ssp.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSIS_CORE_LPC17xx -D__LPC17XX__ -D__REDLIB__ -I"C:\Users\SaumPC\Documents\MCUXpressoIDE_10.2.1_795\workspace\3D-Cube\inc" -I"C:\Users\SaumPC\Documents\MCUXpressoIDE_10.2.1_795\workspace\CMSIS_CORE_LPC17xx\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


