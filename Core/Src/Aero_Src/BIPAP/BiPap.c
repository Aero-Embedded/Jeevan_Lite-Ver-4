/*
 * BiPap.c
 *
 *  Created on: Feb 24, 2023
 *      Author: asus
 */


#include "BiPap.h"


static void BIBAP_CONTROL();
static void RUN_BIPAP_EPAP_MAINTAIN_BLOWER();
static void STOP_BLOWER_IF_FLOW_SENSE();
static void CHECK_PRESSURE_TRIGGER();
static void CHECK_FLOW_TRIGGER();
static void CHECK_TRIGGER_OFFSET();
static void BIPAP_EPAP_AVERAGE();
static uint16_t BIPAP_TUNE_EPAP_BLOWER(uint16_t *,uint8_t);
static uint16_t BIBAP_TUNE_IPAP_BLOWER(uint16_t *,uint8_t );
static void RESET_FLAGS();

static void BACKUP_PC_CMV_START();
static void BIBAP_IPAP_AVERAGE();
static void Patient_Circuit_Disconnected_Alert_BIBAP();
void EPAP_TIME_CONTROL();
void Cummulate_PEEP();
void Inspiration_Trigger_Check();
static void Apnea_O2_control();
void ADJUST_PEEP();

extern uint16_t O2_max_Control,O2_max_Control_Low;
void Tune_O2_BIPAP(uint8_t,uint8_t);
void Low_Oxygen_Achieved_BIPAP(uint8_t ,uint8_t );
void High_Oxygen_Achieved_BIPAP(uint8_t ,uint8_t);

void Oxygen_epap();


void BIPAP_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet)
{
	 uint32_t One_Breathe_time;
	 BIPAP.IPAP_Val                         = Receive_Graph_Packet->PIP_PS_Phigh;
	 BIPAP.EPAP_Val                         = Receive_Graph_Packet->PEEP_CPAP_Plow;

	 BIPAP.TRIG_TYPE                        = (0x40 & (Receive_Graph_Packet->Control_Byte))>>6;
	 BIPAP.TRIG_LMT                         = ( (Receive_Graph_Packet->Trigger_Limit) / 10);
	 BIPAP.TRIG_TIME                        = ( (Receive_Graph_Packet->Trigger_Time) * 100);


	 BIPAP.APNEA_TIME                       = Receive_Graph_Packet->Apnea_Time;
	 BIPAP.APNEA_COUNTER                    = (1000*BIPAP.APNEA_TIME) ;

	 BIPAP._IPAP_TIME                       = ((Receive_Graph_Packet->T_high));
	 BIPAP.IPAP_TIME                        = (BIPAP._IPAP_TIME*100);

	 BIPAP.RESPIRATORY_RATE_Val             = Receive_Graph_Packet->RR;
	 One_Breathe_time                       = (One_Minite_In_MS / BIPAP.RESPIRATORY_RATE_Val);

	 BIPAP.EPAP_TIME                        = (One_Breathe_time - BIPAP.IPAP_TIME );

	 BIPAP.BIPAP_O2_Val                     = Receive_Graph_Packet->FiO2;

	 BIPAP.VENT_TYPE                        = ( 0x80 & (Receive_Graph_Packet->Vent_Type_Min_O2)) >> 7;

	 OOM202.offset_minimum                 =  ( 0x7F & (Receive_Graph_Packet->Vent_Type_Min_O2));
	 OOM202.offset_maximum                 =  Receive_Graph_Packet->Max_O2;


	 if(BIPAP.VENT_TYPE == 0)
		 Vent_Type = ET_TUBE_MODE;
	else if(BIPAP.VENT_TYPE == 1)
		 Vent_Type = MASK_MODE;

	 if(BIPAP.TRIG_TYPE == 0)
		Trigger_Type = Pressure_Trigger;
	else if(BIPAP.TRIG_TYPE == 1)
		Trigger_Type = Flow_Trigger;
	 Mode_Initial_Flag_Set                  = OPEN;
	 vTaskResume(Mode_initial_Flag_Set_Handler);

}




