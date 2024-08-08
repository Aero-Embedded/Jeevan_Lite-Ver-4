/*
 * APRV.c
 *
 *  Created on: Feb 27, 2023
 *      Author: asus
 */

#include "APRV.h"


static void APRV_INSPIRATION_CYCLE_SINGLE();
static void APRV_EXPIRATION_CYCLE_SINGLE();

static void APRV_INSPIRATION_CYCLE_CONTINUOUS();
static void APRV_EXPIRATION_CYCLE_CONTINUOUS(float ,uint16_t );
static void APRV_PIP_AVERAGE();
static uint16_t APRV_TUNE_PIP_BLOWER(uint16_t *,uint8_t);
static void APRV_PEEP_AVERAGE();
static uint16_t PEEP_Control(uint8_t,uint8_t,uint16_t);
static void Check_Trigger();

static void STOP_BLOWER_IF_FLOW_SENSE();
static void CHECK_TRIGGER_OFFSET();
static void CHECK_PRESSURE_TRIGGER();
static void CHECK_FLOW_TRIGGER();






void APRV_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet)
{
	 APRV.P_HIGH		     =  (Receive_Graph_Packet->PIP_PS_Phigh);
	 APRV.P_LOW 		     =  (Receive_Graph_Packet->PEEP_CPAP_Plow);
	 APRV.T_HIGH             =  (Receive_Graph_Packet->T_high);
	 APRV.T_LOW              =  (Receive_Graph_Packet->T_low);
	 APRV.T_HIGH_VAL         =  ( APRV.T_HIGH   * 100);
	 APRV.T_LOW_VAL          =  ( APRV.T_LOW    * 100);
	 APRV.FIO2_VAL           =  (Receive_Graph_Packet->FiO2);

	 APRV.TRIG_TYPE          = (0x40 & (Receive_Graph_Packet->Control_Byte))>>6;
	 APRV.TRIG_LMT           = ( (Receive_Graph_Packet->Trigger_Limit) / 10);
	 APRV.TRIG_TIME          = ( (Receive_Graph_Packet->Trigger_Time) * 100);


	 if(APRV.TRIG_TYPE == 0)
		Trigger_Type = Pressure_Trigger;
	else if(APRV.TRIG_TYPE == 1)
		Trigger_Type = Flow_Trigger;


	 Vent_Type               = ET_TUBE_MODE;
	 OOM202.offset_minimum   = ( 0x7F & (Receive_Graph_Packet->Vent_Type_Min_O2));
	 OOM202.offset_maximum   = Receive_Graph_Packet->Max_O2;


	 Alert_Check_Time        = ((float)APRV.T_HIGH_VAL/3);
	 Mode_Initial_Flag_Set   = OPEN;
	 vTaskResume(Mode_initial_Flag_Set_Handler);
}





void APRV_Task(void *argument)
{
	while(1)
	{
		switch (APRV_current_breathe)
		{
			  case APRV_INS:
				  APRV_INSPIRATION_CYCLE_CONTINUOUS();
			  break;
			  case APRV_EXP:
				  APRV_EXPIRATION_CYCLE_CONTINUOUS(PEEP_Maintain_Parameter.PEEP_Temp,
	                                                PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time);
			  break;
			  default:
			  break;
		}
		vTaskDelay(Two_Millisecond_Delay);
	}

}



void APRV_ONE_TIME_Task(void *argument)
{
	while(1)
	{
		switch (APRV_Next_Breathe)
		{
		      case APRV_INSPIRATION:
		    	  APRV_INSPIRATION_CYCLE_SINGLE();
			  break;
		      case APRV_EXPIRATION:
		    	  APRV_EXPIRATION_CYCLE_SINGLE();
		      break;
		      default:
		      break;
		}
	}

}



