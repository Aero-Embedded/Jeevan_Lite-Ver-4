/**
 *******************************************************************************
 * \file Sensor_Offset.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of Sensor offset calculation.
 *
 * This file contains the Declaration of Sensor offset calculation,
 * including the initialization.
 *******************************************************************************
 **/

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/


#include "Sensor_Offset.h"


#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"



/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/


/**
 * @brief Sensor_Offset_Task.
 * It is used to call the sensor offset function from within this task.
 */

void Sensor_Offset_Task (void *argument)
{
	while (1)
	{
		vTaskSuspend(Sensor_Read_Handler);
		vTaskSuspend(Time_Trace_Handler);
		vTaskSuspend(Bluetooth_Handler);
		vTaskSuspend(Bluetooth_data_Split_Handler);
		vTaskSuspend(Mode_initial_Flag_Set_Handler);
		vTaskSuspend(One_Time_Handler);
		vTaskSuspend(Alert_Handler);
		vTaskSuspend(Oxygen_Blending_Handler);
		vTaskSuspend(Pc_Cmv_Handler);
		vTaskSuspend(PIP_Control_Task_Handler);
		vTaskSuspend(Vc_Cmv_Handler);
		vTaskSuspend(Volume_Control_Task_Handler);
		vTaskSuspend(Pc_Simv_Assist_ON_Handler);
		vTaskSuspend(Pc_Simv_Assist_OFF_Handler);
		vTaskSuspend(Vc_Simv_Assist_ON_Handler);
		vTaskSuspend(Vc_Simv_Assist_OFF_Handler);
		vTaskSuspend(PSV_Handler);
		vTaskSuspend(PSV_PID_Handler);
		vTaskSuspend(CPAP_Handler);
		vTaskSuspend(APRV_Handler);
		vTaskSuspend(APRV_ONE_TIME_Handler);
		vTaskSuspend(BIPAP_Handler);
		vTaskSuspend(BIPAP_PID_Handler);
		vTaskSuspend(BACKUP_PC_Handler);
		vTaskSuspend(Nebuliser_Handler);
		vTaskSuspend(Suction_Handler);
		vTaskSuspend(Suction_Handler);
		vTaskSuspend(HFNC_Handler);
		vTaskSuspend(O2_Calibration_Handler);
		vTaskSuspend(Calibration_Handler);
		vTaskSuspend(Service_Handler);



        vTaskDelay(4000);
        ET_O2_Envitec_Tune_Value1   = 1;
        ET_O2_Envitec_Tune_Value2   = 4;
        ET_O2_Envitec_Tune_Value3   = 20;


		Last_O2_Dac_Val                      = 1700;

		Apnea_Mode                            = OFF;
		Device_Monitoring                     = NO;




if( (xSemaphoreTake(binarysem,4000)) == pdTRUE)
{
		CALIBRATE_SENSOR_TASK();
		Device_Control                  = PAUSE;
		INCREMENT_DAC_VAL_PER_ITERATION = 3;

		INITIAL_DAC_VALUE();
		INITIAL_CPAP_DAC_VALUE_INVASIVE();
		OLD.last_vent_type = NO_MODE;
		HFNC_O2_DAC_VALUE =0;

}
if( (xSemaphoreTake(binarysem,4000)) == pdTRUE)
{

			vTaskResume(Sensor_Read_Handler);
			vTaskResume(Time_Trace_Handler);
			vTaskResume(Bluetooth_Handler);

xSemaphoreGive(binarysem);
}
		vTaskDelete(Sensor_Offset_Handler);
	}
}


/**
 * @brief One_Time_Task.
 * It interchanges the phase of the breath cycle.
 */
void One_Time_Task(void *argument)
{
	while (1)
	{

		switch (Next_Half_Breath_Cycle)
		{
		case Generate_Inspiration_Cycle:
			switch (Mode)
			{
				case Pccmv:
					PC_CMV_Inspiration_Time_Variables_Reset();
				break;
				case Vccmv:
					VC_CMV_Inspiration_Time_Variables_Reset();
				break;
				case Pcsimv:
					PC_SIMV_Inspiration_Time_Variables_Reset();
				break;
				case Vcsimv:
					VC_SIMV_Inspiration_Time_Variables_Reset();
				break;
				case Backup_Pccmv:
					BACKUP_PC_CMV_Inspiration_Time_Variables_Reset();
				break;
				case Idle:
					GRAPH_VALUES.volume = 0;
					vTaskDelay(20);
				break;
				default:
					GRAPH_VALUES.volume = 0;
					vTaskDelay(20);
				break;
			}
		break;

		case Generate_Expiration_Cycle:
			switch (Mode)
			{
				case Pccmv:
					PC_CMV_Expiration_Time_Variables_Reset();
					break;
				case Vccmv:
					VC_CMV_Expiration_Time_Variables_Reset();
				break;
				case Pcsimv:
					PC_SIMV_Expiration_Time_Variables_Reset();
				break;
				case Vcsimv:
					VC_SIMV_Expiration_Time_Variables_Reset();
				break;
				case Backup_Pccmv:
					BACKUP_PC_CMV_Expiration_Time_Variables_Reset();
				break;
				case Idle:
					GRAPH_VALUES.volume = 0;
					vTaskDelay(20);
				break;
				default:
					GRAPH_VALUES.volume = 0;
					vTaskDelay(20);
					break;
			}
			break;

		case NoWaveFormState:
			break;
		default:
			break;

		}
	}
}
