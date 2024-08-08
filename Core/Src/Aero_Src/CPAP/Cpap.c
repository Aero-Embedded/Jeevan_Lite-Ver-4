/*
 * Cpap.c
 *
 *  Created on: Feb 27, 2023
 *      Author: asus
 */


#include "Cpap.h"


static void CPAP_CONTROL();
static void RUN_CPAP_MAINTAIN_BLOWER();
static void STOP_BLOWER_IF_FLOW_SENSE();
static void CHECK_PRESSURE_TRIGGER();
static void CHECK_FLOW_TRIGGER();
static void CHECK_TRIGGER_OFFSET();
static void CPAP_AVERAGE();
static uint16_t TUNE_CPAP_BLOWER(uint16_t *,uint8_t);
static void RESET_FLAGS();
static void BACKUP_PC_CMV_START();
static void Patient_Circuit_Disconnected_Alert_CPAP();
static void CPAP_O2_Control();



uint8_t Valve_Condition=0;



void Tune_O2_CPAP(uint8_t,uint8_t);
void Low_Oxygen_Achieved_CPAP(uint8_t ,uint8_t );
void High_Oxygen_Achieved_CPAP(uint8_t ,uint8_t);





void CPAP_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet)
{
	 CPAP.CPAP_Val                         = Receive_Graph_Packet->PEEP_CPAP_Plow;
	 CPAP.TRIG_TYPE                        = (0x40 & (Receive_Graph_Packet->Control_Byte))>>6;
	 CPAP.TRIG_LMT                         = ( (Receive_Graph_Packet->Trigger_Limit) / 10);
	 CPAP.TRIG_TIME                        = ( (Receive_Graph_Packet->Trigger_Time) * 100);
	 CPAP.APNEA_TIME                       = Receive_Graph_Packet->Apnea_Time;
	 CPAP.APNEA_COUNTER                    = (1000*CPAP.APNEA_TIME) ;
	 CPAP.CPAP_O2_Val                      = Receive_Graph_Packet->FiO2;
	 CPAP.VENT_TYPE                        = ( 0x80 & (Receive_Graph_Packet->Vent_Type_Min_O2)) >> 7;

	 OOM202.offset_minimum                 =  ( 0x7F & (Receive_Graph_Packet->Vent_Type_Min_O2));
	 OOM202.offset_maximum                 =  Receive_Graph_Packet->Max_O2;
	 if(CPAP.VENT_TYPE == 0)
		 Vent_Type = ET_TUBE_MODE;
	 else if(CPAP.VENT_TYPE == 1)
		 Vent_Type = MASK_MODE;
	 if(CPAP.TRIG_TYPE == 0)
		Trigger_Type = Pressure_Trigger;
	 else if(CPAP.TRIG_TYPE == 1)
		Trigger_Type = Flow_Trigger;
	 Mode_Initial_Flag_Set                  = OPEN;
	 vTaskResume(Mode_initial_Flag_Set_Handler);

}


void CPAP_Task(void *argument)
{
	while(1)
	{
		if(CPAP.APNEA_COUNTER > 0)
		{
			CPAP_CONTROL();
		}
		else
		{
			RESET_FLAGS();
			 if(Current_Backup_Mode == Backup_PCCMV_Mode)
			 {
				 BACKUP_PC_CMV_START();
			 }

		}
		vTaskDelay(Two_Millisecond_Delay);
	}
}