static void APRV_INSPIRATION_CYCLE_SINGLE()
{
	//Breath_Count                       = Count_The_Breath;
	Oxygen_Blending_Status=OXYGEN_ACHIEVED;
	Blower_Status       = BlOWER_ON;
	Find_Average_Flow();
	BREATH_STATE                       = INSPIRATION_CYCLE;
	ExpValve_CLOSE();
	APRV_PIP_AVERAGE();
	APRV_PEEP_AVERAGE();

	O2_DAC_Control(Common_Mode_Parameter._FIO2_Val);
	Check_Alert();
	Alert_Inspiration_Time_Parameter();
	LED_Alert();
	Alert_Status                      = NO_ALERT;
	Sampled_Parameter_Inspiration_time();
	GRAPH_VALUES.Maximum_oxygen_flow                          = 0;
	PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time    =   PEEP_Control(APRV_PEEP_average.PEEP_AVG_VAL,
													                       APRV.P_LOW,
												                           PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time);

	PIP_Average_Parameter.Maximum_PIP_Acheived       =  RESET;
	Peep_Status                                      =  PEEP_NOT_ACHEIVED;
	Expiratory_Valve_Lock_Delay                      =  OPENED;
	GRAPH_VALUES.volume                              =  RESET;
	Volume_max                                       =  RESET;
	APRV_Next_Breathe                                =  APRV_EXPIRATION;
	APRV_current_breathe                             =  APRV_INS;
	APRV.T_HIGH_VAL_COUNTER                          =  APRV.T_HIGH_VAL;
	TIME_Base_parameter._INSPIRATION_TIME            =  APRV.T_HIGH_VAL;
	TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED    =  RESET;
	Patient_Trigger_Count                            = 0;
	APRV_Trigger_Check_Time                          = APRV.TRIG_TIME;
	Run_Current_Breathe_State                                  = Run_Inspiration_Cycle;  //new //new //new
	vTaskDelay(APRV.T_HIGH_VAL);
}


static void APRV_EXPIRATION_CYCLE_SINGLE()
{
	BREATH_STATE                                        = EXPIRATION_CYCLE;
	ExpValve_OPEN();
	APRV_mode_blower_control.BLOWER_DAC_VAL             = APRV_TUNE_PIP_BLOWER(&APRV_mode_blower_control.BLOWER_DAC_VAL,
			                                                       APRV.P_HIGH );

	Calculate_O2(Common_Mode_Parameter._FIO2_Val);
	Alert_Expiration_Time_Parameter();

	APRV_Next_Breathe                                   = APRV_INSPIRATION;
	APRV_current_breathe                                = APRV_EXP;
	APRV.T_LOW_VAL_COUNTER                              = APRV.T_LOW_VAL;

	TIME_Base_parameter._EXPIRATION_TIME                = APRV.T_LOW_VAL;
	TIME_Base_parameter.EXPIRATION_TIME_ACHEIVED        = RESET;
	Parkar_valve_Signal(0);
	Run_Current_Breathe_State                           = Run_Expiration_Cycle;
	vTaskDelay(APRV.T_LOW_VAL);
}



static void APRV_INSPIRATION_CYCLE_CONTINUOUS()
{
	if(Blower_Wait_Time == 0)
	{
		Blower_Signal(APRV_mode_blower_control.BLOWER_DAC_VAL);
	}
	Patient_Circuit_Disconnected_Alert();
	Find_Max_PIP();
	Check_Trigger();
	if(GRAPH_VALUES.pressure >= APRV.P_HIGH)
	{
		Parkar_valve_Signal(0);
	}



}



