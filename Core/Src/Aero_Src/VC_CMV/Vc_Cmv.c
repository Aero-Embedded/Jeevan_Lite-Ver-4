/**
 *******************************************************************************
 * \file Vc_cmv.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief implementation of VCCMV mode.
 *
 * This file contains the implementation VCCMV mode variables
 *******************************************************************************
 **/

#include "Vc_Cmv.h"


static void Inspiration_Time_Blower_Control_VC_CMV(uint16_t);
static void Expiration_Time_Blower_Control_VC_CMV(uint16_t,float,uint16_t);
static void CHECK_Flow_Increment();
static void Check_Flow_Graph(int8_t);

uint8_t Minimum_Increment_Limit=3;            ///< minimum value of increment dac value.
uint8_t Maximum_Increment_Limit=20;           ///< maximum value of increment dac value.
uint8_t Stop_blower_Performance_Value = 15;   ///< stop incrementing dac value.

/**
 *@brief Vc_Cmv_Mode_Packet_Data.
 * This task is to extract the VCCMV mode variable from NRF Bluetooth.
 * @param RECEIVE_GRAPH_PACKET structure pointer.
 */

void Vc_Cmv_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet)
{
	uint32_t One_Breathe_time;
	VC_CMV.VOLUME_Val             = Receive_Graph_Packet->VTI_VS;
	VC_CMV.PEEP_Val               = Receive_Graph_Packet->PEEP_CPAP_Plow;
	VC_CMV.FIO2_Val               = Receive_Graph_Packet->FiO2;
	VC_CMV.RESPIRATORY_RATE_Val   = Receive_Graph_Packet->RR;
	One_Breathe_time              = (One_Minite_In_MS / VC_CMV.RESPIRATORY_RATE_Val);
	VC_CMV.INSPIRATION_TIME       = ( ( Receive_Graph_Packet->T_high) * 100 );
	VC_CMV.EXPIRATION_TIME        = (One_Breathe_time - VC_CMV.INSPIRATION_TIME);
	VC_CMV.FLOW_RATE_Val          = Receive_Graph_Packet->Flow_Limit_FiO2_Flow_Limit;

	Alert_Check_Time              = ((float)VC_CMV.INSPIRATION_TIME/3);
	Vent_Type                     = ET_TUBE_MODE;

	OOM202.offset_minimum         = ( 0x7F & (Receive_Graph_Packet->Vent_Type_Min_O2));
	OOM202.offset_maximum         = Receive_Graph_Packet->Max_O2;

	Mode_Initial_Flag_Set         = OPEN;
	vTaskResume(Mode_initial_Flag_Set_Handler);
}


/**
 * @brief Vc_Cmv_Task.
 * This Task calls  two function based on inspiration and expiration time.
 * */

void Vc_Cmv_Task (void *argument)
{

	while(1)
	{
		switch (Run_Current_Breathe_State)
		{
			case Run_Inspiration_Cycle:
				Inspiration_Time_Blower_Control_VC_CMV(Volume_Mode_Blower_control.BLOWER_DAC_VAL);
			break;
			case Run_Expiration_Cycle:
				    	Volume_Mode_Blower_control.BLOWER_DAC_VAL=0;
						Expiration_Time_Blower_Control_VC_CMV(Volume_Mode_Blower_control.BLOWER_DAC_VAL,
								   	   	   	   	   	          PEEP_Maintain_Parameter.PEEP_Temp,
								                              PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time);
			break;
			case No_Run_State:
			break;
			default:
			break;
		}

		vTaskDelay(2);
	}
}


/**
 * @brief Inspiration_Time_Blower_Control_VC_CMV.
 * This function uploads a DAC value to the DAC1 peripheral to control a blower every 2 milliseconds during inspiration time.
 * @param Blower_Signal as Pressure_Mode_blower_control.BLOWER_DAC_VAL.
 * */


static void Inspiration_Time_Blower_Control_VC_CMV(uint16_t Blower_Signal)
{
	Blower_Signal(Blower_Signal);
}



/**
 * @brief Expiration_Time_Blower_Control_VC_CMV.
 * This function uploads a DAC value to the DAC1 peripheral to control a blower every 2 milliseconds during inspiration time.
 * @param  Blower_Signal as Pressure_Mode_blower_control.BLOWER_DAC_VAL.
 * @param  PEEP to lock the expiratory valve in peep area.
 * @param  Expiratory_Valve_Open_Time to control the expiratory valve lock delay.
 * */


