/**
 ******************************************************************************
 * @file HFNC.c
 * @author Sarath Kumar
 * @date 27/03/24
 * @brief Implementation of HFNC Feature.
 *
 * This file contains the implementation of HFNC control Function, Including HFNC Data Split and
 * Alert Control.
 ******************************************************************************
 **/



/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include "HFNC.h"


/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

int O2_supply_error_count;
uint16_t HFNC_DAC_VALUE_TEMP;

/**
* @brief O2 Supply Alert
* @param none
**/
static void Check_O2_Supply();
/**
* @brief Tune a blower dac value
* @param *Last_Blower_DAC_Val last used blower value.
* @param Set_Flow set flow.
* @return uint16_t Blower dac value is passed.
**/
static uint16_t HFNC_TUNE_DAC_VALUE(uint16_t *,uint8_t);

float Cummulative_Flow_Value;                  ///< Cummulative_Flow_Value.
int Flow_count;                                ///< Flow_count.
uint16_t LAST_HFNC_BLOWER_DAC_VALUE = 400;     ///< last hfnc blower dac value.
uint16_t HFNC_BLOWER_DAC_VALUE;                ///< current blower dac value.
int Check_O2= 3000;                            ///< check O2.



/**
 * @brief HFNC_Task.
 * This Task Control blower value to reach a Required Flow rate
 * with maintain a Required Oxygen.
 * @param none.
 * @post none.
  **/

void HFNC_Task (void *argument)
{
	while (1)
	{


		if(  (Oxygen_Parameter.Achieved_Oxygen < HFNC._FIO2_Val) )
		{
			HFNC_O2_DAC_VALUE=controlsys_Update(&Parker_valve_tune, HFNC._FIO2_Val,Oxygen_Parameter.Achieved_Oxygen);
			Parkar_valve_Signal( HFNC_O2_DAC_VALUE);
		}
		else
		{
			HFNC_O2_DAC_VALUE=controlsys_Update(&Parker_valve_tune, HFNC._FIO2_Val,Oxygen_Parameter.Achieved_Oxygen);
			Parkar_valve_Signal( HFNC_O2_DAC_VALUE);
		}
		Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;
		if(Check_O2==0)
		{
			Check_O2_Supply();
		}
		else
		{
			Check_O2  = (Check_O2 -2);
			if(Check_O2 <0)
				Check_O2 = 0;

		}
		Flow_count++;
		Cummulative_Flow_Value += GRAPH_VALUES.flow;
		if(Flow_count >= 37)
		{
			FLOW_AVG                      = ((float)Cummulative_Flow_Value / (float)Flow_count);
			LAST_HFNC_BLOWER_DAC_VALUE    = HFNC_TUNE_DAC_VALUE(&HFNC_BLOWER_DAC_VALUE ,HFNC._Flow_Rate );

			if(LAST_HFNC_BLOWER_DAC_VALUE > 3000)
				LAST_HFNC_BLOWER_DAC_VALUE = 3000;
			else if(LAST_HFNC_BLOWER_DAC_VALUE < 100)
				LAST_HFNC_BLOWER_DAC_VALUE = 100;
			HFNC_BLOWER_DAC_VALUE         = LAST_HFNC_BLOWER_DAC_VALUE;
			Cummulative_Flow_Value        = 0;
			Flow_count                    = 0;

			/* Set PD5 High - Green LED ON*/
			Green_Led_ON();
			Blue_Led_OFF();
			Red_Led_OFF();

			Send_Frame = First_Frame;
			Send_Bluetooth_Data_Farme = SEND_SAMPLED_PARAMETER_FRAME;
		}
		Blower_Signal(HFNC_BLOWER_DAC_VALUE);
		ExpValve_CLOSE();
		vTaskDelay(70);
	}

}

/**
* @brief HFNC data Split
*
* This function Split HFNC MODE Data from Blue-tooth.
* @param RECEIVE_GRAPH_PACKET data from blue-tooth.
* @post HFNC mode data is Separated.
**/