static void CPAP_CONTROL()
{
	        Apnea_Mode  = ON;

				if(CPAP_Control.Blower_Wait_Time_Milli_Second == 0)
				{
					RUN_CPAP_MAINTAIN_BLOWER();
				}
				if(CPAP_Control.CPAP_Settle_Time == 0 )
				{

					CPAP_O2_Control();
					BREATH_STATE    = EXPIRATION_CYCLE;

					Patient_Circuit_Disconnected_Alert_CPAP();
					Check_O2_Supply_Alert_in_Apnea(CPAP.CPAP_O2_Val,Patient_Trigger_Count);
					GRAPH_VALUES.Maximum_oxygen_flow = 0;



					if( (CPAP_Control.Trigger_Check_Wait_Time == 0) && ( GRAPH_VALUES.pressure <(Pressure_Trigger_Offset) ) )
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
					else
					{
						CPAP_Average.CPAP_Cumulative_Val += GRAPH_VALUES.pressure;
						CPAP_Average.CPAP_Event_Count++;

						Cpap_Flow_Cummulative_val = Cpap_Flow_Cummulative_val+GRAPH_VALUES.flow;
						Flow_Sensor_Count++;

						if(CPAP_Average.CPAP_Event_Count >600)
						{
							Reset_Patient_Circuit_Diconnceted_Alert_Flags();
							CPAP_AVERAGE();
							CPAP_Mode_Blower_control.LAST_CPAP_BLOWER_DAC_VAL    = TUNE_CPAP_BLOWER(&CPAP_Mode_Blower_control.LAST_CPAP_BLOWER_DAC_VAL ,
									                                                                CPAP.CPAP_Val );
							CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL         = CPAP_Mode_Blower_control.LAST_CPAP_BLOWER_DAC_VAL;

							if(CPAP_Mode_Blower_control.LAST_CPAP_BLOWER_DAC_VAL <= 500)
								CPAP_Mode_Blower_control.LAST_CPAP_BLOWER_DAC_VAL = 500;
							if(CPAP_Mode_Blower_control.LAST_CPAP_BLOWER_DAC_VAL >= 3500)
								CPAP_Mode_Blower_control.LAST_CPAP_BLOWER_DAC_VAL = 3500;

							CPAP_Average.CPAP_Cumulative_Val = RESET;
							CPAP_Average.CPAP_Event_Count    = RESET;
						}



						if(CPAP_Average.CPAP_Event_Count < 300)
						{
							if( GRAPH_VALUES.flow  <= (Flow_Trigger_Offset + 2) )
							{
							   CHECK_TRIGGER_OFFSET();
							}
						}
					}
				}
				else
				{
					CHECK_TRIGGER_OFFSET();
				}

}



static void RUN_CPAP_MAINTAIN_BLOWER()
{
	ExpValve_CLOSE();
	Blower_Signal( CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL );
}


static void STOP_BLOWER_IF_FLOW_SENSE()
{
	if (Trigger_Type == Pressure_Trigger)
	{
		CPAP_Control.Blower_Wait_Time_Milli_Second = 500;
	}
	else if (Trigger_Type == Flow_Trigger)
	{
		CPAP_Control.Blower_Wait_Time_Milli_Second = 1000;
	}
}




static void CHECK_PRESSURE_TRIGGER()
{
	if (( GRAPH_VALUES.pressure < (Pressure_Trigger_Offset - CPAP.TRIG_LMT )))
	{

		BREATH_STATE                                 = INSPIRATION_CYCLE;
		Breath_Count                                 = Count_The_Breath;
		GRAPH_VALUES.volume                          = RESET;
		Volume_max                                   = RESET;
		Alert_Status                                 = ALERT_HAPPEN;
		LED_Alert();
		Patient_Trigger                              = PATIENT_TRIGGER_HAPPEN;
		Patient_Trigger_Count++;
		Blower_Signal( CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL);
		CPAP_Control.CPAP_Settle_Time                = (1000+CPAP.TRIG_TIME);
		CPAP.APNEA_COUNTER                           = (1000*CPAP.APNEA_TIME) ;
		Send_Frame                                   = First_Frame;
		Send_Bluetooth_Data_Farme                    = SEND_SAMPLED_PARAMETER_FRAME;
		Cpap_Flow                                    = 0;

	}
}

