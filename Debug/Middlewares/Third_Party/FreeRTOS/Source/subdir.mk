################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
../Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
../Middlewares/Third_Party/FreeRTOS/Source/list.c \
../Middlewares/Third_Party/FreeRTOS/Source/queue.c \
../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
../Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
../Middlewares/Third_Party/FreeRTOS/Source/timers.c 

OBJS += \
./Middlewares/Third_Party/FreeRTOS/Source/croutine.o \
./Middlewares/Third_Party/FreeRTOS/Source/event_groups.o \
./Middlewares/Third_Party/FreeRTOS/Source/list.o \
./Middlewares/Third_Party/FreeRTOS/Source/queue.o \
./Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.o \
./Middlewares/Third_Party/FreeRTOS/Source/tasks.o \
./Middlewares/Third_Party/FreeRTOS/Source/timers.o 

C_DEPS += \
./Middlewares/Third_Party/FreeRTOS/Source/croutine.d \
./Middlewares/Third_Party/FreeRTOS/Source/event_groups.d \
./Middlewares/Third_Party/FreeRTOS/Source/list.d \
./Middlewares/Third_Party/FreeRTOS/Source/queue.d \
./Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.d \
./Middlewares/Third_Party/FreeRTOS/Source/tasks.d \
./Middlewares/Third_Party/FreeRTOS/Source/timers.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FreeRTOS/Source/%.o: ../Middlewares/Third_Party/FreeRTOS/Source/%.c Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SERVICE" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SENSORS/READ_SENSORS_VALUES" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SENSORS/SENSORS_OFFSET" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/ALERT" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/APRV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/BACKUP_PC_CMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/BIPAP" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/Bluetooth" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/CALIBRATION" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/CPAP" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/Expiratory_Valve_Control" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/HFNC" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/NEBULISER" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/OTHERS" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/OXYGEN_BLENDING" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/PC_CMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/PC-SIMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/PIP_CONTROL" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/PSV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/Sampled_Parameter" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SENSORS" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/SUCTION" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/TASK_CREATE_&_PERIPHERALS_INIT" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/VC_CMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/VC-SIMV" -I"D:/Embedded/Codings/STM32/Aero_Embedded/Jeevan_Lite/Core/Inc/Aero_Inc/VOLUME_CONTROL" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-FreeRTOS-2f-Source

clean-Middlewares-2f-Third_Party-2f-FreeRTOS-2f-Source:
	-$(RM) ./Middlewares/Third_Party/FreeRTOS/Source/croutine.d ./Middlewares/Third_Party/FreeRTOS/Source/croutine.o ./Middlewares/Third_Party/FreeRTOS/Source/event_groups.d ./Middlewares/Third_Party/FreeRTOS/Source/event_groups.o ./Middlewares/Third_Party/FreeRTOS/Source/list.d ./Middlewares/Third_Party/FreeRTOS/Source/list.o ./Middlewares/Third_Party/FreeRTOS/Source/queue.d ./Middlewares/Third_Party/FreeRTOS/Source/queue.o ./Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.d ./Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.o ./Middlewares/Third_Party/FreeRTOS/Source/tasks.d ./Middlewares/Third_Party/FreeRTOS/Source/tasks.o ./Middlewares/Third_Party/FreeRTOS/Source/timers.d ./Middlewares/Third_Party/FreeRTOS/Source/timers.o

.PHONY: clean-Middlewares-2f-Third_Party-2f-FreeRTOS-2f-Source