static void Expiration_Time_Blower_Control_VC_CMV(uint16_t Blower_Signal,float PEEP,uint16_t Expiratory_Valve_Open_Time)
{
	Blower_Signal( Blower_Signal);
		if(Peep_Status==PEEP_ACHEIVED)
		{
				if(GRAPH_VALUES.pressure < PEEP)
				{
					Blower_Signal( Blower_Signal);
				}
				else
				{
					Blower_Signal( Blower_Signal);
				}
		 }
		if(GRAPH_VALUES.pressure<= PEEP)
		{
			if(Expiratory_Valve_Lock_Delay==OPENED)
			{
				//Blower_OFF();
				Expiratory_Valve_Lock_Delay=CLOSED;
				vTaskDelay(Expiratory_Valve_Open_Time);
			}
			Peep_Status=PEEP_ACHEIVED;
			ExpValve_CLOSE();
		}
		else if(Peep_Status==PEEP_NOT_ACHEIVED)
		{
			ExpValve_OPEN();
		}
}



/**
 * @brief VC_CMV_Inspiration_Time_Variables_Reset.
 * This function resets variables to start an inspiration cycle.
 * */

void VC_CMV_Inspiration_Time_Variables_Reset()
{
	Oxygen_Blending_Status=OXYGEN_ACHIEVED;
	Find_Average_Flow();
	Breath_Count                       = Count_The_Breath;
	if( (VC_CMV.PEEP_Val == PEEP_Maintain_Parameter.PEEP_AVG_VAL ) )
	{
	   CHECK_Flow_Increment();
	}
	else
	{
		Flow_increment       = Flow_fine;
		memset(Volume_Array, 0,(VOL_ARR_SIZE*sizeof(Volume_Array[0])) );
		Volume_Array_count = 0;
	}
	Reset_Blower_Status();
	Volume_Not_Acheieved();
	ExpValve_CLOSE();
	PIP_AVERAGE();
	PEEP_AVERAGE();
	O2_DAC_Control(Common_Mode_Parameter._FIO2_Val);
	Check_Alert();
	Alert_Inspiration_Time_Parameter();
	GRAPH_VALUES.Maximum_oxygen_flow   = 0;                 //new
	LED_Alert();
	Alert_Status = NO_ALERT;
	Sampled_Parameter_Inspiration_time();
	if( (VC_CMV.PEEP_Val != PEEP_Maintain_Parameter.PEEP_AVG_VAL ) )
	{
		PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time     = PEEP_Control_For_Expiratory_Valve_Open_Time_In_Volume_Mode(PEEP_Maintain_Parameter.PEEP_AVG_VAL,
																												           VC_CMV.RESPIRATORY_RATE_Val,
													                                                                       VC_CMV.INSPIRATION_TIME,
													                                                                       VC_CMV.PEEP_Val,
																												           PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time);

	}
    VOLUME_STOP_POINT(Common_Mode_Parameter._VOLUME_Val,Volume_control_Task.Achieved_Volume);
	if(Stop_Increment_Count <= Stop_blower_Performance_Value)
	{
			Volume_Mode_Blower_control.INITIAL_BLOWER_DAC_VAL          =  TUNE_INITIAL_DAC_VAL(Volume_Mode_Blower_control.PREVIOUS_INITIAL_BLOWER_DAC_VAL,
    		                                                                           Common_Mode_Parameter._FLOW_RATE,
																					   Volume_control_Task.Flow_Max_Achieved);
			Volume_Mode_Blower_control.PREVIOUS_INITIAL_BLOWER_DAC_VAL = Volume_Mode_Blower_control.INITIAL_BLOWER_DAC_VAL;
	}
	else
	{
		Volume_Mode_Blower_control.INITIAL_BLOWER_DAC_VAL             = Volume_Mode_Blower_control.PREVIOUS_INITIAL_BLOWER_DAC_VAL;
		Volume_Mode_Blower_control.PREVIOUS_INITIAL_BLOWER_DAC_VAL = Volume_Mode_Blower_control.INITIAL_BLOWER_DAC_VAL;
	}
    LOW_COMPLIANCE_DAC                                         = LOW_Compliance_DAC_value(Common_Mode_Parameter._FLOW_RATE);
	BREATH_STATE                                               = INSPIRATION_CYCLE;
	Peep_Status                   				               = PEEP_NOT_ACHEIVED;
	Expiratory_Valve_Lock_Delay   			                   = OPENED;
	PIP_Average_Parameter.Maximum_PIP_Acheived                 = RESET;
	Maximum_Flow_Achieved                                      = FLOW_NOT_ACHIEVED;
	Volume_control_Task.Flow_Max_Achieved                      = RESET;
	Volume_control_Task.LOW_Flow_Max_Achieved                  = RESET;
	Volume_Achieved_Status                                     = VOLUME_NOT_ACHIEVED;
	TIME_Base_parameter._INSPIRATION_TIME                      = VC_CMV.INSPIRATION_TIME;
	Volume_Achieved_Time                                       = 0;
	Total_Volume_Achieved_Time                                 = 0;
	TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED              = RESET;
	GRAPH_VALUES.volume                                        = RESET;
	Volume_max                                                 = RESET;
	Next_Half_Breath_Cycle                                     = Generate_Expiration_Cycle;
	Run_Current_Breathe_State                                  = Run_Inspiration_Cycle;
	vTaskDelay(VC_CMV.INSPIRATION_TIME);
}