void BIPAP_Task(void *argument)
{
	while(1)
	{
		if(BIPAP.APNEA_COUNTER > 0)
		{
			BIBAP_CONTROL();
		}
		else
		{

			BREATH_STATE                    = EXPIRATION_CYCLE;
			vTaskSuspend(BIPAP_PID_Handler);
			BIPAP.EPAP_COUNTER              = 0;
			BIPAP_Control._INSPIRATION_TIME = 0;
			RESET_FLAGS();
			Blower_Signal(0);
			 if(Current_Backup_Mode == Backup_PCCMV_Mode)
			 {
				 BACKUP_PC_CMV_START();
			 }
		}
		vTaskDelay(Two_Millisecond_Delay);
	}
}




void BIPAP_PID_Task(void *argument)
{
	while(1)
	{

	if(BIPAP.APNEA_COUNTER > 0)
	{
		if(BIPAP_Control._INSPIRATION_TIME > 0)
		{
			BREATH_STATE                                          = PATIENT_TRIGGER_INSPIRATION;
			PIP_Control(BIPAP.IPAP_Val );
			Blower_Signal(Pressure_Mode_blower_control.BLOWER_DAC_VAL);
			BIPAP_Flow_Cummulative_val = BIPAP_Flow_Cummulative_val+GRAPH_VALUES.flow;
			BIPAP_Flow_Sensor_Count++;

			if(BIPAP_Control._INSPIRATION_TIME < 100)
			   {
				   BIPAP_Pip_average.IPAP_Cumulative_Val += GRAPH_VALUES.pressure;
				   BIPAP_Pip_average.IPAP_Event_Count++;
			   }

			   Inspiration_Trigger_Check();

				if(BIPAP.BIPAP_O2_Val > 21)
				{
					if(GRAPH_VALUES.pressure >= BIPAP.IPAP_Val )
					{
						if(Vent_Type == MASK_MODE)
						{

						}
						else
						{
							Parkar_valve_Signal(0);
						}

					}

				}
				if(GRAPH_VALUES.flow > BIPAP_Max_Flow)
				{
					BIPAP_Max_Flow = GRAPH_VALUES.flow;
				}
		}
		else if(BIPAP_Control._INSPIRATION_TIME == 0)
		{
				if(BIPAP.BIPAP_O2_Val > 21)
				{
					//Apnea_O2_control();
					Check_O2_Supply_Alert_in_Apnea_Once(BIPAP.BIPAP_O2_Val);
					GRAPH_VALUES.Maximum_oxygen_flow = 0;
					if(O2_Control_Bipap >= 6)
					{
						Tune_O2_BIPAP(BIPAP.BIPAP_O2_Val,Oxygen_Parameter.Achieved_Oxygen);
						Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;
					}

					BIPAP_Max_Flow = 0;
				}

				ExpValve_OPEN();
				BIPAP.EPAP_COUNTER                                   = BIPAP.EPAP_TIME;
				BIPAP_Control.Trigger_Check_Wait_Time                = 500+ BIPAP.TRIG_TIME ;

					Wait_Time                                       = 10;
					if(BIPAP.BIPAP_O2_Val == 21)
					{
						Parkar_valve_Signal(0);
						Blower_Signal(0);
					}

				Send_Frame                                         = First_Frame;
				Send_Bluetooth_Data_Farme                          = SEND_SAMPLED_PARAMETER_FRAME;
				ADJUST_PEEP();
				BREATH_STATE                                       = EXPIRATION_CYCLE;
				Patient_Trigger                                    = NO_PATIENT_TRIGGER;
				BREATH_STATE                                       = PATIENT_TRIGGER_EXPIRATION;
				BIBAP_IPAP_AVERAGE();
				vTaskResume(BIPAP_Handler);
				vTaskSuspend(BIPAP_PID_Handler);

		}
	}
	else
	{
		Blower_Signal(0);
		vTaskResume(BIPAP_Handler);
		vTaskSuspend(BIPAP_PID_Handler);
	}
		vTaskDelay(Ten_Millisecond_Delay);

	}
}