static void APRV_EXPIRATION_CYCLE_CONTINUOUS(float PEEP,uint16_t Expiratory_Valve_Open_Time)
{
	Blower_Signal(0);
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





static void APRV_PIP_AVERAGE()
{
	if((APRV_PIP_average.PIP_Cumulative_Val == 0) || (APRV_PIP_average.PIP_Event_Count == 0))
	{
		APRV_PIP_average.PIP_Cumulative_Val = 1;
		APRV_PIP_average.PIP_Event_Count    = 1;
	}
	APRV_PIP_average.PIP_AVG_VAL        = (uint32_t)(APRV_PIP_average.PIP_Cumulative_Val/ APRV_PIP_average.PIP_Event_Count);
	APRV_PIP_average.PIP_Cumulative_Val = RESET;
	APRV_PIP_average.PIP_Event_Count    = RESET;
	PIP_Average_Parameter.PIP_AVG_VAL   = APRV_PIP_average.PIP_AVG_VAL;
}




static void APRV_PEEP_AVERAGE()
{
	if((APRV_PEEP_average.PEEP_Cumulative_Val == 0) || (APRV_PEEP_average.PEEP_Event_Count == 0))
	{
		APRV_PEEP_average.PEEP_Cumulative_Val = 1;
		APRV_PEEP_average.PEEP_Event_Count    = 1;
	}
	APRV_PEEP_average.PEEP_AVG_VAL        = (uint32_t)(APRV_PEEP_average.PEEP_Cumulative_Val/ APRV_PEEP_average.PEEP_Event_Count);
	APRV_PEEP_average.PEEP_Cumulative_Val = RESET;
	APRV_PEEP_average.PEEP_Event_Count    = RESET;

	PEEP_Maintain_Parameter.PEEP_AVG_VAL  = APRV_PEEP_average.PEEP_AVG_VAL;
}

static uint16_t APRV_TUNE_PIP_BLOWER(uint16_t *Last_Blower_DAC_Val,uint8_t Set_PIP)
{
	if(  ( APRV_PIP_average.PIP_AVG_VAL)  > ( Set_PIP ) && (APRV_PIP_average.PIP_AVG_VAL  < (Set_PIP + 2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-10);
	}
	else if(  ( APRV_PIP_average.PIP_AVG_VAL)  >= ( Set_PIP + 2 ) && (APRV_PIP_average.PIP_AVG_VAL  < (Set_PIP + 4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-20);
	}
	else if(  ( APRV_PIP_average.PIP_AVG_VAL)  >= ( Set_PIP + 4 )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-40);
	}
	else if( (APRV_PIP_average.PIP_AVG_VAL  < Set_PIP) && (APRV_PIP_average.PIP_AVG_VAL  > (Set_PIP -2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+10);
	}
	else if(  (APRV_PIP_average.PIP_AVG_VAL  <= (Set_PIP-2) ) && (APRV_PIP_average.PIP_AVG_VAL  >  (Set_PIP-4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+20);
	}
	else if(  (APRV_PIP_average.PIP_AVG_VAL  <= (Set_PIP-4) )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+40);
	}
	else
	{
		*Last_Blower_DAC_Val = *Last_Blower_DAC_Val;
	}
	return *Last_Blower_DAC_Val;
}








static uint16_t PEEP_Control(uint8_t Achieved_PEEP,uint8_t Set_PEEP,uint16_t Valve_Open_Time)
{
	static uint8_t Wait_Count_1,Wait_Count_2;
	if (Achieved_PEEP >  (Set_PEEP+8) )
	{
		Wait_Count_1++;
		if (Wait_Count_1 > 0)
		{
			Valve_Open_Time = (Valve_Open_Time + 100);
			Wait_Count_1    = RESET;
				if (Valve_Open_Time >= 500)
				{
					Valve_Open_Time                   = 500;
					PEEP_Maintain_Parameter.PEEP_Temp = (PEEP_Maintain_Parameter.PEEP_Temp - 0.5);
				}
		}
		if (PEEP_Maintain_Parameter.PEEP_Temp <= 3)
			PEEP_Maintain_Parameter.PEEP_Temp = 3;

	}
	else if( (Achieved_PEEP> (Set_PEEP+2))  && (Achieved_PEEP<= (Set_PEEP+8)) )
	{

		Wait_Count_1++;
		if (Wait_Count_1 > 0)
		{
			Valve_Open_Time = Valve_Open_Time + 40;
			Wait_Count_1    = RESET;
				if (Valve_Open_Time >= 500)
				{
					Valve_Open_Time                   = 500;
					PEEP_Maintain_Parameter.PEEP_Temp = (PEEP_Maintain_Parameter.PEEP_Temp - 0.5);
				}
		}
		if (PEEP_Maintain_Parameter.PEEP_Temp <= 3)
			PEEP_Maintain_Parameter.PEEP_Temp = 3;

	}


	else if( (Achieved_PEEP> Set_PEEP)  && (Achieved_PEEP <= (Set_PEEP+2)) )
	{
		Wait_Count_1++;
		if (Wait_Count_1 > 0)
		{

			Valve_Open_Time = Valve_Open_Time + 10;
			Wait_Count_1    = RESET;
				if (Valve_Open_Time >= 500)
				{
					Valve_Open_Time                   = 500;
					PEEP_Maintain_Parameter.PEEP_Temp = (PEEP_Maintain_Parameter.PEEP_Temp - 0.5);
				}
		}
		if (PEEP_Maintain_Parameter.PEEP_Temp <= 3)
			PEEP_Maintain_Parameter.PEEP_Temp = 3;

	}
	else if ((Achieved_PEEP == 0 ) )
	{
		Wait_Count_2++;
		if (Wait_Count_2 > 0)
		{
			PEEP_Maintain_Parameter.PEEP_Temp = (PEEP_Maintain_Parameter.PEEP_Temp + 1);
			Valve_Open_Time                   = (Valve_Open_Time - 10);
			Wait_Count_2                      = RESET;
				if ( (Valve_Open_Time < 1) || (Valve_Open_Time > 500) )
				{
					Valve_Open_Time = 1;
				}
		}
		if (PEEP_Maintain_Parameter.PEEP_Temp  > (Set_PEEP + 10) )
			PEEP_Maintain_Parameter.PEEP_Temp  = PEEP_Maintain_Parameter.PEEP_Temp ;

	}
		else if (Achieved_PEEP< (Set_PEEP -8))
		{
			Wait_Count_2++;
			if (Wait_Count_2 > 0)
			{
				Valve_Open_Time = ( Valve_Open_Time - 50);
				Wait_Count_2    = RESET;
					if ((Valve_Open_Time < 1) || (Valve_Open_Time > 500) )
					{
						Valve_Open_Time                   = 1;
						PEEP_Maintain_Parameter.PEEP_Temp = (PEEP_Maintain_Parameter.PEEP_Temp + 0.5);
					}
			}
			if (PEEP_Maintain_Parameter.PEEP_Temp> (Set_PEEP + 10) )
				PEEP_Maintain_Parameter.PEEP_Temp =PEEP_Maintain_Parameter.PEEP_Temp;

		}
		else if (( Achieved_PEEP >= (Set_PEEP -8)) && (Achieved_PEEP< (Set_PEEP-2)))
		{
			Wait_Count_2++;
			if (Wait_Count_2 > 0)
			{
				Valve_Open_Time = (Valve_Open_Time - 30);
				Wait_Count_2    = RESET;
					if (Valve_Open_Time< 1 || Valve_Open_Time > 500)
					{
						Valve_Open_Time                   = 1;
						PEEP_Maintain_Parameter.PEEP_Temp = (PEEP_Maintain_Parameter.PEEP_Temp + 0.5);
					}
			}
			if (PEEP_Maintain_Parameter.PEEP_Temp > (Set_PEEP + 10) )
				PEEP_Maintain_Parameter.PEEP_Temp = PEEP_Maintain_Parameter.PEEP_Temp;

		}
		else if ((Achieved_PEEP>= (Set_PEEP -2)) && (Achieved_PEEP< (Set_PEEP)))
		{
			Wait_Count_2++;
			if (Wait_Count_2 > 0)
			{

				Valve_Open_Time = (Valve_Open_Time - 10);
				Wait_Count_2    = RESET;
					if ( (Valve_Open_Time < 1) || (Valve_Open_Time> 500) )
					{
						PEEP_Maintain_Parameter.PEEP_Temp = (PEEP_Maintain_Parameter.PEEP_Temp + 0.5);
						Valve_Open_Time                   = 1;
					}
			}
			if (PEEP_Maintain_Parameter.PEEP_Temp > (Set_PEEP + 10) )
				PEEP_Maintain_Parameter.PEEP_Temp = PEEP_Maintain_Parameter.PEEP_Temp;
		}
		else
			PEEP_Maintain_Parameter.PEEP_Temp = PEEP_Maintain_Parameter.PEEP_Temp;


	return Valve_Open_Time;
}




static void Check_Trigger()
{
	if(APRV_Trigger_Check_Time == 0 )
	{
		if( (GRAPH_VALUES.flow > 0) )
		{
			STOP_BLOWER_IF_FLOW_SENSE();

				if (Trigger_Type == Pressure_Trigger)
				{
					CHECK_PRESSURE_TRIGGER();
				}
				else if (Trigger_Type == Flow_Trigger)
				{
					CHECK_FLOW_TRIGGER();
				}
		}
	}
	else
	{
		CHECK_TRIGGER_OFFSET();
	}
}



static void STOP_BLOWER_IF_FLOW_SENSE()
{
	Blower_Signal(0);
	Blower_Wait_Time = 500;
}

static void CHECK_TRIGGER_OFFSET()
{
	if(GRAPH_VALUES.flow==0 || (GRAPH_VALUES.flow>=(-8) && GRAPH_VALUES.flow<0))
	{
		FLOW_TRIGGER             = 0;
		Pressure_Trigger_Offset  =  GRAPH_VALUES.pressure;
		Flow_Trigger_Offset      =  GRAPH_VALUES.flow;
	}
}


static void CHECK_PRESSURE_TRIGGER()
{
	if (( GRAPH_VALUES.pressure < (Pressure_Trigger_Offset - APRV.TRIG_LMT )))
	{
	    Breath_Count                                 = Count_The_Breath;
		Patient_Trigger                              = PATIENT_TRIGGER_HAPPEN;
		Patient_Trigger_Count++;
		APRV_Trigger_Check_Time                      = APRV.TRIG_TIME;
		Patient_Trigger  							 = NO_PATIENT_TRIGGER;
	}
}

static void CHECK_FLOW_TRIGGER()
{
	if ((GRAPH_VALUES.flow > (Flow_Trigger_Offset+APRV.TRIG_LMT )))
	{
		Breath_Count                                = Count_The_Breath;
		LAST_FLOW_TRIGGER 							= FLOW_TRIGGER;
		Patient_Trigger  							= PATIENT_TRIGGER_HAPPEN;
		Patient_Trigger_Count++;
		APRV_Trigger_Check_Time                     = APRV.TRIG_TIME;
		Patient_Trigger  							= NO_PATIENT_TRIGGER;
	}
}


