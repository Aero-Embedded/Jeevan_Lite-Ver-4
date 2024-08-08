/**
 *******************************************************************************
 * \file Pc_cmv.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief implementation of PCCMV mode.
 *
 * This file contains the implementation PCCMV mode variables
 *******************************************************************************
 **/


/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include "Pc_cmv.h"



/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/


/**
 * @brief Inspiration_Time_Blower_Control_PC_CMV.
 * */
static void Inspiration_Time_Blower_Control_PC_CMV(uint16_t);

/**
 * @brief Expiration_Time_Blower_Control_PC_CMV.
 * */
static void Expiration_Time_Blower_Control_PC_CMV(uint16_t,float,uint16_t);

/**
 * @brief Smooth_Ramp_Pressure.
 * */
static void Smooth_Ramp_Pressure();



/**
 *@brief Pc_Cmv_Mode_Packet_Data.
 * This task is to extract the PCCMV mode variable from NRF Bluetooth.
 * @param RECEIVE_GRAPH_PACKET structure pointer.
 */

void Pc_Cmv_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet)
{
	uint32_t One_Breathe_time;
	PC_CMV.PIP_Val                = Receive_Graph_Packet->PIP_PS_Phigh;
	PC_CMV.PEEP_Val               = Receive_Graph_Packet->PEEP_CPAP_Plow;
	PC_CMV.FIO2_Val               = Receive_Graph_Packet->FiO2;
	PC_CMV.RESPIRATORY_RATE_Val   = Receive_Graph_Packet->RR;
	One_Breathe_time              = (One_Minite_In_MS / PC_CMV.RESPIRATORY_RATE_Val);
	PC_CMV.INSPIRATION_TIME       = ( ( Receive_Graph_Packet->T_high) * 100 );
	PC_CMV.EXPIRATION_TIME        = (One_Breathe_time - PC_CMV.INSPIRATION_TIME);
	PC_CMV.Rise_Time              = Receive_Graph_Packet->Rise_Time;

	Alert_Check_Time              = ((float)PC_CMV.INSPIRATION_TIME/3);
	Vent_Type                     = ET_TUBE_MODE;

	OOM202.offset_minimum         = ( 0x7F & (Receive_Graph_Packet->Vent_Type_Min_O2));
	OOM202.offset_maximum         = Receive_Graph_Packet->Max_O2;

    Mode_Initial_Flag_Set         = OPEN;
	vTaskResume(Mode_initial_Flag_Set_Handler);
}


/**
 * @brief Pc_Cmv_Task.
 * This Task calls  two function based on inspiration and expiration time.
 * */

