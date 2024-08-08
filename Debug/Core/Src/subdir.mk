################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/freertos.c \
../Core/Src/main.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_hal_timebase_tim.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/freertos.o \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_hal_timebase_tim.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/freertos.d \
./Core/Src/main.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_hal_timebase_tim.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SERVICE" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SENSORS/READ_SENSORS_VALUES" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SENSORS/SENSORS_OFFSET" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/ALERT" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/APRV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/BACKUP_PC_CMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/BIPAP" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/Bluetooth" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/CALIBRATION" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/CPAP" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/Expiratory_Valve_Control" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/HFNC" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/NEBULISER" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/OTHERS" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/OXYGEN_BLENDING" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/PC_CMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/PC-SIMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/PIP_CONTROL" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/PSV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/Sampled_Parameter" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SENSORS" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SUCTION" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/TASK_CREATE_&_PERIPHERALS_INIT" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/VC_CMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/VC-SIMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/VOLUME_CONTROL" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_timebase_tim.d ./Core/Src/stm32f4xx_hal_timebase_tim.o ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o

.PHONY: clean-Core-2f-Src

