/**
 *******************************************************************************
 * \file Pc_Simv_Assist_ON.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief implementation of Pc_Simv_Assist_ON mode.
 *
 * This file contains the implementation Pc_Simv_Assist_ON mode variables
 *******************************************************************************
 **/

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/
#include "Pc_Simv_Assist_ON.h"

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/
static void CHECK_TRIGGER_ASSIST_ON(uint16_t);
static void Switch_TASK_Inspiration_CYCLE();
static void Inspiration_Time_Blower_Control_PC_SIMV_ASSIST_ON(uint16_t);
static void Expiration_Time_Blower_Control_PC_SIMV_ASSIST_ON(uint16_t,float,uint16_t);
static void CHECK_PRESSURE_TRIGGER();
static void CHECK_FLOW_TRIGGER();
static void CHECK_TRIGGER_OFFSET();
static void Set_Blower_Val();
static void Smooth_Ramp_Pressure();
static void Trigger_Time_Monitor();

uint16_t Expiratory_Valve_Open_Time=700;



/**
 *@brief Pc_Simv_Mode_Packet_Data.
 * This task is to extract the Simv mode variable from NRF Bluetooth.
 * @param RECEIVE_GRAPH_PACKET structure pointer.
 */

void Pc_Simv_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet)
{
	uint32_t One_Breathe_time;
	PC_SIMV.PIP_Val                          = Receive_Graph_Packet->PIP_PS_Phigh;
	PC_SIMV.PEEP_Val                         = Receive_Graph_Packet->PEEP_CPAP_Plow;
	PC_SIMV.FIO2_Val                         =  Receive_Graph_Packet->FiO2;


	PC_SIMV.RESPIRATORY_RATE_Val             = Receive_Graph_Packet->RR;
	One_Breathe_time                         = (One_Minite_In_MS / PC_SIMV.RESPIRATORY_RATE_Val);
	PC_SIMV.INSPIRATION_TIME                 = ( ( Receive_Graph_Packet->T_high) * 100 );
	PC_SIMV.EXPIRATION_TIME                  = (One_Breathe_time - PC_SIMV.INSPIRATION_TIME);


	PC_SIMV.Rise_Time                        = Receive_Graph_Packet->Rise_Time;
	PC_SIMV.RISE_TIME_MS_Val                 = (PC_SIMV.Rise_Time *100);



	PC_SIMV.Assist_Control    	             = (0x80 & (Receive_Graph_Packet->Control_Byte))>>7;


	PC_SIMV.TRIG_TYPE     	                 = (0x40 & (Receive_Graph_Packet->Control_Byte))>>6;
	PC_SIMV.TRIG_LMT     	                 = ( (Receive_Graph_Packet->Trigger_Limit) / 10);
	PC_SIMV.TRIG_TIME     	                 = ( (Receive_Graph_Packet->Trigger_Time) * 100);



	PC_SIMV.CALC_TRIG_VAL 	                 =  (PC_SIMV.EXPIRATION_TIME   - Expiratory_Valve_Open_Time );
	if(PC_SIMV.CALC_TRIG_VAL <0)
	{
		PC_SIMV.CALC_TRIG_VAL = (PC_SIMV.CALC_TRIG_VAL * (-1));
	}

	PC_SIMV.TOLERANCE_EWAVE =  PC_SIMV.TRIG_TIME;
	if(PC_SIMV.CALC_TRIG_VAL < PC_SIMV.TRIG_TIME )
	{
		PC_SIMV.TOLERANCE_EWAVE =  (PC_SIMV.TRIG_TIME - 200) ;
	}
	PC_SIMV.TRIG_WINDOW		                 = PC_SIMV.TOLERANCE_EWAVE;

	Alert_Check_Time              = ((float)PC_SIMV.INSPIRATION_TIME/3);

	if(PC_SIMV.Assist_Control == 1)
		Assist_Control=ASSIST_ON;
	else if(PC_SIMV.Assist_Control == 0)
		Assist_Control=ASSIST_OFF;
	if(PC_SIMV.TRIG_TYPE == 0)
		Trigger_Type = Pressure_Trigger;
	else if(PC_SIMV.TRIG_TYPE == 1)
		Trigger_Type = Flow_Trigger;

	Vent_Type                     = ET_TUBE_MODE;
	OOM202.offset_minimum         = ( 0x7F & (Receive_Graph_Packet->Vent_Type_Min_O2));
	OOM202.offset_maximum         = Receive_Graph_Packet->Max_O2;

	Mode_Initial_Flag_Set                        = OPEN;
	vTaskResume(Mode_initial_Flag_Set_Handler);
}


/**
 * @brief Pc_Simv_Task.
 * This Task calls  two function based on inspiration and expiration time.
 * */
