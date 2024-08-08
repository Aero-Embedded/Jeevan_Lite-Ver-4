/**
 *******************************************************************************
 * \file Vc_Simv_Assist_ON.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief implementation of VC SIMV ASSSIT ON mode.
 *
 * This file contains the implementation VCCMV ASSSIT ON mode variables
 *******************************************************************************
 **/


#include "Vc_Simv_Assist_ON.h"


static void Inspiration_Time_Blower_Control_VC_SIMV_ASSIST_ON(uint16_t);
static void Expiration_Time_Blower_Control_VC_SIMV_ASSIST_ON(uint16_t,float,uint16_t);
static void CHECK_TRIGGER_ASSIST_ON(uint16_t);
static void Switch_TASK_Inspiration_CYCLE();
static void CHECK_PRESSURE_TRIGGER();
static void CHECK_FLOW_TRIGGER();
static void CHECK_TRIGGER_OFFSET();
static void CHECK_Flow_Increment();
static void Check_Flow_Graph(int8_t);
static void Trigger_Time_Monitor();

extern uint16_t Expiratory_Valve_Open_Time;
extern uint8_t Stop_blower_Performance_Value;


extern uint8_t Minimum_Increment_Limit;
extern uint8_t Maximum_Increment_Limit;



/**
 *@brief Vc_Simv_Mode_Packet_Data.
 * This task is to extract the VC-SIMV mode variable from NRF Bluetooth.
 * @param RECEIVE_GRAPH_PACKET structure pointer.
 */

void Vc_Simv_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet)
{
	uint32_t One_Breathe_time;
	VC_SIMV.VOLUME_Val             = Receive_Graph_Packet->VTI_VS;
	VC_SIMV.PEEP_Val               = Receive_Graph_Packet->PEEP_CPAP_Plow;
	VC_SIMV.FIO2_Val               = Receive_Graph_Packet->FiO2;

	VC_SIMV.RESPIRATORY_RATE_Val   = Receive_Graph_Packet->RR;
	One_Breathe_time               = (One_Minite_In_MS / VC_SIMV.RESPIRATORY_RATE_Val);
	VC_SIMV.INSPIRATION_TIME       = ( ( Receive_Graph_Packet->T_high) * 100 );
	VC_SIMV.EXPIRATION_TIME        = (One_Breathe_time - VC_SIMV.INSPIRATION_TIME);

	VC_SIMV.FLOW_RATE_Val          = Receive_Graph_Packet->Flow_Limit_FiO2_Flow_Limit;


	VC_SIMV.Assist_Control    	   = (0x80 & (Receive_Graph_Packet->Control_Byte))>>7;
	VC_SIMV.TRIG_TYPE     	       = (0x40 & (Receive_Graph_Packet->Control_Byte))>>6;
	VC_SIMV.TRIG_LMT     	       = ( (Receive_Graph_Packet->Trigger_Limit) / 10);
	VC_SIMV.TRIG_TIME     	       = ( (Receive_Graph_Packet->Trigger_Time) * 100);

	VC_SIMV.CALC_TRIG_VAL 	                 =  (VC_SIMV.EXPIRATION_TIME   - Expiratory_Valve_Open_Time );
	if(VC_SIMV.CALC_TRIG_VAL <0)
	{
		VC_SIMV.CALC_TRIG_VAL = (VC_SIMV.CALC_TRIG_VAL * (-1));
	}

	VC_SIMV.TOLERANCE_EWAVE =  VC_SIMV.TRIG_TIME;
	if(VC_SIMV.CALC_TRIG_VAL < VC_SIMV.TRIG_TIME )
	{
		VC_SIMV.TOLERANCE_EWAVE =  (VC_SIMV.TRIG_TIME - 200) ;
	}
	VC_SIMV.TRIG_WINDOW		                 = VC_SIMV.TOLERANCE_EWAVE;

	Alert_Check_Time              = ((float)VC_SIMV.INSPIRATION_TIME/3);

	if(VC_SIMV.Assist_Control == 1)
		Assist_Control=ASSIST_ON;
	else if(VC_SIMV.Assist_Control == 0)
		Assist_Control=ASSIST_OFF;
	if(VC_SIMV.TRIG_TYPE == 0)
		Trigger_Type = Pressure_Trigger;
	else if(VC_SIMV.TRIG_TYPE == 1)
		Trigger_Type = Flow_Trigger;

	Vent_Type                     = ET_TUBE_MODE;
	OOM202.offset_minimum         = ( 0x7F & (Receive_Graph_Packet->Vent_Type_Min_O2));
	OOM202.offset_maximum         = Receive_Graph_Packet->Max_O2;

	Mode_Initial_Flag_Set                        = OPEN;
	vTaskResume(Mode_initial_Flag_Set_Handler);
}


