/*
 * PSV.c
 *
 *  Created on: Feb 18, 2023
 *      Author: asus
 */


#include "PSV.h"


static void PSV_CONTROL();
static void STOP_BLOWER_IF_FLOW_SENSE();
static void CHECK_PRESSURE_TRIGGER();
static void CHECK_FLOW_TRIGGER();
static void CHECK_TRIGGER_OFFSET();
static void RUN_PSV_PEEP_MAINTAIN_BLOWER();
static void PSV_PEEP_AVERAGE();
static uint16_t PSV_TUNE_PEEP_BLOWER(uint16_t *,uint8_t);
static void PSV_PIP_AVERAGE();
static uint16_t PSV_TUNE_PIP_BLOWER(uint16_t *,uint8_t );
static void BACKUP_PC_CMV_START();
static void RESET_FLAGS();
static void INSPIRATION_PROCESS_PSV();
static void Patient_Circuit_Disconnected_Alert_PSV();



//static uint16_t Valve_Control(uint8_t,uint8_t,uint16_t);


uint8_t Psv_Set_Peep=3;
uint16_t O2_max_Control      = 2000;
uint16_t O2_max_Control_Low  = 1700;


static void Apnea_O2_control();
static void WAIT_TIME_CONTROL();
static void Expiration_Cycle();
static void For_Trigger_Control();
static void MASK_MODE_PID();
static void ET_TUBE_MODE_PID();





void Tune_O2_PSV(uint8_t,uint8_t);
void Low_Oxygen_Achieved_PSV(uint8_t ,uint8_t );
void High_Oxygen_Achieved_PSV(uint8_t ,uint8_t);
void Oxygen_PEEP();


void PSV_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet)
{
	 PSV.PRESSURE_SUPPORT_Val             = Receive_Graph_Packet->PIP_PS_Phigh;
	 PSV.PEEP_Val                         = Receive_Graph_Packet->PEEP_CPAP_Plow;

	 PSV.TRIG_TYPE                        = (0x40 & (Receive_Graph_Packet->Control_Byte))>>6;
	 PSV.TRIG_LMT                         = ( (Receive_Graph_Packet->Trigger_Limit) / 10);
	 PSV.TRIG_TIME                        = ( (Receive_Graph_Packet->Trigger_Time) * 100);

	 PSV.APNEA_TIME                       = Receive_Graph_Packet->Apnea_Time;
	 PSV.APNEA_COUNTER                    = (1000*PSV.APNEA_TIME) ;

	 PSV.PSV_O2_Val                       = Receive_Graph_Packet->FiO2;
	 PSV.VENT_TYPE                        = ( 0x80 & (Receive_Graph_Packet->Vent_Type_Min_O2)) >> 7;

	 OOM202.offset_minimum                 = ( 0x7F & (Receive_Graph_Packet->Vent_Type_Min_O2));
	 OOM202.offset_maximum                 = Receive_Graph_Packet->Max_O2;

	 if(PSV.VENT_TYPE == 0)
		 Vent_Type = ET_TUBE_MODE;
	else if(PSV.VENT_TYPE == 1)
		 Vent_Type = MASK_MODE;

	 if(PSV.TRIG_TYPE == 0)
		Trigger_Type = Pressure_Trigger;
	else if(PSV.TRIG_TYPE == 1)
		Trigger_Type = Flow_Trigger;
	 Mode_Initial_Flag_Set                        = OPEN;

	 vTaskResume(Mode_initial_Flag_Set_Handler);
}