/**
 * @brief VC_CMV_Expiration_Time_Variables_Reset.
 * This function resets variables to start an expiration cycle.
 * */

void VC_CMV_Expiration_Time_Variables_Reset()
{
	Volume_control_Task.Achieved_Volume                 = GRAPH_VALUES.volume ;
	BREATH_STATE                                        = EXPIRATION_CYCLE;
	PEEP_Maintain_Parameter.PEEP_Cumulative_Val_Trigger = RESET;
	PEEP_Maintain_Parameter.PEEP_Event_Count_Trigger    = RESET;
	PEEP_Maintain_Parameter.PEEP_Cumulative_Val         = RESET;
	PEEP_Maintain_Parameter.PEEP_Event_Count            = RESET;
	Calculate_O2(Common_Mode_Parameter._FIO2_Val);
	Alert_Expiration_Time_Parameter();
	TIME_Base_parameter._EXPIRATION_TIME                = VC_CMV.EXPIRATION_TIME;
	TIME_Base_parameter.EXPIRATION_TIME_ACHEIVED        = RESET;
	Next_Half_Breath_Cycle                              = Generate_Inspiration_Cycle;
	Run_Current_Breathe_State                           = Run_Expiration_Cycle;
	vTaskDelay(VC_CMV.EXPIRATION_TIME);
}


/**
 * @brief CHECK_Flow_Increment.
 * This function identifies the flow achieved in the last breath cycle.
 * */

static void CHECK_Flow_Increment()
{


	if(Stop_Increment_Count <= Stop_blower_Performance_Value)
	{
	   if( Flow_increment == Low_Flow_Problem )
	   {
			   INCREMENT_DAC_VAL_PER_ITERATION = (INCREMENT_DAC_VAL_PER_ITERATION + 1);
				   if(INCREMENT_DAC_VAL_PER_ITERATION >Maximum_Increment_Limit)
				      INCREMENT_DAC_VAL_PER_ITERATION=Maximum_Increment_Limit;

	   }
	   else if(Flow_increment == High_Flow_Problem)
	   {
		   INCREMENT_DAC_VAL_PER_ITERATION = (INCREMENT_DAC_VAL_PER_ITERATION - 1);
		       if(INCREMENT_DAC_VAL_PER_ITERATION <Minimum_Increment_Limit)
		    	   INCREMENT_DAC_VAL_PER_ITERATION=Minimum_Increment_Limit;
	   }

	    Last_Flow_increment = Flow_increment;

	    Set_Flow_value       = Not_Detect;
		Flow_increment       = Flow_fine;
		Check_Flow_Graph(VC_CMV.FLOW_RATE_Val);
	}

	memset(Volume_Array, 0,(VOL_ARR_SIZE*sizeof(Volume_Array[0])) );
	Volume_Array_count   = 0;

}



/**
 * @brief Check_Flow_Graph.
 * This function identifies the flow achieved in the last breath cycle using array element.
 * */


static void Check_Flow_Graph(int8_t flow_rate)
{
	Low_Flow_Rate = flow_rate - 17;

	if(Low_Flow_Rate <= 1)
	{
		Low_Flow_Rate = 1;
	}
	for(int i=0;i<VOL_ARR_SIZE;i++)
	{
		if( Volume_Array[i] == flow_rate)
		{
			Set_Flow_value = Detect;
		}
		if(Set_Flow_value == Detect)
		{
			if( (Volume_Array[i] > (flow_rate+7)))                                 //new
			{
				Flow_increment = High_Flow_Problem;
			}
			else if( (Volume_Array[i] < (Low_Flow_Rate))  && (Volume_Array[i] !=0) )  //new line changed as ulta
			{
				Flow_increment = Low_Flow_Problem;
			}
		}
	}

	if(  (Last_Flow_increment != Flow_increment) && (Flow_increment != Flow_fine)  && (Last_Flow_increment != Flow_fine) )
	{
		if(Stop_Increment_Count <= Stop_blower_Performance_Value)
		{
			Stop_Increment_Count++;
		}
	}


}


/**
 * @brief Reset_Blower_Status.
 * This function Reset the blower from abnormal breath
 * */

void Reset_Blower_Status()
{
	Blower_Status       = BlOWER_ON;
	Current_Pressure    = 0;

	if(Over_Pressure_Count > 10)
	{
		Over_Pressure_Count = 0;
	}
}


void Volume_Not_Acheieved()
{

}


void Volume_Acheieved()
{

}