/**
 * @brief Vc_Simv_Assist_ON_Task.
 * This Task calls  two function based on inspiration and expiration time.
 * */
void Vc_Simv_Assist_ON_Task(void *argument)
{
	while(1)
	{
		switch (Run_Current_Breathe_State)
		{
			case Run_Inspiration_Cycle:
				Inspiration_Time_Blower_Control_VC_SIMV_ASSIST_ON(Volume_Mode_Blower_control.BLOWER_DAC_VAL);

			break;
			case Run_Expiration_Cycle:
				Volume_Mode_Blower_control.BLOWER_DAC_VAL=0;
				Expiration_Time_Blower_Control_VC_SIMV_ASSIST_ON(Volume_Mode_Blower_control.BLOWER_DAC_VAL,
													  PEEP_Maintain_Parameter.PEEP_Temp,
													  PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time);
			break;
			case No_Run_State:
			break;
			default:
			break;
		}

		vTaskDelay(Two_Millisecond_Delay);
	}
}


/**
 * @brief VC_SIMV_Inspiration_Time_Variables_Reset.
 * This function decides which function needs to be called based on assist control.
 * */

void VC_SIMV_Inspiration_Time_Variables_Reset()
{
	if(Assist_Control == ASSIST_ON)
	{
		VC_SIMV_Assist_ON_Inspiration_Time_Variables_Reset();
	}
	else if(Assist_Control == ASSIST_OFF)
	{
		VC_SIMV_Assist_OFF_Inspiration_Time_Variables_Reset();
	}
}


/**
 * @brief VC_SIMV_Expiration_Time_Variables_Reset.
 * This function decides which function needs to be called based on assist control.
 * */
void VC_SIMV_Expiration_Time_Variables_Reset()
{
	if(Assist_Control == ASSIST_ON)
	{
		VC_SIMV_Assist_ON_Expiration_Time_Variables_Reset();
	}
	else if(Assist_Control == ASSIST_OFF)
	{
		VC_SIMV_Assist_OFF_Expiration_Time_Variables_Reset();
	}
}

/**
 * @brief Inspiration_Time_Blower_Control_VC_SIMV.
 * This function uploads a DAC value to the DAC1 peripheral to control a blower every 2 milliseconds during inspiration time.
 * @param Blower_Signal as Pressure_Mode_blower_control.BLOWER_DAC_VAL.
 * */
static void Inspiration_Time_Blower_Control_VC_SIMV_ASSIST_ON(uint16_t Blower_Signal)
{
	Blower_Signal(Blower_Signal);
}


/**
 * @brief Expiration_Time_Blower_Control_VC_SIMV.
 * This function uploads a DAC value to the DAC1 peripheral to control a blower every 2 milliseconds during inspiration time.
 * @param  Blower_Signal as Pressure_Mode_blower_control.BLOWER_DAC_VAL.
 * @param  PEEP to lock the expiratory valve in peep area.
 * @param  Expiratory_Valve_Open_Time to control the expiratory valve lock delay.
 * */

static void Expiration_Time_Blower_Control_VC_SIMV_ASSIST_ON(uint16_t Blower_Signal,float PEEP,uint16_t Expiratory_Valve_Open_Time)
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

		CHECK_TRIGGER_ASSIST_ON(VC_SIMV.TRIG_WINDOW);
}

/**
 * @brief CHECK_TRIGGER_ASSIST_ON.
 * This function senses the type of patient trigger that occurred during breath
 * */
static void CHECK_TRIGGER_ASSIST_ON(uint16_t Trigger_Check_Time)
{
	if ( (TIME_Base_parameter._EXPIRATION_TIME <= Trigger_Check_Time) && (TIME_Base_parameter._EXPIRATION_TIME != 0))
	{
		if (Trigger_Type == Pressure_Trigger)
		{
			CHECK_PRESSURE_TRIGGER();
		}
		else if (Trigger_Type == Flow_Trigger)
		{
			CHECK_FLOW_TRIGGER();
		}
	}

	else
	{
		CHECK_TRIGGER_OFFSET();
	}
}

/**
 * @brief Switch_TASK_Inspiration_CYCLE.
 * This function deletes and creates a one-time task to deliver a breath.
 * */