void PSV_Task(void *argument)
{
	while(1)
	{
		if(PSV.APNEA_COUNTER > 0)
		{
			PSV_CONTROL();
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





static void INSPIRATION_PROCESS_PSV()
{
	 Breath_Count         = Count_The_Breath;
	 O2_Control_PSV++;
		if(O2_Control_PSV > 6)
			O2_Control_PSV=6;

	 if(Current_Backup_Mode == Backup_PCCMV_Mode)
	 {
		 PSV_Control._INSPIRATION_TIME                   = BACKUP_PC_CMV.INSPIRATION_TIME;
	 }

	if(Vent_Type == MASK_MODE)
	{
		Pressure_Mode_blower_control.BLOWER_DAC_VAL           = APNEA_DAC_VAL(PSV.PEEP_Val);
		PIP_control_Task.PIP_Control_Task_Delay               = 10;
		PIP_control_Task.Increase_Each_DAC_SET_CONST          = 30;
		PIP_Status                    				          = PIP_NOT_ACHEIVED;
		if(PSV.PSV_O2_Val > 21)
		{
			Parkar_valve_Signal(  (Oxygen_Parameter.PSV_O2_DAC+100)  );
		}
	}
	else if(Vent_Type == ET_TUBE_MODE)
	{
		Pressure_Mode_blower_control.BLOWER_DAC_VAL           = DAC_VAL(PSV.PEEP_Val);
		PIP_control_Task.PIP_Control_Task_Delay               = 10;
		PIP_control_Task.Increase_Each_DAC_SET_CONST          = 30;
		PIP_Status                    				          = PIP_NOT_ACHEIVED;

		if(PSV.PSV_O2_Val > 21)
		{

			Parkar_valve_Signal(Oxygen_Parameter.PSV_O2_DAC);
		}
	}

	Pressure_Mode_blower_control.LAST_BLOWER_DAC_VAL      = Pressure_Mode_blower_control.BLOWER_DAC_VAL;
	Set_PIP_Status                                        = SET_PIP_VALUE_NOT_ACHEIVED;
	Read_Time_vs_Pressure                                 = READ_TIME_OPEN;
	PIP_control_Task.PIP_Control_Event_Occured            = RESET;
	PIP_Average_Parameter.Maximum_PIP_Acheived            = RESET;
	Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = PSV_TUNE_PIP_BLOWER(&Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL,
                                                                                  PSV.PRESSURE_SUPPORT_Val );

	GRAPH_VALUES.volume                                   = RESET;
	Volume_max                                            = RESET;
	PSV_PEEP_AVERAGE();
	vTaskResume(PSV_PID_Handler);
	vTaskSuspend(PSV_Handler);
}


void PSV_PID_Task(void *argument)
{
	while(1)
	{
		if(Vent_Type == MASK_MODE)
		{
			MASK_MODE_PID();
		}
		else if(Vent_Type == ET_TUBE_MODE)
		{
			ET_TUBE_MODE_PID();
		}

		vTaskDelay(Ten_Millisecond_Delay);
	}
}


static void PSV_CONTROL()
{
	    Expiration_Cycle();
	    if(Wait_Time > 0 )
	    {
	    	WAIT_TIME_CONTROL();
	    }
	    else
	    {
	    	    Apnea_Mode  = ON;
	    	    Apnea_O2_control();
	    	    //Check_O2_Supply_Alert_in_Apnea(PSV.PSV_O2_Val,O2_Control_PSV);
				if(PSV_Control.Blower_Wait_Time_Milli_Second == 0)
				{
					RUN_PSV_PEEP_MAINTAIN_BLOWER();
				}

				if(PSV_Control.PSV_PEEP_Settle_Time == 0 )
				{
					Patient_Circuit_Disconnected_Alert_PSV();
					For_Trigger_Control();
				}
				else
				{
					CHECK_TRIGGER_OFFSET();
				}
	    }
}


static void CHECK_PRESSURE_TRIGGER()
{
	if (( GRAPH_VALUES.pressure < (Pressure_Trigger_Offset - PSV.TRIG_LMT )))
	{
		Patient_Trigger                             = PATIENT_TRIGGER_HAPPEN;
		PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL = 0;
		Blower_Signal( PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL);

		INSPIRATION_PROCESS_PSV();
	}
}

static void CHECK_FLOW_TRIGGER()
{
	if ((GRAPH_VALUES.flow > (Flow_Trigger_Offset+PSV.TRIG_LMT )))
	{
		LAST_FLOW_TRIGGER 							= FLOW_TRIGGER;
		Patient_Trigger  							= PATIENT_TRIGGER_HAPPEN;
		PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL = 0;
		Blower_Signal( PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL);

		INSPIRATION_PROCESS_PSV();

	}
}

static void CHECK_TRIGGER_OFFSET()
{

	if(Vent_Type == MASK_MODE)
	{
		if(GRAPH_VALUES.flow>=0)
		{
			FLOW_TRIGGER             = 0;
			Pressure_Trigger_Offset  =  GRAPH_VALUES.pressure;
			Flow_Trigger_Offset      =  GRAPH_VALUES.flow;
		}
	}
	else if(Vent_Type == ET_TUBE_MODE)
	{
		if(GRAPH_VALUES.flow==0 || (GRAPH_VALUES.flow>=(-8) && GRAPH_VALUES.flow<0))
		{
			FLOW_TRIGGER             = 0;
			Pressure_Trigger_Offset  =  GRAPH_VALUES.pressure;
			Flow_Trigger_Offset      =  GRAPH_VALUES.flow;
		}
	}


}


static void RUN_PSV_PEEP_MAINTAIN_BLOWER()
{
	ExpValve_CLOSE();
	Blower_Signal( PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL);
}


static void STOP_BLOWER_IF_FLOW_SENSE()
{
	if(Vent_Type == MASK_MODE)
	{
		if (Trigger_Type == Pressure_Trigger)
		{
			PSV_Control.Blower_Wait_Time_Milli_Second = 0;
		}
		else if (Trigger_Type == Flow_Trigger)
		{
			PSV_Control.Blower_Wait_Time_Milli_Second = 500;
		}
	}
	else if(Vent_Type == ET_TUBE_MODE)
	{
		Blower_Signal(0);
		if (Trigger_Type == Pressure_Trigger)
		{
			PSV_Control.Blower_Wait_Time_Milli_Second = 1000;
		}
		else if (Trigger_Type == Flow_Trigger)
		{
			PSV_Control.Blower_Wait_Time_Milli_Second = 500;
		}
	}
}



static void PSV_PEEP_AVERAGE()
{
	if((PSV_Peep_Average.PEEP_Cumulative_Val == 0) || (PSV_Peep_Average.PEEP_Event_Count == 0))
	{
		PSV_Peep_Average.PEEP_Cumulative_Val = 1;
		PSV_Peep_Average.PEEP_Event_Count    = 1;
	}
	PSV_Peep_Average.PEEP_AVG_VAL        = (float)(PSV_Peep_Average.PEEP_Cumulative_Val/ PSV_Peep_Average.PEEP_Event_Count);

	if( (PSV_Peep_Average.PEEP_AVG_VAL <=0)  || (PSV_Peep_Average.PEEP_AVG_VAL > 80) )
	{
		PSV_Peep_Average.PEEP_AVG_VAL = 0;
	}
	if(PSV_Peep_Average.PEEP_AVG_VAL == PSV.PEEP_Val)
	{

		if(Vent_Type == MASK_MODE)
		{
			Change_CPAP_DAC_Values_INVASIVE(PSV.PEEP_Val,PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL);
		}
		else if(Vent_Type == ET_TUBE_MODE)
		{
			Change_DAC_Values(PSV.PEEP_Val,PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL);
		}

	}
	else
	{
		if (Trigger_Type == Flow_Trigger)
	    {
		  PSV_Control.PSV_PEEP_Settle_Time           = 500;
	    }
	}
}



static void PSV_PIP_AVERAGE()
{
	if((PSV_Pip_average.PRESSURE_SUPPORT_Cumulative_Val == 0) || (PSV_Pip_average.PRESSURE_SUPPORT_Event_Count == 0))
	{
		PSV_Pip_average.PRESSURE_SUPPORT_Cumulative_Val = 1;
		PSV_Pip_average.PRESSURE_SUPPORT_Event_Count    = 1;
	}
	PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL        = (uint32_t)(PSV_Pip_average.PRESSURE_SUPPORT_Cumulative_Val/ PSV_Pip_average.PRESSURE_SUPPORT_Event_Count);
	PSV_Pip_average.PRESSURE_SUPPORT_Cumulative_Val = RESET;
	PSV_Pip_average.PRESSURE_SUPPORT_Event_Count    = RESET;
	if(PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL == PSV.PRESSURE_SUPPORT_Val)
	{
		if(Vent_Type == MASK_MODE)
		{
			Change_CPAP_DAC_Values_INVASIVE(PSV.PRESSURE_SUPPORT_Val,Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL);
		}
		else if(Vent_Type == ET_TUBE_MODE)
		{
			Change_DAC_Values(PSV.PRESSURE_SUPPORT_Val,Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL);
		}

	}
}


static uint16_t PSV_TUNE_PEEP_BLOWER(uint16_t *Last_Blower_DAC_Val,uint8_t Set_PEEP)
{
	if(  ( PSV_Peep_Average.PEEP_AVG_VAL)  > ( Set_PEEP ) && (PSV_Peep_Average.PEEP_AVG_VAL  < (Set_PEEP + 2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-10);
	}
	else if(  ( PSV_Peep_Average.PEEP_AVG_VAL)  >= ( Set_PEEP + 2 ) && (PSV_Peep_Average.PEEP_AVG_VAL  < (Set_PEEP + 4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-20);
	}
	else if(  ( PSV_Peep_Average.PEEP_AVG_VAL)  >= ( Set_PEEP + 4 )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-40);
	}
	else if( (PSV_Peep_Average.PEEP_AVG_VAL  < Set_PEEP) && (PSV_Peep_Average.PEEP_AVG_VAL  > (Set_PEEP -2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+10);
	}
	else if(  (PSV_Peep_Average.PEEP_AVG_VAL  <= (Set_PEEP-2) ) && (PSV_Peep_Average.PEEP_AVG_VAL  >  (Set_PEEP-4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+20);
	}
	else if(  (PSV_Peep_Average.PEEP_AVG_VAL  <= (Set_PEEP-4) )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+40);
	}
	else
	{
		*Last_Blower_DAC_Val = *Last_Blower_DAC_Val;
	}


	return *Last_Blower_DAC_Val;
}


static uint16_t PSV_TUNE_PIP_BLOWER(uint16_t *Last_Blower_DAC_Val,uint8_t Set_PIP)
{

	if(  ( PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL)  > ( Set_PIP ) && (PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL  < (Set_PIP + 2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-10);
	}
	else if(  ( PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL)  >= ( Set_PIP + 2 ) && (PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL  < (Set_PIP + 4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-20);
	}
	else if(  ( PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL)  >= ( Set_PIP + 4 )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val-40);
	}
	else if( (PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL  < Set_PIP) && (PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL  > (Set_PIP -2) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+10);
	}
	else if(  (PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL  <= (Set_PIP-2) ) && (PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL  >  (Set_PIP-4) ) )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+20);
	}
	else if(  (PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL  <= (Set_PIP-4) )  )
	{
		*Last_Blower_DAC_Val = (*Last_Blower_DAC_Val+40);
	}
	else
	{
		*Last_Blower_DAC_Val = *Last_Blower_DAC_Val;
	}
	return *Last_Blower_DAC_Val;
}



static void BACKUP_PC_CMV_START()
{
	if(Backup_PC_CMV_SET_VALUE == CHANGE_HAPPEN)
	{
		Mode                                                  = Backup_Pccmv;
		Peep_Status                   				          = PEEP_NOT_ACHEIVED;
		Expiratory_Valve_Lock_Delay   			              = OPENED;
		 Oxygen_Parameter.O2_DAC=1700;


		if(Vent_Type == MASK_MODE)
		{
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
		}
		else if(Vent_Type == ET_TUBE_MODE)
		{
			Maximum_Limit=6;
			Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = DAC_VAL(BACKUP_PC_CMV.PIP_Val);
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
		Backup_PC_CMV_SET_VALUE                               = NO_CHANGES;
	 }
	 else if(Backup_PC_CMV_SET_VALUE == NO_CHANGES)
	 {
		Mode                                                  = Backup_Pccmv;
		Peep_Status                   				          = PEEP_NOT_ACHEIVED;
		Expiratory_Valve_Lock_Delay   			              = OPENED;
		Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = OLD.LAST_BLOWER_ENDING_DAC_SIGNAL;
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
			Parkar_valve_Signal(0);
			vTaskSuspend(Oxygen_Blending_Handler);
		}
		vTaskSuspend(PSV_PID_Handler);
		vTaskSuspend(PSV_Handler);
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


static void Patient_Circuit_Disconnected_Alert_PSV()
{
	if(Vent_Type == MASK_MODE)
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

								PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL           = APNEA_DAC_VAL(PSV.PEEP_Val );
								PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL      = PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL;


							}
					}
				}
		}
	}
	else if(Vent_Type == ET_TUBE_MODE)
	{
		if( (GRAPH_VALUES.pressure <= (1))   && (GRAPH_VALUES.flow<=5 ))
		{
				if((DAC1->DHR12R1)>=400)
				{
					if(PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK == CHECK)
					{
						Alert_Count.Patient_Circuit_disconnected_Alert_Count++;
							if(Alert_Count.Patient_Circuit_disconnected_Alert_Count >= 2000)
							{
								Clear_All_Alert_Bits();
								Alert_Status = ALERT_HAPPEN;
								SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_PATIENT_CIRCUIT_DISCONNECTED);
								//PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = DO_NOT_CHECK;
								Send_Bluetooth_Data_Farme                = SEND_ALERT_FRAME;
								Apnea_Alert                              = APNEA_ALERT_HAPPEN;
								//PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = DO_NOT_CHECK;
								LED_Alert();

								Alert_Count.Patient_Circuit_disconnected_Alert_Count = 0;
								PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL           = DAC_VAL(PSV.PEEP_Val );
								PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL      = PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL;
							}
					}
				}
		}


		else if( (GRAPH_VALUES.pressure <= (1)) && (GRAPH_VALUES.flow>=50 ))
		{
				if((DAC1->DHR12R1)>=400)
				{
					if(PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK == CHECK)
					{
						Alert_Count.Patient_Circuit_disconnected_Alert_Count++;
							if(Alert_Count.Patient_Circuit_disconnected_Alert_Count >= 4)
							{
								Clear_All_Alert_Bits();
								Alert_Status = ALERT_HAPPEN;
								SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_PATIENT_CIRCUIT_DISCONNECTED);
								//PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = DO_NOT_CHECK;
								Send_Bluetooth_Data_Farme                = SEND_ALERT_FRAME;
								Apnea_Alert                              = APNEA_ALERT_HAPPEN;
								//PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = DO_NOT_CHECK;
								LED_Alert();

								Alert_Count.Patient_Circuit_disconnected_Alert_Count = 0;
								PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL           = DAC_VAL(PSV.PEEP_Val );
								PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL      = PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL;
							}
					}
				}
		}

		else if(  ( ((PSV_Peep_Average.PEEP_AVG_VAL < (PSV.PEEP_Val -2))) && ( (GRAPH_VALUES.flow==0 || GRAPH_VALUES.flow==-1 || GRAPH_VALUES.flow==-2))) || ( (GRAPH_VALUES.flow>= 50) && (GRAPH_VALUES.volume>=100)))
		{
				if((DAC1->DHR12R1)>=400)
				{
					if(PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK == CHECK)
					{
						Alert_Count.Patient_Circuit_disconnected_Alert_Count++;
							if(Alert_Count.Patient_Circuit_disconnected_Alert_Count >= 1000)
							{
								Clear_All_Alert_Bits();
								Alert_Status = ALERT_HAPPEN;
								SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_PATIENT_CIRCUIT_DISCONNECTED);
								PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = DO_NOT_CHECK;
								Send_Bluetooth_Data_Farme                = SEND_ALERT_FRAME;
								Apnea_Alert                              = APNEA_ALERT_HAPPEN;
								PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = DO_NOT_CHECK;
								LED_Alert();

								PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL           = DAC_VAL(PSV.PEEP_Val );
								PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL      = PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL;
							}
					}
				}
		}
	}
}