static void CHECK_FLOW_TRIGGER()
{
	if ((GRAPH_VALUES.flow > (Flow_Trigger_Offset+CPAP.TRIG_LMT )))
	{
		BREATH_STATE                                = INSPIRATION_CYCLE;
		Breath_Count                                = Count_The_Breath;
		GRAPH_VALUES.volume                         = RESET;
		Volume_max                                  = RESET;
		Alert_Status                                = ALERT_HAPPEN;
		LED_Alert();
		LAST_FLOW_TRIGGER 							= FLOW_TRIGGER;
		Patient_Trigger  							= PATIENT_TRIGGER_HAPPEN;
		Patient_Trigger_Count++;
		Blower_Signal( CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL);
		CPAP_Control.CPAP_Settle_Time               = (1000 + CPAP.TRIG_TIME);
		CPAP.APNEA_COUNTER                          = (1000*CPAP.APNEA_TIME) ;
		Send_Frame                                  = First_Frame;
		Send_Bluetooth_Data_Farme                   = SEND_SAMPLED_PARAMETER_FRAME;
		Cpap_Flow                                    = 0;


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


static void CPAP_AVERAGE()
{
	if((CPAP_Average.CPAP_Cumulative_Val == 0) || (CPAP_Average.CPAP_Event_Count == 0))
	{
		CPAP_Average.CPAP_Cumulative_Val = 1;
		CPAP_Average.CPAP_Event_Count    = 1;
	}
	CPAP_Average.CPAP_AVG_VAL        = (uint32_t)(CPAP_Average.CPAP_Cumulative_Val/ CPAP_Average.CPAP_Event_Count);
	if(CPAP_Average.CPAP_AVG_VAL   == CPAP.CPAP_Val)
	{
		Change_CPAP_DAC_Values_INVASIVE(CPAP.CPAP_Val,CPAP_Mode_Blower_control.LAST_CPAP_BLOWER_DAC_VAL );
	}
	else
	{
		if (Trigger_Type == Flow_Trigger)
		{
			CPAP_Control.CPAP_Settle_Time   = 1000;
		}

	}
}



static uint16_t TUNE_CPAP_BLOWER(uint16_t *Last_Blower_DAC_Val,uint8_t Set_PEEP)
{
	if(  ( CPAP_Average.CPAP_AVG_VAL)  > ( Set_PEEP ) && (CPAP_Average.CPAP_AVG_VAL  < (Set_PEEP + 2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-10);
	}
	else if(  ( CPAP_Average.CPAP_AVG_VAL)  >= ( Set_PEEP + 2 ) && (CPAP_Average.CPAP_AVG_VAL  < (Set_PEEP + 4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-20);
	}
	else if(  ( CPAP_Average.CPAP_AVG_VAL)  >= ( Set_PEEP + 4 )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-40);
	}
	else if( (CPAP_Average.CPAP_AVG_VAL  < Set_PEEP) && (CPAP_Average.CPAP_AVG_VAL  > (Set_PEEP -2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+10);
	}
	else if(  (CPAP_Average.CPAP_AVG_VAL  <= (Set_PEEP-2) ) && (CPAP_Average.CPAP_AVG_VAL  >  (Set_PEEP-4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+20);
	}
	else if(  (CPAP_Average.CPAP_AVG_VAL  <= (Set_PEEP-4) )  )
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
	Patient_Trigger_Count                       = 0;
	Clear_All_Alert_Bits();
	Send_Bluetooth_Data_Farme                   = SEND_ALERT_FRAME;
	Apnea_Alert                                 = APNEA_ALERT_HAPPEN;
	Alert_Status                                = ALERT_HAPPEN;
	SET_ALERT_BIT(SECOND_FRAME_UN,_ALERT_APNEA);
	LED_Alert();

}



static void BACKUP_PC_CMV_START()
{

	if(Backup_PC_CMV_SET_VALUE == CHANGE_HAPPEN)
	{
		Mode                                                  = Backup_Pccmv;
		Peep_Status                   				          = PEEP_NOT_ACHEIVED;
		Expiratory_Valve_Lock_Delay   			              = OPENED;
		Oxygen_Parameter.O2_DAC                               = 1700;

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
		PIP_Average_Parameter.PIP_Cumulative_Val                   = 1;
		PIP_Average_Parameter.PIP_Event_Count                      = 1;
		PEEP_Maintain_Parameter.PEEP_Cumulative_Val                = 1;
		PEEP_Maintain_Parameter.PEEP_Event_Count                   = 1;
		Need_PID_Task_Delay                                        = NO;
		Tune_PID_Task_Delay                                        = T_OPEN;
		PIP_control_Task.Low_PIP_Count                             = RESET;
		PIP_control_Task.Last_Early_Acheived_Ramp_Time_Percentage  = RESET;
		Next_Half_Breath_Cycle                                     = Generate_Expiration_Cycle;
		Backup_PC_CMV_SET_VALUE                                    = NO_CHANGES;
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
	vTaskSuspend(CPAP_Handler);
}







static void Patient_Circuit_Disconnected_Alert_CPAP()
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

							CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL           = APNEA_DAC_VAL(CPAP.CPAP_Val );
							CPAP_Mode_Blower_control.LAST_CPAP_BLOWER_DAC_VAL      = CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL;

						}
				}
			}
	}

}






static void CPAP_O2_Control()
{
	if(CPAP.CPAP_O2_Val > 21)
	{
		if(Patient_Trigger_Count <=5)
		{
			O2_Flow_Rate   = ( GRAPH_VALUES.flow* ( (float)( CPAP.CPAP_O2_Val - 21) / 100) );
			if(O2_Flow_Rate <=8)
			{
				O2_Flow_Rate = 8;
			}
			if(GRAPH_VALUES.oxygen_flow <= O2_Flow_Rate)
			{
				Oxygen_Parameter.CPAP_O2_DAC=controlsys_Update(&Parker_valve_tune, O2_Flow_Rate , GRAPH_VALUES.oxygen_flow);
				Parkar_valve_Signal(Oxygen_Parameter.CPAP_O2_DAC);
			}
			else
			{
				Oxygen_Parameter.CPAP_O2_DAC=controlsys_Update(&Parker_valve_tune, O2_Flow_Rate , GRAPH_VALUES.oxygen_flow);
				Parkar_valve_Signal(Oxygen_Parameter.CPAP_O2_DAC);
			}
			Parkar_valve_Signal(Oxygen_Parameter.CPAP_O2_DAC);
			Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;
		}
		else
		{
			CPAP_Average.CPAP_O2_Event_Count++;
			if(CPAP_Average.CPAP_O2_Event_Count>500)
			{
				Check_O2_Supply_Alert_in_Apnea_Once(CPAP.CPAP_O2_Val);
				GRAPH_VALUES.Maximum_oxygen_flow = 0;
				Tune_O2_CPAP(CPAP.CPAP_O2_Val,Oxygen_Parameter.Achieved_Oxygen);
				CPAP_Average.CPAP_O2_Event_Count = 0;

			}
		}

		Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;


	}
	else
	{
		Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;
	}
}



void Low_Oxygen_Achieved_CPAP(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{


		if(  ( Achieved_Oxygen)  < ( FIO2_Val) && (Achieved_Oxygen  >= (FIO2_Val - 2) ) )
		{
			Oxygen_Parameter.CPAP_O2_DAC = (Oxygen_Parameter.CPAP_O2_DAC+ET_O2_Envitec_Tune_Value1);
		}
		else if(  ( Achieved_Oxygen)  < ( FIO2_Val - 2 ) && (Achieved_Oxygen  > (FIO2_Val - 5) ) )
		{
			Oxygen_Parameter.CPAP_O2_DAC = (Oxygen_Parameter.CPAP_O2_DAC+ET_O2_Envitec_Tune_Value2);
		}
		else if(  ( Achieved_Oxygen)  <= ( FIO2_Val - 5 )  )
		{
			Oxygen_Parameter.CPAP_O2_DAC = (Oxygen_Parameter.CPAP_O2_DAC+ET_O2_Envitec_Tune_Value3);
		}

		if(Oxygen_Parameter.CPAP_O2_DAC>=3000)
		{
			Oxygen_Parameter.CPAP_O2_DAC=3000;
		}
		Parkar_valve_Signal(Oxygen_Parameter.CPAP_O2_DAC);
}


/**
  * This function used to reduce the Proportional valve when high O2 Achieved.
  */


void High_Oxygen_Achieved_CPAP(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{

		if(  ( Achieved_Oxygen)  > ( FIO2_Val ) && (Achieved_Oxygen  <= (FIO2_Val + 2) ) )
		{
			Oxygen_Parameter.CPAP_O2_DAC = (Oxygen_Parameter.CPAP_O2_DAC-ET_O2_Envitec_Tune_Value1);
		}
		else if(  ( Achieved_Oxygen)  > ( FIO2_Val + 2 ) && (Achieved_Oxygen  < (FIO2_Val + 5) ) )
		{
			Oxygen_Parameter.CPAP_O2_DAC = (Oxygen_Parameter.CPAP_O2_DAC-ET_O2_Envitec_Tune_Value2);
		}
		else if(  ( Achieved_Oxygen)  >= ( FIO2_Val + 5 )  )
		{
			Oxygen_Parameter.CPAP_O2_DAC = (Oxygen_Parameter.CPAP_O2_DAC-ET_O2_Envitec_Tune_Value3);
		}


		if(Oxygen_Parameter.CPAP_O2_DAC<=1400)
		{
			Oxygen_Parameter.CPAP_O2_DAC=1400;
		}
	    Parkar_valve_Signal(Oxygen_Parameter.CPAP_O2_DAC);
}



/**
  * This function check Achieved O2 high or low
  */

void Tune_O2_CPAP(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{
	   if (Achieved_Oxygen < (FIO2_Val-1))
		{
			Low_Oxygen_Achieved_CPAP(FIO2_Val,Achieved_Oxygen);
		}
		else if (Achieved_Oxygen > (FIO2_Val))
		{
			High_Oxygen_Achieved_CPAP(FIO2_Val,Achieved_Oxygen);
		}
		else
			Parkar_valve_Signal(Oxygen_Parameter.CPAP_O2_DAC);
		Last_O2_Dac_Val = Oxygen_Parameter.CPAP_O2_DAC;
		Parkar_valve_Signal(Oxygen_Parameter.CPAP_O2_DAC);
}



void Check_O2_Supply_Alert_in_Apnea(int Fio2_Val,uint32_t Patient_Trigger_Count)
{
		if(Fio2_Val > 21)
		{
			if(Patient_Trigger_Count <=5)
			{
				Check_O2_Supply_Alert_Count_in_Apnea++;
				if(Check_O2_Supply_Alert_Count_in_Apnea > 500)
				{

					if(GRAPH_VALUES.Maximum_oxygen_flow <5)
					{
							Alert_Count.FIO2_Supply_Alert_Count++;
							if(Alert_Count.FIO2_Supply_Alert_Count>2)
							{
								  SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_OXYGEN_SUPPLY_FAILED);
								  Alert_Status                        = ALERT_HAPPEN;
								  Alert_Count.FIO2_Supply_Alert_Count = 0;
								  Oxygen_Supply_Failed                = YES;


								 Send_Bluetooth_Data_Farme    = SEND_ALERT_FRAME;
								 Apnea_Alert                  = APNEA_ALERT_HAPPEN;
								 LED_Alert();
							}
					}
					else
					{
						Alert_Count.FIO2_Supply_Alert_Count=0;
						Oxygen_Supply_Failed = NO;
					}

					Check_O2_Supply_Alert_Count_in_Apnea  = 0;
				}
			}
		}
}





void Check_O2_Supply_Alert_in_Apnea_Once(int Fio2_Val)
{
		if(Fio2_Val > 21)
		{
				if(GRAPH_VALUES.Maximum_oxygen_flow <5)
				{
						Alert_Count.FIO2_Supply_Alert_Count++;
						if(Alert_Count.FIO2_Supply_Alert_Count>2)
						{
							  SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_OXYGEN_SUPPLY_FAILED);
							  Alert_Status                        = ALERT_HAPPEN;
							  Alert_Count.FIO2_Supply_Alert_Count = 0;
							  Oxygen_Supply_Failed                = YES;


							 Send_Bluetooth_Data_Farme    = SEND_ALERT_FRAME;
							 Apnea_Alert                  = APNEA_ALERT_HAPPEN;
							 LED_Alert();
						}
				}
				else
				{
					Alert_Count.FIO2_Supply_Alert_Count=0;
					Oxygen_Supply_Failed = NO;
				}
		}
}