void HFNC_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet)
{
	controlsys_Init(&Parker_valve_tune);          //parker


	HFNC._Flow_Rate  = (Receive_Graph_Packet ->Flow_Limit_FiO2_Flow_Limit);
	HFNC._FIO2_Val   = (Receive_Graph_Packet ->FiO2);


	vTaskSuspend(Vc_Cmv_Handler);
	vTaskSuspend(Volume_Control_Task_Handler);
	vTaskSuspend(One_Time_Handler);
	vTaskSuspend(Pc_Cmv_Handler);
	vTaskSuspend(PIP_Control_Task_Handler);
	vTaskSuspend(Pc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Pc_Simv_Assist_ON_Handler);
	vTaskSuspend(Vc_Simv_Assist_ON_Handler);
	vTaskSuspend(Vc_Simv_Assist_OFF_Handler);
	vTaskSuspend(BACKUP_PC_Handler);
	vTaskSuspend(PSV_Handler);
	vTaskSuspend(PSV_PID_Handler);
	vTaskSuspend(CPAP_Handler);
	vTaskSuspend(BIPAP_Handler);
	vTaskSuspend(BIPAP_PID_Handler);
	vTaskSuspend(APRV_Handler);
	vTaskSuspend(APRV_ONE_TIME_Handler);
	vTaskSuspend(Nebuliser_Handler);
	vTaskSuspend(Suction_Handler);
	vTaskSuspend(HFNC_Handler);
	ExpValve_OPEN();
	Blower_Signal( 0);
	Parkar_valve_Signal(0);
	Nebuliser_OFF();
	HFNC_O2_DAC_VALUE=0;
	Parkar_valve_Signal(0);
	Flow_count                       = 0;
	HFNC_BLOWER_DAC_VALUE            = (10.6*HFNC._Flow_Rate)+82.1;
	GRAPH_VALUES.Maximum_oxygen_flow = 0;
	Oxygen_Parameter.Achieved_Oxygen = 90;
	Check_O2                         = 1000;
	vTaskResume(HFNC_Handler);
}


/**
* @brief Check_O2_Supply
* This function checks for disconnection of the O2 supply.
**/

void Check_O2_Supply()
{
	    if(GRAPH_VALUES.oxygen_flow<=0)
		{
			O2_supply_error_count++;

			if(O2_supply_error_count >= 43)
			{
				Clear_All_Alert_Bits();

				SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_OXYGEN_SUPPLY_FAILED);
				Send_Bluetooth_Data_Farme = SEND_ALERT_FRAME;

				Red_Led_ON();
				Blue_Led_OFF();
				Green_Led_OFF();

				Flow_count            = 0;
				O2_supply_error_count = 0;
			}
		}
}

/**
* @brief Tune a blower dac value
*
* This function alters the blower value to achieve the required flow rate.
* @param *Last_Blower_DAC_Val last used blower value.
* @param Set_Flow set flow.
* @return uint16_t Blower dac value is passed.
**/

static uint16_t HFNC_TUNE_DAC_VALUE(uint16_t *Last_Blower_DAC_Val,uint8_t Set_Flow)
{
	if(  ( FLOW_AVG)  > ( Set_Flow ) && (FLOW_AVG  < (Set_Flow + 2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-10);
	}
	else if(  ( FLOW_AVG)  >= ( Set_Flow + 2 ) && (FLOW_AVG  < (Set_Flow + 4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-20);
	}
	else if(  ( FLOW_AVG)  >= ( Set_Flow + 4 )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-40);
	}
	else if( (FLOW_AVG  < Set_Flow) && (FLOW_AVG  > (Set_Flow -2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+10);
	}
	else if(  (FLOW_AVG  <= (Set_Flow-2) ) && (FLOW_AVG  >  (Set_Flow-4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+20);
	}
	else if(  (FLOW_AVG  <= (Set_Flow-4) )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+40);
	}
	else
	{
		*Last_Blower_DAC_Val = *Last_Blower_DAC_Val;
	}
	return *Last_Blower_DAC_Val;
}