void Pc_Cmv_Task (void *argument)
{
	while(1)
	{
		switch (Run_Current_Breathe_State)
		{
			case Run_Inspiration_Cycle:
				Inspiration_Time_Blower_Control_PC_CMV(Pressure_Mode_blower_control.BLOWER_DAC_VAL);
			break;
			case Run_Expiration_Cycle:
				Pressure_Mode_blower_control.BLOWER_DAC_VAL=0;
				Expiration_Time_Blower_Control_PC_CMV(Pressure_Mode_blower_control.BLOWER_DAC_VAL,
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
 * @brief Inspiration_Time_Blower_Control_PC_CMV.
 * This function uploads a DAC value to the DAC1 peripheral to control a blower every 2 milliseconds during inspiration time.
 * @param Blower_Signal as Pressure_Mode_blower_control.BLOWER_DAC_VAL.
 * */

static void Inspiration_Time_Blower_Control_PC_CMV(uint16_t Blower_Signal)
{
	Blower_Signal(Blower_Signal);
}


/**
 * @brief Expiration_Time_Blower_Control_PC_CMV.
 * This function uploads a DAC value to the DAC1 peripheral to control a blower every 2 milliseconds during inspiration time.
 * @param  Blower_Signal as Pressure_Mode_blower_control.BLOWER_DAC_VAL.
 * @param  PEEP to lock the expiratory valve in peep area.
 * @param  Expiratory_Valve_Open_Time to control the expiratory valve lock delay.
 * */

static void Expiration_Time_Blower_Control_PC_CMV(uint16_t Blower_Signal,float PEEP,uint16_t Expiratory_Valve_Open_Time)
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
}

/**
 * @brief PC_CMV_Inspiration_Time_Variables_Reset.
 * This function resets variables to start an inspiration cycle.
 * */
void PC_CMV_Inspiration_Time_Variables_Reset()
{
	Oxygen_Blending_Status=OXYGEN_ACHIEVED;
	Blower_Status       = BlOWER_ON;
	Find_Average_Flow();
	Breath_Count         = Count_The_Breath;
	Smooth_Ramp_Pressure();
	PIP_Not_Acheieved();
	ExpValve_CLOSE();
	PIP_AVERAGE();
	PEEP_AVERAGE();                                             //new
	O2_DAC_Control(Common_Mode_Parameter._FIO2_Val);
	Check_Alert();
	Alert_Inspiration_Time_Parameter();
	LED_Alert();
	Alert_Status                                              = NO_ALERT;
	GRAPH_VALUES.Maximum_oxygen_flow                          = 0;                        //new
	Sampled_Parameter_Inspiration_time();
	if( (PC_CMV.PEEP_Val != PEEP_Maintain_Parameter.PEEP_AVG_VAL ) )
	{
		PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time     = PEEP_Control_For_Expiratory_Valve_Open_Time_In_Pressure_Mode(PEEP_Maintain_Parameter.PEEP_AVG_VAL,PC_CMV.RESPIRATORY_RATE_Val,PC_CMV.INSPIRATION_TIME,PC_CMV.PEEP_Val,PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time);
	}
	INCREASE_EACH_DAC_SET_CONST(Common_Mode_Parameter._PIP_Val,PC_CMV.RISE_TIME_MS_Val);
	BLOWER_ENDING_DAC_SIGNAL_TUNE(PC_CMV.PIP_Val,PC_CMV.INSPIRATION_TIME,PC_CMV.Rise_Time_percentage);
	BREATH_STATE                                               = INSPIRATION_CYCLE;
	Peep_Status                   				               = PEEP_NOT_ACHEIVED;
	Expiratory_Valve_Lock_Delay   			                   = OPENED;
	PIP_Status                    				               = PIP_NOT_ACHEIVED;
	Set_PIP_Status                                             = SET_PIP_VALUE_NOT_ACHEIVED;
	Read_Time_vs_Pressure                                      = READ_TIME_OPEN;
	PIP_control_Task.PIP_Control_Event_Occured                 = RESET;
	PIP_Average_Parameter.Maximum_PIP_Acheived                 = RESET;
	Pressure_Mode_blower_control.BLOWER_DAC_VAL                = DAC_VAL(PC_CMV.PEEP_Val );
	Pressure_Mode_blower_control.LAST_BLOWER_DAC_VAL           = Pressure_Mode_blower_control.BLOWER_DAC_VAL;
	TIME_Base_parameter._INSPIRATION_TIME                      = PC_CMV.INSPIRATION_TIME;
	TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED              = RESET;
	PIP_control_Task.Last_Early_Acheived_Ramp_Time_Percentage  = PIP_control_Task.Early_Acheived_Ramp_Time_Percentage;
	GRAPH_VALUES.volume                                        = RESET;
	Volume_max                                                 = RESET;
	Next_Half_Breath_Cycle                                     = Generate_Expiration_Cycle;
	Run_Current_Breathe_State                                  = Run_Inspiration_Cycle;
	vTaskDelay(PC_CMV.INSPIRATION_TIME);
}

/**
 * @brief PC_CMV_Expiration_Time_Variables_Reset.
 * This function resets variables to start an expiration cycle.
 * */

void PC_CMV_Expiration_Time_Variables_Reset()
{
	BREATH_STATE                                        = EXPIRATION_CYCLE;
	PEEP_Maintain_Parameter.PEEP_Cumulative_Val_Trigger = RESET;
	PEEP_Maintain_Parameter.PEEP_Event_Count_Trigger    = RESET;
	PEEP_Maintain_Parameter.PEEP_Cumulative_Val         = RESET;
	PEEP_Maintain_Parameter.PEEP_Event_Count            = RESET;
	Calculate_O2(Common_Mode_Parameter._FIO2_Val);
	Alert_Expiration_Time_Parameter();
	TIME_Base_parameter._EXPIRATION_TIME                = PC_CMV.EXPIRATION_TIME;
	TIME_Base_parameter.EXPIRATION_TIME_ACHEIVED        = RESET;
	Next_Half_Breath_Cycle                              = Generate_Inspiration_Cycle;
	Run_Current_Breathe_State                           = Run_Expiration_Cycle;
	vTaskDelay(PC_CMV.EXPIRATION_TIME);
}



/**
 * @brief Smooth_Ramp_Pressure.
 * This function is used to provide a pressure value smoothly based on lung characteristics
 * */


static void Smooth_Ramp_Pressure()
{
	if(Smooth_Ramp_Time   == Smooth_Ramp_Time_Val_Pending)
	{
		if( (PC_CMV.PEEP_Val == PEEP_Maintain_Parameter.PEEP_AVG_VAL)  && (PC_CMV.PEEP_Val >= 9) && (PC_CMV.PEEP_Val <= 14) && (PC_CMV.PIP_Val >=30) )
		{
			if(PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time  < 2)
			{
				if(Smooth_Ramp_Time_Val_Count > 2 )
				{
					Smooth_Ramp_Time_Val = PC_CMV.PEEP_Val;
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
 * @brief Find_Average_Flow.
 * This function is used to find the maximum flow delivered to the lungs in the oxygen blending concept.
 * */
void Find_Average_Flow()           //new
{
	Avg_Flow   = Flow_Cummulative_Val/Flow_Count;

	if(Avg_Flow <=1)
	{
		Avg_Flow = 1;
	}
	Flow_Count =0;
	Flow_Cummulative_Val =0;
}


/**
 * @brief PIP_Not_Acheieved.
 * This Function used for debugging purpose.
 * */

void PIP_Not_Acheieved()
{
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
}

/**
 * @brief PIP_Acheieved.
 * This Function used for debugging purpose.
 * */
void PIP_Acheieved()
{
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
}
