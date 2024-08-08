################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Aero_Src/PSV/PSV.c 

OBJS += \
./Core/Src/Aero_Src/PSV/PSV.o 

C_DEPS += \
./Core/Src/Aero_Src/PSV/PSV.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Aero_Src/PSV/%.o: ../Core/Src/Aero_Src/PSV/%.c Core/Src/Aero_Src/PSV/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SERVICE" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SENSORS/READ_SENSORS_VALUES" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SENSORS/SENSORS_OFFSET" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/ALERT" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/APRV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/BACKUP_PC_CMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/BIPAP" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/Bluetooth" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/CALIBRATION" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/CPAP" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/Expiratory_Valve_Control" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/HFNC" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/NEBULISER" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/OTHERS" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/OXYGEN_BLENDING" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/PC_CMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/PC-SIMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/PIP_CONTROL" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/PSV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/Sampled_Parameter" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SENSORS" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SUCTION" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/TASK_CREATE_&_PERIPHERALS_INIT" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/VC_CMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/VC-SIMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/VOLUME_CONTROL" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Aero_Src-2f-PSV

clean-Core-2f-Src-2f-Aero_Src-2f-PSV:
	-$(RM) ./Core/Src/Aero_Src/PSV/PSV.d ./Core/Src/Aero_Src/PSV/PSV.o

.PHONY: clean-Core-2f-Src-2f-Aero_Src-2f-PSV