static void Apnea_O2_control()
{
	if(PSV.PSV_O2_Val > 21)
	{

		if(Vent_Type == MASK_MODE)
		{
			Oxygen_PEEP();
		}
		else if(Vent_Type == ET_TUBE_MODE)
		{
			if(onetime == 1)
			{
				Oxygen_Parameter.PSV_O2_DAC = 1750;
				Parkar_valve_Signal(Oxygen_Parameter.PSV_O2_DAC);
				onetime = 0;
			}
			else
			{
				Parkar_valve_Signal(0);
			}
		}
	}
}




static void WAIT_TIME_CONTROL()
{

	Blower_Signal(0);
	//Check_Peep_Status_timer = 1000;
	if(GRAPH_VALUES.pressure <= Psv_Set_Peep)
	{
		ExpValve_CLOSE();
		Wait_Time = 0;
	}
}


static void Expiration_Cycle()
{
	 BREATH_STATE    = EXPIRATION_CYCLE;
}

static void For_Trigger_Control()
{
	if(Vent_Type == MASK_MODE)
	{
		if( (PSV_Control.Trigger_Check_Wait_Time == 0) && ( GRAPH_VALUES.pressure <(Pressure_Trigger_Offset) ) )
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
			PSV_Peep_Average.PEEP_Cumulative_Val += GRAPH_VALUES.pressure;
			PSV_Peep_Average.PEEP_Event_Count++;
			if(PSV_Peep_Average.PEEP_Event_Count >1000)
			{
				Reset_Patient_Circuit_Diconnceted_Alert_Flags();
				PSV_PEEP_AVERAGE();
				PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL    = PSV_TUNE_PEEP_BLOWER(&PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL,
																							 PSV.PEEP_Val );
				if(PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL <= 500)
					PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL = 500;
				if(PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL >= 3500)
					PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL = 3500;
				PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL          = PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL;

				PSV_Peep_Average.PEEP_Cumulative_Val                 = RESET;
				PSV_Peep_Average.PEEP_Event_Count                    = RESET;
			}
		}

		if(PSV_Peep_Average.PEEP_Event_Count < 300)
		{

			if( GRAPH_VALUES.flow  <= (Flow_Trigger_Offset ) )
			{
				CHECK_TRIGGER_OFFSET();
			}
		}
	}
	else if(Vent_Type == ET_TUBE_MODE)
	{
		if( (PSV_Control.Trigger_Check_Wait_Time == 0) && ( GRAPH_VALUES.flow > 0) )
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
			PSV_Peep_Average.PEEP_Cumulative_Val += GRAPH_VALUES.pressure;
			PSV_Peep_Average.PEEP_Event_Count++;
			if(PSV_Peep_Average.PEEP_Event_Count >1000)
			{
				Reset_Patient_Circuit_Diconnceted_Alert_Flags();
				PSV_PEEP_AVERAGE();
				PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL    = PSV_TUNE_PEEP_BLOWER(&PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL,
																							 PSV.PEEP_Val );
				if(PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL <= 500)
					PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL = 500;
				if(PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL >= 3500)
					PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL = 3500;
				PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL          = PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL;

				PSV_Peep_Average.PEEP_Cumulative_Val                 = RESET;
				PSV_Peep_Average.PEEP_Event_Count                    = RESET;
			}
		}

		if(PSV_Peep_Average.PEEP_Event_Count < 200)
		{
			if( GRAPH_VALUES.flow  <= (Flow_Trigger_Offset) )
			{
				CHECK_TRIGGER_OFFSET();
			}
		}
	}
}