static void Switch_TASK_Inspiration_CYCLE()
{
	vTaskDelete(One_Time_Handler);
	TIME_Base_parameter._INSPIRATION_TIME = 0 ;
	TIME_Base_parameter._EXPIRATION_TIME  = 0 ;
	Next_Half_Breath_Cycle                = Generate_Inspiration_Cycle;
	xTaskCreate(One_Time_Task,"One-Time-Task", 128, NULL,3, &One_Time_Handler);
}

/**
 * @brief VC_SIMV_Inspiration_Time_Variables_Reset.
 * This function resets variables to start an inspiration cycle.
 * */
void VC_SIMV_Assist_ON_Inspiration_Time_Variables_Reset()
{
	Oxygen_Blending_Status=OXYGEN_ACHIEVED;
	Find_Average_Flow();
	Reset_Blower_Status();
	Breath_Count         = Count_The_Breath;

	if(Patient_Trigger  == NO_PATIENT_TRIGGER)
	{
		if(VC_SIMV.PEEP_Val == PEEP_Maintain_Parameter.PEEP_AVG_VAL )
		{
		   CHECK_Flow_Increment();
		}
		else
		{
			Flow_increment       = Flow_fine;
			memset(Volume_Array, 0,(VOL_ARR_SIZE*sizeof(Volume_Array[0])) );
			Volume_Array_count = 0;
		}
	}
	else
	{
		Flow_increment       = Flow_fine;
		memset(Volume_Array, 0,(VOL_ARR_SIZE*sizeof(Volume_Array[0])) );
		Volume_Array_count = 0;
	}

	Volume_Not_Acheieved();
	ExpValve_CLOSE();
	PIP_AVERAGE();
	PEEP_AVERAGE();
	O2_DAC_Control(Common_Mode_Parameter._FIO2_Val);
	Check_Alert();
	Alert_Inspiration_Time_Parameter();
	LED_Alert();
	Alert_Status = NO_ALERT;
	Sampled_Parameter_Inspiration_time();
	GRAPH_VALUES.Maximum_oxygen_flow   = 0;                 //new
	if(VC_SIMV.PEEP_Val != PEEP_Maintain_Parameter.PEEP_AVG_VAL )
	{
		PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time      = PEEP_Control_For_Expiratory_Valve_Open_Time_In_Volume_Mode(PEEP_Maintain_Parameter.PEEP_AVG_VAL,
																												           VC_SIMV.RESPIRATORY_RATE_Val,
													                                                                       VC_SIMV.INSPIRATION_TIME,
													                                                                       VC_SIMV.PEEP_Val,
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
		Volume_Mode_Blower_control.INITIAL_BLOWER_DAC_VAL          = Volume_Mode_Blower_control.PREVIOUS_INITIAL_BLOWER_DAC_VAL;
		Volume_Mode_Blower_control.PREVIOUS_INITIAL_BLOWER_DAC_VAL = Volume_Mode_Blower_control.INITIAL_BLOWER_DAC_VAL;
	}

	if(Patient_Trigger  == PATIENT_TRIGGER_HAPPEN)
	{
		BREATH_STATE                                      = PATIENT_TRIGGER_INSPIRATION;

	}
	else
	{
		BREATH_STATE                                       = INSPIRATION_CYCLE;
	}

	Peep_Status                   				               = PEEP_NOT_ACHEIVED;
	Expiratory_Valve_Lock_Delay   			                   = OPENED;
	PIP_Average_Parameter.Maximum_PIP_Acheived                 = RESET;
	Maximum_Flow_Achieved                                      = FLOW_NOT_ACHIEVED;
	Volume_control_Task.Flow_Max_Achieved                      = RESET;
	Volume_control_Task.LOW_Flow_Max_Achieved                  = RESET;
	Volume_Achieved_Status                                     = VOLUME_NOT_ACHIEVED;
	TIME_Base_parameter._INSPIRATION_TIME                      = VC_SIMV.INSPIRATION_TIME;
	Volume_Achieved_Time                                       = 0;
	Total_Volume_Achieved_Time                                 = 0;

	TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED              = RESET;
	GRAPH_VALUES.volume                                        = RESET;
	Volume_max                                                 = RESET;
	Next_Half_Breath_Cycle                                     = Generate_Expiration_Cycle;
	Run_Current_Breathe_State                                  = Run_Inspiration_Cycle;
	vTaskDelay(VC_SIMV.INSPIRATION_TIME);
}

/**
 * @brief VC_SIMV_Expiration_Time_Variables_Reset.
 * This function resets variables to start an expiration cycle.
 * */
void VC_SIMV_Assist_ON_Expiration_Time_Variables_Reset()
{
	Volume_control_Task.Achieved_Volume                 = GRAPH_VALUES.volume ;
	if(Patient_Trigger  == PATIENT_TRIGGER_HAPPEN)
	{
		BREATH_STATE                                      = PATIENT_TRIGGER_EXPIRATION;
	}
	else
	{
		BREATH_STATE                                       = EXPIRATION_CYCLE;
	}
	PEEP_Maintain_Parameter.PEEP_Cumulative_Val_Trigger = RESET;
	PEEP_Maintain_Parameter.PEEP_Event_Count_Trigger    = RESET;
	PEEP_Maintain_Parameter.PEEP_Cumulative_Val         = RESET;
	PEEP_Maintain_Parameter.PEEP_Event_Count            = RESET;

	Patient_Trigger                                     = NO_PATIENT_TRIGGER;
	Trigger_Time_Monitor();

	Calculate_O2(Common_Mode_Parameter._FIO2_Val);
	Alert_Expiration_Time_Parameter();
	TIME_Base_parameter._EXPIRATION_TIME                = VC_SIMV.EXPIRATION_TIME;

	TIME_Base_parameter.EXPIRATION_TIME_ACHEIVED        = RESET;
	Next_Half_Breath_Cycle                              = Generate_Inspiration_Cycle;
	Run_Current_Breathe_State                           = Run_Expiration_Cycle;
	vTaskDelay(VC_SIMV.EXPIRATION_TIME);
}


/**
 * @brief CHECK_PRESSURE_TRIGGER.
 * This function checks pressure trigger limit.
 * */
static void CHECK_PRESSURE_TRIGGER()
{
	if (( GRAPH_VALUES.pressure < (Pressure_Trigger_Offset - VC_SIMV.TRIG_LMT )))
	{
		Patient_Trigger = PATIENT_TRIGGER_HAPPEN;
		Switch_TASK_Inspiration_CYCLE();
	}
}

/**
 * @brief CHECK_FLOW_TRIGGER.
 * This function checks the flow trigger limit.
 * */
static void CHECK_FLOW_TRIGGER()
{
	if ((FLOW_TRIGGER > (Flow_Trigger_Offset+VC_SIMV.TRIG_LMT )))
	{
		LAST_FLOW_TRIGGER = FLOW_TRIGGER;
		Patient_Trigger   = PATIENT_TRIGGER_HAPPEN;
		Switch_TASK_Inspiration_CYCLE();
	}
}
/**
 * @brief CHECK_FLOW_TRIGGER.
 * This function finds the trigger offset.
 * */
static void CHECK_TRIGGER_OFFSET()
{
	if(GRAPH_VALUES.flow==0 || (GRAPH_VALUES.flow>=(-8) && GRAPH_VALUES.flow<0))
	{
		Pressure_Trigger_Offset  =  GRAPH_VALUES.pressure;
		Flow_Trigger_Offset      =  GRAPH_VALUES.flow;
		FLOW_TRIGGER             =  RESET;
	}
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
		Check_Flow_Graph(VC_SIMV.FLOW_RATE_Val);
	}
	memset(Volume_Array, 0,(VOL_ARR_SIZE*sizeof(Volume_Array[0])) );
	Volume_Array_count = 0;
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
		if(Volume_Array[i] == flow_rate)
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
			if(Patient_Trigger  == NO_PATIENT_TRIGGER)
			{
				Stop_Increment_Count++;
			}
		}
	}
}


/**
 * @brief Trigger_Time_Monitor.
 * This function calculate the trigger time.
 * */
static void Trigger_Time_Monitor()
{
	VC_SIMV.CALC_TRIG_VAL 	                 =  (VC_SIMV.EXPIRATION_TIME   - PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time );
	if(VC_SIMV.CALC_TRIG_VAL <0)
	{
		VC_SIMV.CALC_TRIG_VAL = (VC_SIMV.CALC_TRIG_VAL * (-1));
	}
	VC_SIMV.TOLERANCE_EWAVE =  VC_SIMV.TRIG_TIME;
	if(VC_SIMV.CALC_TRIG_VAL < VC_SIMV.TRIG_TIME )
	{
		VC_SIMV.TOLERANCE_EWAVE =  (VC_SIMV.TRIG_TIME - 200) ;
	}

	VC_SIMV.TRIG_WINDOW		                 = VC_SIMV.TOLERANCE_EWAVE;
}
