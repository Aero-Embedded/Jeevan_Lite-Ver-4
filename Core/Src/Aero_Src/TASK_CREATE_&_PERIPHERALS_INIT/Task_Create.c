/*
 * Task_Create.c
 *
 *  Created on: Jan 18, 2023
 *      Author: asus
 */


#include "Task_Create.h"




void Initial_Task_Create()
{
		   HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&Adc.AdcData,3);
		   Blower_ON();
		   HAL_DAC_Start(&hdac,DAC1_CHANNEL_1);
		   Blower_Signal(0);
		   HAL_DAC_Start(&hdac,DAC1_CHANNEL_2);
		   Parkar_valve_Signal(0);
		   Uart_Receive_Debug_Toggle_Led();
		   PIP_Not_Acheieved();
		   Blue_Led_ON();
		   Red_Led_OFF();
		   Green_Led_OFF();

		   ADS1115_Init(1, &hi2c2);
		   ADS1115_Init(2, &hi2c3);
		   ADS1115_Init(3, &hi2c1);

		   Bluetooth_Receive = xQueueCreate(1, sizeof(Bluetooth_Parameter.Bluetooth_RX_BUF));
		   vSemaphoreCreateBinary(binarysem);

		   xTaskCreate(Sensor_Offset_Task,"Sensor-Offset-Task", 512, NULL, 4, &Sensor_Offset_Handler);
		   xTaskCreate(Time_Trace_Task,"Time-Trace-Task", 512, NULL, 3, &Time_Trace_Handler);
		   xTaskCreate(Sensor_Read_Task,"Sensor-Read-Task", 512, NULL, 3, &Sensor_Read_Handler);
		   xTaskCreate(Bluetooth_Task,"Bluetooth-Task", 512, NULL, 2, &Bluetooth_Handler);
		   xTaskCreate(BlueTooth_Receiver_Task,"BlueTooth-Data-Receive-Task", 256, NULL, 5, &Bluetooth_Receiver_Handler);
		   xTaskCreate(Bluetooth_data_split_Task,"Uart-data-split-Task", 512, NULL, 5, &Bluetooth_data_Split_Handler);
		   xTaskCreate(Mode_Initial_Flag_Set_Task,"Initial-Flag-Set-Task", 512, NULL, 4, &Mode_initial_Flag_Set_Handler);
		   xTaskCreate(One_Time_Task,"One-Time-Task", 512, NULL,3, &One_Time_Handler);
		   xTaskCreate(Pc_Cmv_Task,"Pc-Cmv-Task", 256, NULL, 2, &Pc_Cmv_Handler);
		   xTaskCreate(PIP_Control_Task,"PIP-Control-PC-CMV-Task", 512, NULL, 2, &PIP_Control_Task_Handler);
		   xTaskCreate(Vc_Cmv_Task,"Vc-Cmv-Task", 512, NULL, 2, &Vc_Cmv_Handler);
		   xTaskCreate(Volume_Control_Task,"Volume-Control-VC-CMV-Task", 512, NULL, 2, &Volume_Control_Task_Handler);
		   xTaskCreate(Oxygen_Blending_Task, "Oxygen-Blending-Task", 512, NULL, 2, &Oxygen_Blending_Handler);
		   xTaskCreate(Alert_Task, "Alert Task", 512, NULL, 2, &Alert_Handler);
		   xTaskCreate(Pc_Simv_Assist_ON_Task,"Pc-Simv-Assist-ON-Task", 512, NULL, 2, &Pc_Simv_Assist_ON_Handler);
		   xTaskCreate(Pc_Simv_Assist_OFF_Task,"Pc-Simv-Assist-OFF-Task", 512, NULL, 2, &Pc_Simv_Assist_OFF_Handler);
		   xTaskCreate(Vc_Simv_Assist_ON_Task,"Vc-Simv-Assist-ON-Task", 512, NULL, 2, &Vc_Simv_Assist_ON_Handler);
		   xTaskCreate(Vc_Simv_Assist_OFF_Task,"Vc-Simv-Assist-OFF-Task", 512, NULL, 2, &Vc_Simv_Assist_OFF_Handler);
		   xTaskCreate(PSV_Task,"PSV-Task", 256, NULL, 2, &PSV_Handler);
		   xTaskCreate(PSV_PID_Task,"PSV-PID-Task", 256, NULL, 2, &PSV_PID_Handler);
		   xTaskCreate(CPAP_Task,"CPAP-Task", 256, NULL, 2, &CPAP_Handler);
		   xTaskCreate(BIPAP_Task,"BIPAP-Task", 256, NULL, 2, &BIPAP_Handler);
		   xTaskCreate(BIPAP_PID_Task,"BIPAP-PID-Task", 256, NULL, 3, &BIPAP_PID_Handler);
		   xTaskCreate(APRV_Task,"APRV-Task", 256, NULL, 2, &APRV_Handler);
		   xTaskCreate(APRV_ONE_TIME_Task,"APRV-ONE-TIME-Task", 256, NULL, 2, &APRV_ONE_TIME_Handler);
		   xTaskCreate(BACKUP_PC_CMV_Task,"BACKUP-PC-CMV-Task", 256, NULL, 2, &BACKUP_PC_Handler);
		   xTaskCreate(Nebuliser_Task,"Nebulizer-Task", 256, NULL, 2, &Nebuliser_Handler);
	       xTaskCreate(Suction_Task,"Suction-Task", 256, NULL, 2, &Suction_Handler);
	       xTaskCreate(HFNC_Task,"HFNC-Task", 256, NULL, 2, &HFNC_Handler);
	       xTaskCreate(O2_Calibration_Task,"O2-Calibration-Task", 128, NULL, 2, &O2_Calibration_Handler);
	       xTaskCreate(CALIBRATION_Task, "CALIBRATION Task", 512, NULL, 2, &Calibration_Handler);
	       xTaskCreate(SERVICE_Task, "Service Task", 256, NULL, 2, &Service_Handler);

		   free_heap_size = xPortGetFreeHeapSize();
		   vTaskStartScheduler();
}