void Pc_Simv_Assist_ON_Task(void *argument)
{
	while(1)
	{
		switch (Run_Current_Breathe_State)
		{
			case Run_Inspiration_Cycle:
				Inspiration_Time_Blower_Control_PC_SIMV_ASSIST_ON(Pressure_Mode_blower_control.BLOWER_DAC_VAL);
			break;
			case Run_Expiration_Cycle:
				Pressure_Mode_blower_control.BLOWER_DAC_VAL=0;
				Expiration_Time_Blower_Control_PC_SIMV_ASSIST_ON(Pressure_Mode_blower_control.BLOWER_DAC_VAL,
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
 * @brief PC_SIMV_Expiration_Time_Variables_Reset.
 * This function decides which function needs to be called based on assist control.
 * */
void PC_SIMV_Inspiration_Time_Variables_Reset()
{
	if(Assist_Control == ASSIST_ON)
	{
		PC_SIMV_Assist_ON_Inspiration_Time_Variables_Reset();
	}
	else if(Assist_Control == ASSIST_OFF)
	{
		PC_SIMV_Assist_OFF_Inspiration_Time_Variables_Reset();
	}
}


/**
 * @brief PC_SIMV_Expiration_Time_Variables_Reset.
 * This function decides which function needs to be called based on assist control.
 * */
void PC_SIMV_Expiration_Time_Variables_Reset()
{
	if(Assist_Control == ASSIST_ON)
	{
		PC_SIMV_Assist_ON_Expiration_Time_Variables_Reset();
	}
	else if(Assist_Control == ASSIST_OFF)
	{
		PC_SIMV_Assist_OFF_Expiration_Time_Variables_Reset();
	}
}


/**
 * @brief Inspiration_Time_Blower_Control_PC_SIMV.
 * This function uploads a DAC value to the DAC1 peripheral to control a blower every 2 milliseconds during inspiration time.
 * @param Blower_Signal as Pressure_Mode_blower_control.BLOWER_DAC_VAL.
 * */
static void Inspiration_Time_Blower_Control_PC_SIMV_ASSIST_ON(uint16_t Blower_Signal)
{
	Blower_Signal(Blower_Signal);
}


/**
 * @brief Expiration_Time_Blower_Control_PC_SIMV.
 * This function uploads a DAC value to the DAC1 peripheral to control a blower every 2 milliseconds during inspiration time.
 * @param  Blower_Signal as Pressure_Mode_blower_control.BLOWER_DAC_VAL.
 * @param  PEEP to lock the expiratory valve in peep area.
 * @param  Expiratory_Valve_Open_Time to control the expiratory valve lock delay.
 * */
static void Expiration_Time_Blower_Control_PC_SIMV_ASSIST_ON(uint16_t Blower_Signal,float PEEP,uint16_t Expiratory_Valve_Open_Time)
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

		CHECK_TRIGGER_ASSIST_ON(PC_SIMV.TRIG_WINDOW);
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



static void Switch_TASK_Inspiration_CYCLE()
{
	vTaskDelete(One_Time_Handler);
	TIME_Base_parameter._INSPIRATION_TIME = 0;
	TIME_Base_parameter._EXPIRATION_TIME  = 0;



	Next_Half_Breath_Cycle                = Generate_Inspiration_Cycle;
	xTaskCreate(One_Time_Task,"One-Time-Task", 128, NULL,3, &One_Time_Handler);
}

/**
 * @brief PC_SIMV_Inspiration_Time_Variables_Reset.
 * This function resets variables to start an inspiration cycle.
 * */
void PC_SIMV_Assist_ON_Inspiration_Time_Variables_Reset()
{
	    Oxygen_Blending_Status=OXYGEN_ACHIEVED;
		Blower_Status       = BlOWER_ON;
		Find_Average_Flow();
	    Breath_Count         = Count_The_Breath;
	    Smooth_Ramp_Pressure();
	    PIP_Not_Acheieved();
		ExpValve_CLOSE();
		PIP_AVERAGE();
		PEEP_AVERAGE();
		//Set_Blower_Val();
		O2_DAC_Control(Common_Mode_Parameter._FIO2_Val);
		Check_Alert();
		Alert_Inspiration_Time_Parameter();
		LED_Alert();
		Alert_Status = NO_ALERT;
		GRAPH_VALUES.Maximum_oxygen_flow   = 0;
		Sampled_Parameter_Inspiration_time();
		if( (PC_SIMV.PEEP_Val != PEEP_Maintain_Parameter.PEEP_AVG_VAL ) )
		{
		PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time        = PEEP_Control_For_Expiratory_Valve_Open_Time_In_Pressure_Mode(PEEP_Maintain_Parameter.PEEP_AVG_VAL,
																																 PC_SIMV.RESPIRATORY_RATE_Val,
																																 PC_SIMV.INSPIRATION_TIME,
																																 PC_SIMV.PEEP_Val,
																												                 PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time);

		}
		INCREASE_EACH_DAC_SET_CONST(Common_Mode_Parameter._PIP_Val,PC_SIMV.RISE_TIME_MS_Val);
		BLOWER_ENDING_DAC_SIGNAL_TUNE(PC_SIMV.PIP_Val,PC_SIMV.INSPIRATION_TIME,PC_SIMV.Rise_Time_percentage);

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
		PIP_Status                    				               = PIP_NOT_ACHEIVED;
		Set_PIP_Status                                             = SET_PIP_VALUE_NOT_ACHEIVED;
		Read_Time_vs_Pressure                                      = READ_TIME_OPEN;
		PIP_control_Task.PIP_Control_Event_Occured                 = RESET;
		PIP_Average_Parameter.Maximum_PIP_Acheived                 = RESET;
		Pressure_Mode_blower_control.BLOWER_DAC_VAL                = DAC_VAL(PC_SIMV.PEEP_Val );
		Pressure_Mode_blower_control.LAST_BLOWER_DAC_VAL           = Pressure_Mode_blower_control.BLOWER_DAC_VAL;
		TIME_Base_parameter._INSPIRATION_TIME                      = PC_SIMV.INSPIRATION_TIME;


		TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED              = RESET;
		PIP_control_Task.Last_Early_Acheived_Ramp_Time_Percentage  = PIP_control_Task.Early_Acheived_Ramp_Time_Percentage;
		GRAPH_VALUES.volume                                        = RESET;
		Volume_max                                                 = RESET;
		Next_Half_Breath_Cycle                                     = Generate_Expiration_Cycle;
		Run_Current_Breathe_State                                  = Run_Inspiration_Cycle;
		vTaskDelay(PC_SIMV.INSPIRATION_TIME);
}

/**
 * @brief PC_SIMV_Expiration_Time_Variables_Reset.
 * This function resets variables to start an expiration cycle.
 * */
void PC_SIMV_Assist_ON_Expiration_Time_Variables_Reset()
{
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
		TIME_Base_parameter._EXPIRATION_TIME                = PC_SIMV.EXPIRATION_TIME;


		TIME_Base_parameter.EXPIRATION_TIME_ACHEIVED        = RESET;
		Next_Half_Breath_Cycle                              = Generate_Inspiration_Cycle;
		Run_Current_Breathe_State                           = Run_Expiration_Cycle;
		vTaskDelay(PC_SIMV.EXPIRATION_TIME);
}

/**
 * @brief CHECK_PRESSURE_TRIGGER.
 * This function checks pressure trigger limit.
 * */
static void CHECK_PRESSURE_TRIGGER()
{
	if (( GRAPH_VALUES.pressure < (Pressure_Trigger_Offset - PC_SIMV.TRIG_LMT )))
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
	if ((FLOW_TRIGGER > (Flow_Trigger_Offset+PC_SIMV.TRIG_LMT )))
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
		FLOW_TRIGGER             =  0;
	}
}



static void Set_Blower_Val()
{
	if(PIP_Average_Parameter.PIP_AVG_VAL  == PC_SIMV.PIP_Val  )
	{
		Change_DAC_Values(PC_SIMV.PIP_Val ,Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL);
	}
}

/**
 * @brief Smooth_Ramp_Pressure.
 * This function is used to provide a pressure value smoothly based on lung characteristics
 * */
static void Smooth_Ramp_Pressure()
{
	if(Smooth_Ramp_Time   == Smooth_Ramp_Time_Val_Pending)
	{
		if( (PC_SIMV.PEEP_Val == PEEP_Maintain_Parameter.PEEP_AVG_VAL)  && (PC_SIMV.PEEP_Val >= 9) && (PC_SIMV.PEEP_Val <= 14) && (PC_SIMV.PIP_Val >=30) )
		{
			if(PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time  < 2)
			{
				if(Smooth_Ramp_Time_Val_Count > 2 )
				{
					Smooth_Ramp_Time_Val = PC_SIMV.PEEP_Val;
					Smooth_Ramp_Time     = Smooth_Ramp_Time_Val_Finished;
				}
				else
				{
					Smooth_Ramp_Time_Val_Count++;
				}
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
	PC_SIMV.CALC_TRIG_VAL 	                 =  (PC_SIMV.EXPIRATION_TIME   - PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time );
	if(PC_SIMV.CALC_TRIG_VAL <0)
	{
		PC_SIMV.CALC_TRIG_VAL = (PC_SIMV.CALC_TRIG_VAL * (-1));
	}
	PC_SIMV.TOLERANCE_EWAVE =  PC_SIMV.TRIG_TIME;
	if(PC_SIMV.CALC_TRIG_VAL < PC_SIMV.TRIG_TIME )
	{
		PC_SIMV.TOLERANCE_EWAVE =  (PC_SIMV.TRIG_TIME - 200) ;
	}

	PC_SIMV.TRIG_WINDOW		                 = PC_SIMV.TOLERANCE_EWAVE;
}