static void MASK_MODE_PID()
{
	if(PSV_Control._INSPIRATION_TIME > 0)
	{
		BREATH_STATE                                          = PATIENT_TRIGGER_INSPIRATION;
		PIP_Control(PSV.PRESSURE_SUPPORT_Val );
		Blower_Signal(Pressure_Mode_blower_control.BLOWER_DAC_VAL);
		if(PSV_Control._INSPIRATION_TIME < 100)
	    {
		   PSV_Pip_average.PRESSURE_SUPPORT_Cumulative_Val += GRAPH_VALUES.pressure;
		   PSV_Pip_average.PRESSURE_SUPPORT_Event_Count++;
	    }
		if(PSV.PSV_O2_Val > 21)
		{
			if(GRAPH_VALUES.pressure >= PSV.PRESSURE_SUPPORT_Val )
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
		if(GRAPH_VALUES.flow > PSV_Max_Flow)
		{
			PSV_Max_Flow = GRAPH_VALUES.flow;
		}
	}
	else if(PSV_Control._INSPIRATION_TIME == 0)
	{
		if(PSV.PSV_O2_Val > 21)
		{
			PSV_Max_Flow = 0;
			if(PSV.PSV_O2_Val == 21)
			{
				Parkar_valve_Signal(0);
				Blower_Signal(0);
			}
			Check_O2_Supply_Alert_in_Apnea_Once(PSV.PSV_O2_Val);
			GRAPH_VALUES.Maximum_oxygen_flow = 0;
			if(O2_Control_PSV >= 6)
			{
				Tune_O2_PSV(PSV.PSV_O2_Val,Oxygen_Parameter.Achieved_Oxygen);
				Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;
			}
		}
		Blower_Signal(0);
		ExpValve_OPEN();
		PSV.APNEA_COUNTER                                = (1000*PSV.APNEA_TIME) ;
		PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL      = APNEA_DAC_VAL(PSV.PEEP_Val );
		Patient_Trigger                                  = NO_PATIENT_TRIGGER;
	    BREATH_STATE                                     = PATIENT_TRIGGER_EXPIRATION;
		PSV_PIP_AVERAGE();
		PSV_Control.PSV_PEEP_Settle_Time                 = 100+PSV.TRIG_TIME;
		BIPAP_Peep_Average.EPAP_AVG_VAL                  = 0;
		ExpValve_OPEN();
		Wait_Time                                        = 10;
		Send_Frame                                       = First_Frame;
		Send_Bluetooth_Data_Farme                        = SEND_SAMPLED_PARAMETER_FRAME;
		Blower_Signal(0);
		vTaskResume(PSV_Handler);
		vTaskSuspend(PSV_PID_Handler);
	}
}

static void ET_TUBE_MODE_PID()
{
	if(PSV_Control._INSPIRATION_TIME > 0)
	{
		BREATH_STATE                                          = PATIENT_TRIGGER_INSPIRATION;
		PIP_Control(PSV.PRESSURE_SUPPORT_Val );
		Blower_Signal(Pressure_Mode_blower_control.BLOWER_DAC_VAL);

		if(PSV_Control._INSPIRATION_TIME < 100)
	    {
		   PSV_Pip_average.PRESSURE_SUPPORT_Cumulative_Val += GRAPH_VALUES.pressure;
		   PSV_Pip_average.PRESSURE_SUPPORT_Event_Count++;

	    }
		if(PSV.PSV_O2_Val > 21)
		{
			if(Oxygen_Blending_Status == OXYGEN_NOT_ACHIEVED)
			   Oxygen_PEEP();
			if(GRAPH_VALUES.pressure >= PSV.PRESSURE_SUPPORT_Val )
			{
				Parkar_valve_Signal(0);
				Oxygen_Blending_Status=OXYGEN_ACHIEVED;

			}

		}
		if(GRAPH_VALUES.flow > PSV_Max_Flow)
		{
			PSV_Max_Flow = GRAPH_VALUES.flow;
		}


	}
	else if(PSV_Control._INSPIRATION_TIME == 0)
	{
		if(PSV.PSV_O2_Val > 21)
		{
			Check_O2_Supply_Alert_in_Apnea_Once(PSV.PSV_O2_Val);
			GRAPH_VALUES.Maximum_oxygen_flow = 0;
			if(O2_Control_PSV >= 6)
			{
				Check_O2_Supply_Alert_in_Apnea_Once(PSV.PSV_O2_Val);
				GRAPH_VALUES.Maximum_oxygen_flow = 0;
				Tune_O2_PSV(PSV.PSV_O2_Val,Oxygen_Parameter.Achieved_Oxygen);
				Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;
			}
			PSV_Max_Flow = 0;
			Oxygen_Blending_Status = OXYGEN_NOT_ACHIEVED;
		}
		Parkar_valve_Signal(0);
		Blower_Signal(0);
		ExpValve_OPEN();
		PSV.APNEA_COUNTER                                = (1000*PSV.APNEA_TIME) ;
		PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL      = DAC_VAL(PSV.PEEP_Val );
		Patient_Trigger                                  = NO_PATIENT_TRIGGER;
	    BREATH_STATE                                     = PATIENT_TRIGGER_EXPIRATION;
		PSV_PIP_AVERAGE();
		PSV_Control.PSV_PEEP_Settle_Time                 = 100+PSV.TRIG_TIME;
		BIPAP_Peep_Average.EPAP_AVG_VAL                  = 0;
		ExpValve_OPEN();
		Wait_Time                                        = 700;
		//Wait_Time                                       = Valve_Control(Setteled_Pressure,PSV.PEEP_Val,Last_Wait_Time);
        //Last_Wait_Time                                  = Wait_Time;
		Send_Frame                                       = First_Frame;
		Send_Bluetooth_Data_Farme                        = SEND_SAMPLED_PARAMETER_FRAME;
		Blower_Signal(0);
		vTaskResume(PSV_Handler);
		vTaskSuspend(PSV_PID_Handler);
	}
}


void Oxygen_PEEP()
{

	if(O2_Control_PSV < 6)
	{
		O2_Flow_Rate   = ( GRAPH_VALUES.flow* ( (float)( PSV.PSV_O2_Val - 21) / 100) );
		if(O2_Flow_Rate <=8)
		{
			O2_Flow_Rate = 8;
		}
		if(GRAPH_VALUES.oxygen_flow <= O2_Flow_Rate)
		{
			Oxygen_Parameter.PSV_O2_DAC=controlsys_Update(&Parker_valve_tune, O2_Flow_Rate , GRAPH_VALUES.oxygen_flow);
			Parkar_valve_Signal(Oxygen_Parameter.PSV_O2_DAC);
		}
		else
		{
			Oxygen_Parameter.PSV_O2_DAC=controlsys_Update(&Parker_valve_tune, O2_Flow_Rate , GRAPH_VALUES.oxygen_flow);
			Parkar_valve_Signal(Oxygen_Parameter.PSV_O2_DAC);
		}
		Parkar_valve_Signal(Oxygen_Parameter.PSV_O2_DAC);
		Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;
	}


	Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;
}



void Low_Oxygen_Achieved_PSV(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{


		if(  ( Achieved_Oxygen)  < ( FIO2_Val) && (Achieved_Oxygen  >= (FIO2_Val - 2) ) )
		{
			Oxygen_Parameter.PSV_O2_DAC = (Oxygen_Parameter.PSV_O2_DAC+ET_O2_Envitec_Tune_Value1);
		}
		else if(  ( Achieved_Oxygen)  < ( FIO2_Val - 2 ) && (Achieved_Oxygen  > (FIO2_Val - 5) ) )
		{
			Oxygen_Parameter.PSV_O2_DAC = (Oxygen_Parameter.PSV_O2_DAC+ET_O2_Envitec_Tune_Value2);
		}
		else if(  ( Achieved_Oxygen)  <= ( FIO2_Val - 5 )  )
		{
			Oxygen_Parameter.PSV_O2_DAC = (Oxygen_Parameter.PSV_O2_DAC+ET_O2_Envitec_Tune_Value3);
		}

		if(Oxygen_Parameter.PSV_O2_DAC>=3000)
		{
			Oxygen_Parameter.PSV_O2_DAC=3000;
		}
		Parkar_valve_Signal(Oxygen_Parameter.PSV_O2_DAC);
}


/**
  * This function used to reduce the Proportional valve when high O2 Achieved.
  */


void High_Oxygen_Achieved_PSV(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{

		if(  ( Achieved_Oxygen)  > ( FIO2_Val ) && (Achieved_Oxygen  <= (FIO2_Val + 2) ) )
		{
			Oxygen_Parameter.PSV_O2_DAC = (Oxygen_Parameter.PSV_O2_DAC-ET_O2_Envitec_Tune_Value1);
		}
		else if(  ( Achieved_Oxygen)  > ( FIO2_Val + 2 ) && (Achieved_Oxygen  < (FIO2_Val + 5) ) )
		{
			Oxygen_Parameter.PSV_O2_DAC = (Oxygen_Parameter.PSV_O2_DAC-ET_O2_Envitec_Tune_Value2);
		}
		else if(  ( Achieved_Oxygen)  >= ( FIO2_Val + 5 )  )
		{
			Oxygen_Parameter.PSV_O2_DAC = (Oxygen_Parameter.PSV_O2_DAC-ET_O2_Envitec_Tune_Value3);
		}


		if(Oxygen_Parameter.PSV_O2_DAC<=1400)
		{
			Oxygen_Parameter.PSV_O2_DAC=1400;
		}
	    Parkar_valve_Signal(Oxygen_Parameter.PSV_O2_DAC);
}



/**
  * This function check Achieved O2 high or low
  */

void Tune_O2_PSV(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{
	   if (Achieved_Oxygen < (FIO2_Val-1))
		{
			Low_Oxygen_Achieved_PSV(FIO2_Val,Achieved_Oxygen);
		}
		else if (Achieved_Oxygen > (FIO2_Val))
		{
			High_Oxygen_Achieved_PSV(FIO2_Val,Achieved_Oxygen);
		}
		else
			Parkar_valve_Signal(Oxygen_Parameter.PSV_O2_DAC);
		Last_O2_Dac_Val = Oxygen_Parameter.PSV_O2_DAC;
		Parkar_valve_Signal(Oxygen_Parameter.PSV_O2_DAC);
}




/*static uint16_t Valve_Control(uint8_t Achieved_PEEP,uint8_t Set_PEEP,uint16_t Valve_Open_Time)
{
	static uint8_t Wait_Count_1,Wait_Count_2;
	if (Achieved_PEEP >  (Set_PEEP+8) )
	{
		Wait_Count_1++;
		if (Wait_Count_1 > 0)
		{
			Valve_Open_Time = (Valve_Open_Time + 100);
			Wait_Count_1    = RESET;
				if (Valve_Open_Time >= 700)
				{
					Valve_Open_Time = 700;
					Psv_Set_Peep = (Psv_Set_Peep - 1);
				}
		}
		if (Psv_Set_Peep <= 3)
			Psv_Set_Peep = 3;

	}
	else if( (Achieved_PEEP> (Set_PEEP+2))  && (Achieved_PEEP<= (Set_PEEP+8)) )
	{
		Wait_Count_1++;
		if (Wait_Count_1 > 0)
		{
			Valve_Open_Time = Valve_Open_Time + 40;
			Wait_Count_1    = RESET;
				if (Valve_Open_Time >= 700)
				{
					Valve_Open_Time                   = 700;
					Psv_Set_Peep = (Psv_Set_Peep - 1);
				}
		}
		if (Psv_Set_Peep <= 3)
			Psv_Set_Peep = 3;
	}
	else if( (Achieved_PEEP> Set_PEEP)  && (Achieved_PEEP <= (Set_PEEP+2)) )
	{
		Wait_Count_1++;
		if (Wait_Count_1 > 0)
		{
			Valve_Open_Time = Valve_Open_Time + 10;
			Wait_Count_1    = RESET;
				if (Valve_Open_Time >= 700)
				{
					Valve_Open_Time  = 700;
					Psv_Set_Peep = (Psv_Set_Peep - 1);
				}
		}
		if (Psv_Set_Peep <= 3)
			Psv_Set_Peep = 3;
	}
	else if ((Achieved_PEEP == 0 ) )
	{
		Wait_Count_2++;
		if (Wait_Count_2 > 0)
		{
			Psv_Set_Peep = (Psv_Set_Peep + 1);
			Valve_Open_Time                   = (Valve_Open_Time - 10);
			Wait_Count_2                      = RESET;
				if ( (Valve_Open_Time < 1) || (Valve_Open_Time > 700) )
				{
					Valve_Open_Time = 1;
				}
		}
		if (Psv_Set_Peep  > (Set_PEEP + 10) )
			Psv_Set_Peep  = Psv_Set_Peep ;

	}
	else if (Achieved_PEEP< (Set_PEEP -8))
	{
		Wait_Count_2++;
		if (Wait_Count_2 > 0)
		{
			Valve_Open_Time = ( Valve_Open_Time - 50);
			Wait_Count_2    = RESET;
				if ((Valve_Open_Time < 1) || (Valve_Open_Time > 700) )
				{
					Valve_Open_Time   = 1;
					Psv_Set_Peep = (Psv_Set_Peep + 1);
				}
		}
		if (Psv_Set_Peep> (Set_PEEP + 10) )
			Psv_Set_Peep =Psv_Set_Peep;
	}
	else if (( Achieved_PEEP >= (Set_PEEP -8)) && (Achieved_PEEP< (Set_PEEP-2)))
	{
		Wait_Count_2++;
		if (Wait_Count_2 > 0)
		{
			Valve_Open_Time = (Valve_Open_Time - 30);
			Wait_Count_2    = RESET;
				if (Valve_Open_Time< 1 || Valve_Open_Time > 700)
				{
					Valve_Open_Time                   = 1;
					Psv_Set_Peep = (Psv_Set_Peep + 1);
				}
		}
		if (Psv_Set_Peep > (Set_PEEP + 10) )
			Psv_Set_Peep = Psv_Set_Peep;

	}
	else if ((Achieved_PEEP>= (Set_PEEP -2)) && (Achieved_PEEP< (Set_PEEP)))
	{
		Wait_Count_2++;
		if (Wait_Count_2 > 0)
		{

			Valve_Open_Time = (Valve_Open_Time - 10);
			Wait_Count_2    = RESET;
				if ( (Valve_Open_Time < 1) || (Valve_Open_Time> 700) )
				{
					Psv_Set_Peep = (Psv_Set_Peep + 1);
					Valve_Open_Time  = 1;
				}
		}
		if (Psv_Set_Peep > (Set_PEEP + 10) )
			Psv_Set_Peep = Psv_Set_Peep;
	}
	else
		Psv_Set_Peep = Psv_Set_Peep;

	return Valve_Open_Time;
}*/