static void BIBAP_CONTROL()
{
	    BREATH_STATE                              = EXPIRATION_CYCLE;
	    if(Wait_Time > 0 )
	    {
	    	Blower_Signal(0);
	    	if(GRAPH_VALUES.pressure <= 3)
	    	{
	    		ExpValve_CLOSE();
	    		Wait_Time                               = 0;
	    	}
	    }

	    else
	    {

			if(BIPAP.EPAP_COUNTER > 0)
			{
				EPAP_TIME_CONTROL();
			}


	    }
}



static void RUN_BIPAP_EPAP_MAINTAIN_BLOWER()
{
	ExpValve_CLOSE();
	Blower_Signal( BIPAP_Mode_Blower_control.EPAP_BLOWER_DAC_VAL);
}


static void STOP_BLOWER_IF_FLOW_SENSE()
{
		if (Trigger_Type == Pressure_Trigger)
		{
			BIPAP_Control.Blower_Wait_Time_Milli_Second = 0;
		}
		else if (Trigger_Type == Flow_Trigger)
		{
			BIPAP_Control.Blower_Wait_Time_Milli_Second = 100;
		}
}


static void CHECK_PRESSURE_TRIGGER()
{
	if (( GRAPH_VALUES.pressure < (Pressure_Trigger_Offset - BIPAP.TRIG_LMT )))
	{
		BREATH_STATE                                 = INSPIRATION_CYCLE;
		Breath_Count                                 = Count_The_Breath;
		Patient_Trigger                              = PATIENT_TRIGGER_HAPPEN;
		BIPAP_Control.Blower_Wait_Time_Milli_Second = 200;
		BIPAP_Control.Trigger_Check_Wait_Time       = 300;
		BIPAP.APNEA_COUNTER                         = (1000*BIPAP.APNEA_TIME) ;
	}
}

static void CHECK_FLOW_TRIGGER()
{
	if ((GRAPH_VALUES.flow > (Flow_Trigger_Offset+BIPAP.TRIG_LMT )))
	{
		BREATH_STATE                                 = INSPIRATION_CYCLE;
		Breath_Count                                 = Count_The_Breath;
		LAST_FLOW_TRIGGER 							= FLOW_TRIGGER;
		Patient_Trigger  							= PATIENT_TRIGGER_HAPPEN;
		BIPAP_Control.Blower_Wait_Time_Milli_Second = 200;
		BIPAP_Control.Trigger_Check_Wait_Time       = 300;
		BIPAP.APNEA_COUNTER                         = (1000*BIPAP.APNEA_TIME) ;
	}
}

static void CHECK_TRIGGER_OFFSET()
{
	if(GRAPH_VALUES.flow>=0)
	{
		FLOW_TRIGGER             = 0;
		Pressure_Trigger_Offset  =  GRAPH_VALUES.pressure;
		Flow_Trigger_Offset      =  GRAPH_VALUES.flow;
	}
}




static void BIPAP_EPAP_AVERAGE()
{
	if((BIPAP_Peep_Average.EPAP_Cumulative_Val == 0) || (BIPAP_Peep_Average.EPAP_Event_Count == 0))
	{
		BIPAP_Peep_Average.EPAP_Cumulative_Val = 1;
		BIPAP_Peep_Average.EPAP_Event_Count    = 1;
	}
	BIPAP_Peep_Average.EPAP_AVG_VAL        = (uint32_t)(BIPAP_Peep_Average.EPAP_Cumulative_Val/ BIPAP_Peep_Average.EPAP_Event_Count);
	BIPAP_Peep_Average.EPAP_Cumulative_Val = RESET;
	BIPAP_Peep_Average.EPAP_Event_Count    = RESET;

	if(BIPAP_Peep_Average.EPAP_AVG_VAL  == BIPAP.EPAP_Val)
	{
		Change_CPAP_DAC_Values_INVASIVE(BIPAP.EPAP_Val,BIPAP_Mode_Blower_control.LAST_EPAP_BLOWER_DAC_VAL);
	}
}



static uint16_t BIPAP_TUNE_EPAP_BLOWER(uint16_t *Last_Blower_DAC_Val,uint8_t Set_PEEP)
{
	if(  ( BIPAP_Peep_Average.EPAP_AVG_VAL)  > ( Set_PEEP ) && (BIPAP_Peep_Average.EPAP_AVG_VAL  < (Set_PEEP + 2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-10);
	}
	else if(  ( BIPAP_Peep_Average.EPAP_AVG_VAL)  >= ( Set_PEEP + 2 ) && (BIPAP_Peep_Average.EPAP_AVG_VAL  < (Set_PEEP + 4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-20);
	}
	else if(  ( BIPAP_Peep_Average.EPAP_AVG_VAL)  >= ( Set_PEEP + 4 )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-40);
	}
	else if( (BIPAP_Peep_Average.EPAP_AVG_VAL  < Set_PEEP) && (BIPAP_Peep_Average.EPAP_AVG_VAL  > (Set_PEEP -2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+10);
	}
	else if(  (BIPAP_Peep_Average.EPAP_AVG_VAL  <= (Set_PEEP-2) ) && (BIPAP_Peep_Average.EPAP_AVG_VAL  >  (Set_PEEP-4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+20);
	}
	else if(  (BIPAP_Peep_Average.EPAP_AVG_VAL  <= (Set_PEEP-4) )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+40);
	}
	else
	{
		*Last_Blower_DAC_Val = *Last_Blower_DAC_Val;
	}
	return *Last_Blower_DAC_Val;
}



static uint16_t BIBAP_TUNE_IPAP_BLOWER(uint16_t *Last_Blower_DAC_Val,uint8_t Set_PIP)
{
	if(  ( BIPAP_Pip_average.IPAP_AVG_VAL)  > ( Set_PIP ) && (BIPAP_Pip_average.IPAP_AVG_VAL  < (Set_PIP + 2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-10);
	}
	else if(  ( BIPAP_Pip_average.IPAP_AVG_VAL)  >= ( Set_PIP + 2 ) && (BIPAP_Pip_average.IPAP_AVG_VAL  < (Set_PIP + 4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-20);
	}
	else if(  ( BIPAP_Pip_average.IPAP_AVG_VAL)  >= ( Set_PIP + 4 )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-40);
	}
	else if( (BIPAP_Pip_average.IPAP_AVG_VAL  < Set_PIP) && (BIPAP_Pip_average.IPAP_AVG_VAL  > (Set_PIP -2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+10);
	}
	else if(  (BIPAP_Pip_average.IPAP_AVG_VAL  <= (Set_PIP-2) ) && (BIPAP_Pip_average.IPAP_AVG_VAL  >  (Set_PIP-4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+20);
	}
	else if(  (BIPAP_Pip_average.IPAP_AVG_VAL  <= (Set_PIP-4) )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+40);
	}
	else
	{
		*Last_Blower_DAC_Val = *Last_Blower_DAC_Val;
	}
	return *Last_Blower_DAC_Val;
}



static void RESET_FLAGS()
{
	Apnea_Mode                                  = OFF;
	Blower_Signal(0);
	Peep_Status                   				= PEEP_NOT_ACHEIVED;
	Expiratory_Valve_Lock_Delay   			    = OPENED;
	Clear_All_Alert_Bits();
	Send_Bluetooth_Data_Farme                   = SEND_ALERT_FRAME;
	Apnea_Alert                                 = APNEA_ALERT_HAPPEN;
	Alert_Status                                = ALERT_HAPPEN;
	SET_ALERT_BIT(SECOND_FRAME_UN,_ALERT_APNEA);
	LED_Alert();

}


void INSPIRATION_PROCESS_BIPAP()
{
	//Breath_Count                                          = Count_The_Breath;

	O2_Control_Bipap++;
	if(O2_Control_Bipap > 6)
		O2_Control_Bipap=6;
	BIPAP_Control._INSPIRATION_TIME                       = BIPAP.IPAP_TIME;

		Pressure_Mode_blower_control.BLOWER_DAC_VAL           = APNEA_DAC_VAL(BIPAP.EPAP_Val);
		if(BIPAP.BIPAP_O2_Val > 21)
		{
				Parkar_valve_Signal((Oxygen_Parameter.BIPAP_O2_DAC+100));
		}

		PIP_control_Task.PIP_Control_Task_Delay               = 10;
		PIP_control_Task.Increase_Each_DAC_SET_CONST          = 30;
		PIP_Status                    				          = PIP_NOT_ACHEIVED;


	Pressure_Mode_blower_control.LAST_BLOWER_DAC_VAL      = Pressure_Mode_blower_control.BLOWER_DAC_VAL;
	Set_PIP_Status                                        = SET_PIP_VALUE_NOT_ACHEIVED;
	Read_Time_vs_Pressure                                 = READ_TIME_OPEN;
	PIP_control_Task.PIP_Control_Event_Occured            = RESET;
	PIP_Average_Parameter.Maximum_PIP_Acheived            = RESET;
	Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = BIBAP_TUNE_IPAP_BLOWER(&Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL,
			                                                                      BIPAP.IPAP_Val );
	BREATH_STATE                                          = INSPIRATION_CYCLE;
	GRAPH_VALUES.volume                                   = RESET;
	Volume_max                                            = RESET;
	BIPAP_EPAP_AVERAGE();
	BIPAP_Control.Trigger_Check_Wait_Time                = 500+BIPAP.TRIG_TIME;

	vTaskResume(BIPAP_PID_Handler);
	vTaskSuspend(BIPAP_Handler);

}




static void BACKUP_PC_CMV_START()
{
	if(Backup_PC_CMV_SET_VALUE == CHANGE_HAPPEN)
	{
			Mode                                                  = Backup_Pccmv;
			Peep_Status                   				          = PEEP_NOT_ACHEIVED;
			Expiratory_Valve_Lock_Delay   			              = OPENED;
			Oxygen_Parameter.O2_DAC=1700;

				Maximum_Limit=3;
				if(Common_Mode_Parameter._FIO2_Val == 100)
				{
					Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = APNEA_DAC_VAL(BACKUP_PC_CMV.PIP_Val);
					Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL  = Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL -200;
				}
				else
				{
					Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = APNEA_DAC_VAL(BACKUP_PC_CMV.PIP_Val);
				}


		    	PIP_control_Task.PIP_Control_Task_Delay           = 10;                               //new
		    	PIP_control_Task.Increase_Each_DAC_SET_CONST      = 30;                               //new
		    	BACKUP_PC_CMV.RISE_TIME_MS_Val                    = (BACKUP_PC_CMV.Rise_Time *100);

		    	if(BACKUP_PC_CMV.RISE_TIME_MS_Val >= BACKUP_PC_CMV.INSPIRATION_TIME)                  //new
		    	{
		    		BACKUP_PC_CMV.RISE_TIME_MS_Val =BACKUP_PC_CMV.INSPIRATION_TIME;
		    	}
		    	if(BACKUP_PC_CMV.RISE_TIME_MS_Val <=600)
		    	{
		    		BACKUP_PC_CMV.RISE_TIME_MS_Val =600;
		    	}
		    	BACKUP_PC_CMV.Rise_Time_percentage   = ((float)BACKUP_PC_CMV.RISE_TIME_MS_Val /BACKUP_PC_CMV.INSPIRATION_TIME) * 100.00;     //new
		    	PIP_control_Task.PIP_Acheived_Time_Ms      	 =  (BACKUP_PC_CMV.RISE_TIME_MS_Val * 2);



				Smooth_Ramp_Time_Val_Count = 0;
				Smooth_Ramp_Time_Val       = 15;
				if(Common_Mode_Parameter._PEEP_Val >= 15)
				{
				  Smooth_Ramp_Time           = Smooth_Ramp_Time_Val_Finished;
				}
				else if( (Common_Mode_Parameter._PEEP_Val < 15) && (Common_Mode_Parameter._PEEP_Val >= 9))
				{
				  Smooth_Ramp_Time           = Smooth_Ramp_Time_Val_Pending;
				}
				else
				{
					Smooth_Ramp_Time           = Smooth_Ramp_Time_Val_Finished;
				}

				controlsys_Init(&Parker_valve_tune);          //parker
				Avg_Flow                                                   = 1;
				Flow_Count                                                 = 0;
				Flow_Cummulative_Val                                       = 0;
				Check_Oxygen_Count                                         = 0;        //new
				Temp_Dac                                                   = 1400;     //new
				inc_dac                                                    = 2;        //new
				Oxygen_Blending_Status                                     = OXYGEN_NOT_ACHIEVED;  //new


			PIP_Average_Parameter.PIP_Cumulative_Val              = 1;
			PIP_Average_Parameter.PIP_Event_Count                 = 1;
			PEEP_Maintain_Parameter.PEEP_Cumulative_Val           = 1;
			PEEP_Maintain_Parameter.PEEP_Event_Count              = 1;
			Need_PID_Task_Delay                                   = NO;
			Tune_PID_Task_Delay                                   = T_OPEN;
			PIP_control_Task.Low_PIP_Count                        = RESET;
			PIP_control_Task.Last_Early_Acheived_Ramp_Time_Percentage  = RESET;
			Next_Half_Breath_Cycle                                = Generate_Expiration_Cycle;
			Backup_PC_CMV_SET_VALUE = NO_CHANGES;
	}
	else if(Backup_PC_CMV_SET_VALUE == NO_CHANGES)
	{
		Mode                                                  = Backup_Pccmv;
		Peep_Status                   				          = PEEP_NOT_ACHEIVED;
		Expiratory_Valve_Lock_Delay   			              = OPENED;
		Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = OLD.LAST_BLOWER_ENDING_DAC_SIGNAL;;
		PIP_control_Task.PIP_Control_Task_Delay               = OLD.LAST_PIP_Control_Task_Delay;
		PIP_control_Task.Increase_Each_DAC_SET_CONST          = OLD.LAST_Increase_Each_DAC_SET_CONST;
		PEEP_Maintain_Parameter.PEEP_Cumulative_Val           = PEEP_Maintain_Parameter.PEEP_AVG_VAL;
		PEEP_Maintain_Parameter.PEEP_Event_Count              = 1;
		Next_Half_Breath_Cycle                                = Generate_Expiration_Cycle;

	}
	Alert_Check = 5;
	vTaskResume(One_Time_Handler);
	vTaskResume(BACKUP_PC_Handler);
	vTaskResume(PIP_Control_Task_Handler);
	vTaskResume(Alert_Handler);
	if(Common_Mode_Parameter._FIO2_Val > 21)
	{
		vTaskResume(Oxygen_Blending_Handler);
	}
	else
	{
		vTaskSuspend(Oxygen_Blending_Handler);
	}

	vTaskSuspend(BIPAP_PID_Handler);
	vTaskSuspend(BIPAP_Handler);
}






static void BIBAP_IPAP_AVERAGE()
{
	if((BIPAP_Pip_average.IPAP_Cumulative_Val == 0) || (BIPAP_Pip_average.IPAP_Event_Count == 0))
	{
		BIPAP_Pip_average.IPAP_Cumulative_Val = 1;
		BIPAP_Pip_average.IPAP_Event_Count    = 1;
	}
	BIPAP_Pip_average.IPAP_AVG_VAL        = (uint32_t)(BIPAP_Pip_average.IPAP_Cumulative_Val/ BIPAP_Pip_average.IPAP_Event_Count);
	BIPAP_Pip_average.IPAP_Cumulative_Val = RESET;
	BIPAP_Pip_average.IPAP_Event_Count    = RESET;
	if(BIPAP_Pip_average.IPAP_AVG_VAL == BIPAP.IPAP_Val)
	{
		Change_CPAP_DAC_Values_INVASIVE(BIPAP.IPAP_Val,Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL);
	}
}



static void Patient_Circuit_Disconnected_Alert_BIBAP()
{
	if(  (GRAPH_VALUES.pressure<=3) && (GRAPH_VALUES.flow==0 || GRAPH_VALUES.flow==-1 || GRAPH_VALUES.flow==-2 || ( (GRAPH_VALUES.flow>= 80) && (GRAPH_VALUES.pressure<=3) ) ) )
	{
			if((DAC1->DHR12R1)>=400)
			{
				if(PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK == CHECK)
				{
					Alert_Count.Patient_Circuit_disconnected_Alert_Count++;
						if(Alert_Count.Patient_Circuit_disconnected_Alert_Count >= 500)
						{
							Clear_All_Alert_Bits();
							Alert_Status = ALERT_HAPPEN;
							SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_PATIENT_CIRCUIT_DISCONNECTED);
							PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = DO_NOT_CHECK;

							Send_Bluetooth_Data_Farme                = SEND_ALERT_FRAME;
							Apnea_Alert                              = APNEA_ALERT_HAPPEN;
							PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = DO_NOT_CHECK;
							LED_Alert();

							BIPAP_Mode_Blower_control.EPAP_BLOWER_DAC_VAL           = APNEA_DAC_VAL(BIPAP.EPAP_Val );

							BIPAP_Mode_Blower_control.LAST_EPAP_BLOWER_DAC_VAL      = BIPAP_Mode_Blower_control.EPAP_BLOWER_DAC_VAL;

						}
				}
			}
	}
}


void EPAP_TIME_CONTROL()
{
	Apnea_Mode  = ON;
    if(BIPAP_Control.Blower_Wait_Time_Milli_Second == 0)
    {
    	RUN_BIPAP_EPAP_MAINTAIN_BLOWER();
    }

	Patient_Circuit_Disconnected_Alert_BIBAP();
    Apnea_O2_control();

    if(BIPAP_Control.Trigger_Check_Wait_Time == 0)
	{
		if(GRAPH_VALUES.flow > 0)
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

				Cummulate_PEEP();

	   }
	}

	else
	{
		CHECK_TRIGGER_OFFSET();
	}



}

void Cummulate_PEEP()
{
	BIPAP_Peep_Average.EPAP_Cumulative_Val += GRAPH_VALUES.pressure;
	BIPAP_Peep_Average.EPAP_Event_Count++;
}

void ADJUST_PEEP()
{
	//Reset_Patient_Circuit_Diconnceted_Alert_Flags();
	BIPAP_Mode_Blower_control.LAST_EPAP_BLOWER_DAC_VAL    = BIPAP_TUNE_EPAP_BLOWER(&BIPAP_Mode_Blower_control.LAST_EPAP_BLOWER_DAC_VAL,
			                                                                       BIPAP.EPAP_Val );
	BIPAP_Mode_Blower_control.EPAP_BLOWER_DAC_VAL         = BIPAP_Mode_Blower_control.LAST_EPAP_BLOWER_DAC_VAL;
}


void Inspiration_Trigger_Check()
{


    if(BIPAP_Control.Blower_Wait_Time_Milli_Second == 0)
    {

    }

    if(BIPAP_Control.Trigger_Check_Wait_Time == 0)
	{
		if(GRAPH_VALUES.flow > 0)
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

static void Apnea_O2_control()
{
	if(BIPAP.BIPAP_O2_Val > 21)
	{
		Oxygen_epap();
	}
}


void Oxygen_epap()
{

	if(O2_Control_Bipap < 6)
	{
		O2_Flow_Rate   = ( GRAPH_VALUES.flow* ( (float)( BIPAP.BIPAP_O2_Val - 21) / 100) );
		if(O2_Flow_Rate <=8)
		{
			O2_Flow_Rate = 8;
		}
		if(GRAPH_VALUES.oxygen_flow <= O2_Flow_Rate)
		{
			Oxygen_Parameter.BIPAP_O2_DAC=controlsys_Update(&Parker_valve_tune, O2_Flow_Rate , GRAPH_VALUES.oxygen_flow);
			Parkar_valve_Signal(Oxygen_Parameter.BIPAP_O2_DAC);
		}
		else
		{
			Oxygen_Parameter.BIPAP_O2_DAC=controlsys_Update(&Parker_valve_tune, O2_Flow_Rate , GRAPH_VALUES.oxygen_flow);
			Parkar_valve_Signal(Oxygen_Parameter.BIPAP_O2_DAC);
		}
		Parkar_valve_Signal(Oxygen_Parameter.BIPAP_O2_DAC);
		Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;
	}


	Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;
}






void Low_Oxygen_Achieved_BIPAP(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{


		if(  ( Achieved_Oxygen)  < ( FIO2_Val) && (Achieved_Oxygen  >= (FIO2_Val - 2) ) )
		{
			Oxygen_Parameter.BIPAP_O2_DAC = (Oxygen_Parameter.BIPAP_O2_DAC+ET_O2_Envitec_Tune_Value1);
		}
		else if(  ( Achieved_Oxygen)  < ( FIO2_Val - 2 ) && (Achieved_Oxygen  > (FIO2_Val - 5) ) )
		{
			Oxygen_Parameter.BIPAP_O2_DAC = (Oxygen_Parameter.BIPAP_O2_DAC+ET_O2_Envitec_Tune_Value2);
		}
		else if(  ( Achieved_Oxygen)  <= ( FIO2_Val - 5 )  )
		{
			Oxygen_Parameter.BIPAP_O2_DAC = (Oxygen_Parameter.BIPAP_O2_DAC+ET_O2_Envitec_Tune_Value3);
		}

		if(Oxygen_Parameter.BIPAP_O2_DAC>=3000)
		{
			Oxygen_Parameter.BIPAP_O2_DAC=3000;
		}
		Parkar_valve_Signal(Oxygen_Parameter.BIPAP_O2_DAC);
}


/**
  * This function used to reduce the Proportional valve when high O2 Achieved.
  */


void High_Oxygen_Achieved_BIPAP(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{

		if(  ( Achieved_Oxygen)  > ( FIO2_Val ) && (Achieved_Oxygen  <= (FIO2_Val + 2) ) )
		{
			Oxygen_Parameter.BIPAP_O2_DAC = (Oxygen_Parameter.BIPAP_O2_DAC-ET_O2_Envitec_Tune_Value1);
		}
		else if(  ( Achieved_Oxygen)  > ( FIO2_Val + 2 ) && (Achieved_Oxygen  < (FIO2_Val + 5) ) )
		{
			Oxygen_Parameter.BIPAP_O2_DAC = (Oxygen_Parameter.BIPAP_O2_DAC-ET_O2_Envitec_Tune_Value2);
		}
		else if(  ( Achieved_Oxygen)  >= ( FIO2_Val + 5 )  )
		{
			Oxygen_Parameter.BIPAP_O2_DAC = (Oxygen_Parameter.BIPAP_O2_DAC-ET_O2_Envitec_Tune_Value3);
		}


		if(Oxygen_Parameter.BIPAP_O2_DAC<=1400)
		{
			Oxygen_Parameter.BIPAP_O2_DAC=1400;
		}
	    Parkar_valve_Signal(Oxygen_Parameter.BIPAP_O2_DAC);
}



/**
  * This function check Achieved O2 high or low
  */

void Tune_O2_BIPAP(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{
	   if (Achieved_Oxygen < (FIO2_Val-1))
		{
			Low_Oxygen_Achieved_BIPAP(FIO2_Val,Achieved_Oxygen);
		}
		else if (Achieved_Oxygen > (FIO2_Val))
		{
			High_Oxygen_Achieved_BIPAP(FIO2_Val,Achieved_Oxygen);
		}
		else
			Parkar_valve_Signal(Oxygen_Parameter.BIPAP_O2_DAC);
		Last_O2_Dac_Val = Oxygen_Parameter.BIPAP_O2_DAC;
		Parkar_valve_Signal(Oxygen_Parameter.BIPAP_O2_DAC);
}
