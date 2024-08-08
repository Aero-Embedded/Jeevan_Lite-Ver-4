/**
 *******************************************************************************
 * @file Mode_Initial_Flag_Set.c
 * @author Sarath Kumar
 * @date 27/03/24
 * @brief Implementation of Initial setup before starting the mode.
 *
 * This file contains the implementation of Initial setup before starting the mode.
 *******************************************************************************
 * */


#include "Mode_Initial_Flag_Set.h"

void PSV_Mask_Mode_initial_flags_sets();
void PSV_ET_TUBE_Mode_initial_flags_sets();
uint16_t DEC_DAC=200;
extern uint8_t Valve_Condition;
extern uint16_t O2_max_Control;
uint8_t Dummy;
extern SENSOR MPX7002;
extern SENSOR MPX5010;

/**
 * \brief Mode_Initial_Flag_Set_Task.
 * This task determines the initialization of flags in the specified mode.
 * */
void Mode_Initial_Flag_Set_Task (void *argument)
{
	while(1)
	{
		One_Minite_Or_60000_Ms  = 60000;
		Breath                  = 0;
		Breath_Count            = Leave_Count_The_Breath;
		O2_Low_Count            = 0;
	    O2_High_Count           = 0;
		count1                  = 0;
		count2                  = 0;
		INITIAL_DAC_VALUE();
		INITIAL_CPAP_DAC_VALUE_INVASIVE();
		switch (Mode_Initial_Flag_Set)
		{
			case OPEN:
				switch(Mode)
				{
					case Pccmv:
						PC_CMV_Initial_Flags_SET();
					break;
					case Vccmv:
						VC_CMV_Initial_Flags_SET();
					break;
					case Pcsimv:
						PC_SIMV_Initial_Flags_SET();
					break;
					case Vcsimv:
						VC_SIMV_Initial_Flags_SET();
					break;
					case Psv:
						PSV_Initial_Flags_SET();
					break;
					case Cpap:
						CPAP_Initial_Flags_SET();
					break;
					case BiPap:
						BIBAP_Initial_Flags_SET();
					break;
					case Aprv:
						APRV_Initial_Flags_SET();
					break;
					default:
					break;
				}
			break;
			default:
			break;
		}



		switch (Back_Up_Mode_Initial_Flag_Set)
		{
			case OPEN:
				switch(Current_Backup_Mode)
				{
					case Backup_PCCMV_Mode:
						Backup_PC_CMV_Initial_Flags_SET();
					break;
					default:
					break;
				}
			break;
			default:
			break;
		}
	}
}

/**
 * \brief PC_CMV_Initial_Flags_SET.
 * This function resets flags for PCCMV mode and also starts PCCMV mode.
 * */
void PC_CMV_Initial_Flags_SET()
{
	controlsys_Init(&Parker_valve_tune);
	Avg_Flow                                              = 1;
	Flow_Count                                            = 0;
	Flow_Cummulative_Val                                  = 0;
	Check_Oxygen_Count                                    = 0;
	Temp_Dac                                              = 1400;
	inc_dac                                               = 2;
	Oxygen_Blending_Status                                = OXYGEN_NOT_ACHIEVED;

	Reset_Flow_Offset();
    OLD.last_vent_type                                    = ET_TUBE_MODE;
	Maximum_Limit                                         = 6;
	Apnea_Mode                                            = OFF;
	O2_DAC_AS_PER_FLOW_RATE_Count                         = 0;
	APRV_current_breathe                                  = APRV_IDLE;
	APRV_Next_Breathe                                     = APRV_Idle;
	Common_Mode_Parameter._PIP_Val                        = PC_CMV.PIP_Val;
	Common_Mode_Parameter._PEEP_Val                       = PC_CMV.PEEP_Val;
	Common_Mode_Parameter._RESPIRATORY_RATE_Val           = PC_CMV.RESPIRATORY_RATE_Val;
	Common_Mode_Parameter._FIO2_Val                       = PC_CMV.FIO2_Val;
	Peep_Status                   				          = PEEP_NOT_ACHEIVED;
	Expiratory_Valve_Lock_Delay   			              = OPENED;
	PEEP_Maintain_Parameter.PEEP_Temp                     = (PC_CMV.PEEP_Val / 2);
	PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time    = 0;
	PEEP_Maintain_Parameter.PEEP_Cumulative_Val           = 1;
	PEEP_Maintain_Parameter.PEEP_Event_Count              = 1;
    Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = DAC_VAL(PC_CMV.PIP_Val);




	PIP_control_Task.PIP_Control_Task_Delay           = 10;              //new
	PIP_control_Task.Increase_Each_DAC_SET_CONST      = 30;              //new



	PC_CMV.RISE_TIME_MS_Val     = (PC_CMV.Rise_Time *100);
	if(PC_CMV.RISE_TIME_MS_Val >= PC_CMV.INSPIRATION_TIME)            //new
	{
		PC_CMV.RISE_TIME_MS_Val =PC_CMV.INSPIRATION_TIME;
	}
	if(PC_CMV.RISE_TIME_MS_Val <=600)
	{
		PC_CMV.RISE_TIME_MS_Val =600;
	}
	PC_CMV.Rise_Time_percentage   = ((float)PC_CMV.RISE_TIME_MS_Val /PC_CMV.INSPIRATION_TIME) * 100.00;     //new
	PIP_control_Task.PIP_Acheived_Time_Ms      	 =  (PC_CMV.RISE_TIME_MS_Val * 2);



	Need_PID_Task_Delay                                   = NO;
	Tune_PID_Task_Delay                                   = T_OPEN;
	PIP_control_Task.Low_PIP_Count                        = RESET;
	PIP_control_Task.Last_Early_Acheived_Ramp_Time_Percentage  = RESET;
	Smooth_Ramp_Time_Val_Count                            = 0;
	Smooth_Ramp_Time_Val                                  = 15;
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
    Fio2_Blending_Method = PRESSURE_CONTROLLED;
    O2_Initial_Parameter(PC_CMV.FIO2_Val,Dummy);
	Alert_Flags_Reset();
	Next_Half_Breath_Cycle                                = Generate_Expiration_Cycle;
	Run_Current_Breathe_State                             = Run_Expiration_Cycle;
	Back_Up_Mode_Initial_Flag_Set                         = CLOSE;
	Mode_Initial_Flag_Set                                 = CLOSE;
	Reset_Patient_Circuit_Diconnceted_Alert_Flags();
	vTaskSuspend(Vc_Cmv_Handler);
	vTaskSuspend(Volume_Control_Task_Handler);
	vTaskSuspend(Pc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Pc_Simv_Assist_ON_Handler);
	vTaskSuspend(Vc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Vc_Simv_Assist_ON_Handler);
	vTaskSuspend(BACKUP_PC_Handler);

	vTaskSuspend(PSV_Handler);
	vTaskSuspend(PSV_PID_Handler);
	vTaskSuspend(CPAP_Handler);
	vTaskSuspend(BIPAP_Handler);
	vTaskSuspend(BIPAP_PID_Handler);
	vTaskSuspend(APRV_Handler);
	vTaskSuspend(APRV_ONE_TIME_Handler);
	vTaskResume(Alert_Handler);
	vTaskResume(One_Time_Handler);
	vTaskResume(Pc_Cmv_Handler);
	vTaskResume(PIP_Control_Task_Handler);
	vTaskSuspend(Mode_initial_Flag_Set_Handler);
}

/**
 * \brief VC_CMV_Initial_Flags_SET.
 * This function resets flags for VCCMV mode and also starts VCCMV mode.
 * */
void VC_CMV_Initial_Flags_SET()
{
	Stop_Increment_Count                                  = 0;
	Last_Flow_increment                                   = 0;
	controlsys_Init(&Parker_valve_tune);          //parker
	Flow_Count                                            = 0;
	Flow_Cummulative_Val                                  = 0;
	Check_Oxygen_Count                                    = 0;
	Temp_Dac                                              = 1400;
	inc_dac                                               = 1;
	Oxygen_Blending_Status                                = OXYGEN_NOT_ACHIEVED;
	Reset_Flow_Offset();
    OLD.last_vent_type                                    = ET_TUBE_MODE;
	Maximum_Limit                                         = 6;
	Apnea_Mode                                            = OFF;
	O2_DAC_AS_PER_FLOW_RATE_Count                         = 0;
	Reset_Blower_Status();
	APRV_current_breathe                                  = APRV_IDLE;
	APRV_Next_Breathe                                     = APRV_Idle;
	Total_Volume_Achieved_Time                            = 500;
	Common_Mode_Parameter._PEEP_Val                       = VC_CMV.PEEP_Val;
	Common_Mode_Parameter._RESPIRATORY_RATE_Val           = VC_CMV.RESPIRATORY_RATE_Val;
	Common_Mode_Parameter._FIO2_Val                       = VC_CMV.FIO2_Val;
	Common_Mode_Parameter._VOLUME_Val                     = VC_CMV.VOLUME_Val;
	Common_Mode_Parameter._FLOW_RATE                      = VC_CMV.FLOW_RATE_Val;
	Peep_Status                   				          = PEEP_NOT_ACHEIVED;
	Expiratory_Valve_Lock_Delay   			              = OPENED;
    PEEP_Maintain_Parameter.PEEP_Temp                     = 3;
    PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time    = 0;
    PEEP_Maintain_Parameter.PEEP_Cumulative_Val           = 1;
    PEEP_Maintain_Parameter.PEEP_Event_Count              = 1;


	LOW_COMPLIANCE_DAC                                         = LOW_Compliance_DAC_value(Common_Mode_Parameter._FLOW_RATE);
	Volume_Mode_Blower_control.PREVIOUS_INITIAL_BLOWER_DAC_VAL = LOW_COMPLIANCE_DAC;


    Fio2_Blending_Method = VOLUME_CONTROLLED;
    O2_Initial_Parameter(VC_CMV.FIO2_Val,Common_Mode_Parameter._FLOW_RATE);
    INCREMENT_DAC_VAL_PER_ITERATION           = 3;
	Volume_control_Task.Volume_Stop_Value     = 10;
	Alert_Flags_Reset();
	Next_Half_Breath_Cycle                    = Generate_Expiration_Cycle;
	Run_Current_Breathe_State                 = Run_Expiration_Cycle;
	Back_Up_Mode_Initial_Flag_Set             = CLOSE;
	Mode_Initial_Flag_Set                     = CLOSE;
	Reset_Patient_Circuit_Diconnceted_Alert_Flags();
	vTaskSuspend(Pc_Cmv_Handler);
	vTaskSuspend(PIP_Control_Task_Handler);
	vTaskSuspend(Pc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Pc_Simv_Assist_ON_Handler);
	vTaskSuspend(Vc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Vc_Simv_Assist_ON_Handler);
	vTaskSuspend(BACKUP_PC_Handler);

	vTaskSuspend(PSV_Handler);
	vTaskSuspend(PSV_PID_Handler);
	vTaskSuspend(CPAP_Handler);
	vTaskSuspend(BIPAP_Handler);
	vTaskSuspend(BIPAP_PID_Handler);
	vTaskSuspend(APRV_Handler);
	vTaskSuspend(APRV_ONE_TIME_Handler);
	vTaskResume(Alert_Handler);
	vTaskResume(One_Time_Handler);
	vTaskResume(Vc_Cmv_Handler);
	vTaskResume(Volume_Control_Task_Handler);
	vTaskSuspend(Mode_initial_Flag_Set_Handler);
}


/**
 * \brief PC_SIMV_Initial_Flags_SET.
 * This function resets flags for PC SIMV mode and also starts PC SIMV mode.
 * */
void PC_SIMV_Initial_Flags_SET()
{
	controlsys_Init(&Parker_valve_tune);          //parker
	Avg_Flow                                              = 1;
	Flow_Count                                            = 0;
	Flow_Cummulative_Val                                  = 0;
	Check_Oxygen_Count                                    = 0;        //new
	Temp_Dac                                              = 1400;     //new
	inc_dac                                               = 2;        //new
	Oxygen_Blending_Status                                = OXYGEN_NOT_ACHIEVED;  //new

	Reset_Flow_Offset();
    OLD.last_vent_type                                    = ET_TUBE_MODE;
	Maximum_Limit                                         = 6;
	Apnea_Mode                                            = OFF;
	O2_DAC_AS_PER_FLOW_RATE_Count                         = 0;
	TIME_Base_parameter._EXPIRATION_TIME                  = 0;
	APRV_current_breathe                                  = APRV_IDLE;
	APRV_Next_Breathe                                     = APRV_Idle;
	Common_Mode_Parameter._PIP_Val                        = PC_SIMV.PIP_Val;
	Common_Mode_Parameter._PEEP_Val                       = PC_SIMV.PEEP_Val;
	Common_Mode_Parameter._RESPIRATORY_RATE_Val           = PC_SIMV.RESPIRATORY_RATE_Val;
	Common_Mode_Parameter._FIO2_Val                       = PC_SIMV.FIO2_Val;
	Peep_Status                   				          = PEEP_NOT_ACHEIVED;
	Expiratory_Valve_Lock_Delay   			              = OPENED;
	PEEP_Maintain_Parameter.PEEP_Cumulative_Val           = 1;
	PEEP_Maintain_Parameter.PEEP_Event_Count              = 1;
	PEEP_Maintain_Parameter.PEEP_Temp                     = (PC_SIMV.PEEP_Val / 2);
	PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time    = 0;
	Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = DAC_VAL(PC_SIMV.PIP_Val);


	PIP_control_Task.PIP_Control_Task_Delay           = 10;              //new
	PIP_control_Task.Increase_Each_DAC_SET_CONST      = 30;              //new
	PC_SIMV.RISE_TIME_MS_Val     = (PC_SIMV.Rise_Time *100);
	if(PC_SIMV.RISE_TIME_MS_Val >= PC_SIMV.INSPIRATION_TIME)               //new
	{
		PC_SIMV.RISE_TIME_MS_Val =PC_SIMV.INSPIRATION_TIME;
	}
	if(PC_SIMV.RISE_TIME_MS_Val <=600)
	{
		PC_SIMV.RISE_TIME_MS_Val =600;
	}
	PC_SIMV.Rise_Time_percentage   = ((float)PC_SIMV.RISE_TIME_MS_Val /PC_SIMV.INSPIRATION_TIME) * 100.00;     //new
	PIP_control_Task.PIP_Acheived_Time_Ms      	 =  (PC_SIMV.RISE_TIME_MS_Val * 2);



	Need_PID_Task_Delay                                   = NO;
	Tune_PID_Task_Delay                                   = T_OPEN;
	PIP_control_Task.Low_PIP_Count                        = RESET;
	PIP_control_Task.Last_Early_Acheived_Ramp_Time_Percentage  = RESET;
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

	Fio2_Blending_Method  = PRESSURE_CONTROLLED;
	O2_Initial_Parameter(PC_SIMV.FIO2_Val,Dummy);
	Alert_Flags_Reset();
	Next_Half_Breath_Cycle                                = Generate_Expiration_Cycle;
	Run_Current_Breathe_State                             = Run_Expiration_Cycle;
	Back_Up_Mode_Initial_Flag_Set     = CLOSE;
	Mode_Initial_Flag_Set             = CLOSE;
	Reset_Patient_Circuit_Diconnceted_Alert_Flags();
	vTaskSuspend(Pc_Cmv_Handler);
	vTaskSuspend(PIP_Control_Task_Handler);
	vTaskSuspend(Vc_Cmv_Handler);
	vTaskSuspend(Volume_Control_Task_Handler);
	vTaskSuspend(Vc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Vc_Simv_Assist_ON_Handler);
	vTaskSuspend(BACKUP_PC_Handler);

	vTaskSuspend(PSV_Handler);
	vTaskSuspend(PSV_PID_Handler);
	vTaskSuspend(CPAP_Handler);
	vTaskSuspend(BIPAP_Handler);
	vTaskSuspend(BIPAP_PID_Handler);
	vTaskSuspend(APRV_Handler);
	vTaskSuspend(APRV_ONE_TIME_Handler);
	vTaskResume(Alert_Handler);
	vTaskResume(One_Time_Handler);
	if(Assist_Control == ASSIST_ON)
	{
		vTaskResume(Pc_Simv_Assist_ON_Handler);
		vTaskResume(PIP_Control_Task_Handler);
		vTaskSuspend(Pc_Simv_Assist_OFF_Handler);
	}
	else if(Assist_Control == ASSIST_OFF)
	{
		Pressure_Mode_blower_control.PEEP_BLOWER_DAC_VAL      = DAC_VAL(Common_Mode_Parameter._PEEP_Val);
		Pressure_Mode_blower_control.LAST_PEEP_BLOWER_DAC_VAL = Pressure_Mode_blower_control.PEEP_BLOWER_DAC_VAL;
		vTaskResume(Pc_Simv_Assist_OFF_Handler);
		vTaskResume(PIP_Control_Task_Handler);
		vTaskSuspend(Pc_Simv_Assist_ON_Handler);
		Trigger_Process = ONE_TIME_TRIGGER;
	}
	vTaskSuspend(Mode_initial_Flag_Set_Handler);
}

/**
 * \brief VC_SIMV_Initial_Flags_SET.
 * This function resets flags for VC SIMV mode and also starts VC SIMV mode.
 * */
void VC_SIMV_Initial_Flags_SET()
{
	Stop_Increment_Count                                  = 0;
	Last_Flow_increment                                   = 0;
	controlsys_Init(&Parker_valve_tune);          //parker
	Flow_Count                                            = 0;
	Flow_Cummulative_Val                                  = 0;
	Check_Oxygen_Count                                    = 0;
	Temp_Dac                                              = 1400;
	inc_dac                                               = 1;
	Oxygen_Blending_Status                                = OXYGEN_NOT_ACHIEVED;

    OLD.last_vent_type                                    = ET_TUBE_MODE;
	Reset_Flow_Offset();
	Maximum_Limit                                         = 6;
	Apnea_Mode                                            = OFF;
	O2_DAC_AS_PER_FLOW_RATE_Count                         = 0;
	Reset_Blower_Status();
	TIME_Base_parameter._EXPIRATION_TIME                  = 0;
	APRV_current_breathe                                  = APRV_IDLE;
	APRV_Next_Breathe                                     = APRV_Idle;
	Total_Volume_Achieved_Time                            = 500;
	Common_Mode_Parameter._PEEP_Val                       = VC_SIMV.PEEP_Val;
	Common_Mode_Parameter._RESPIRATORY_RATE_Val           = VC_SIMV.RESPIRATORY_RATE_Val;
	Common_Mode_Parameter._FIO2_Val                       = VC_SIMV.FIO2_Val;
	Common_Mode_Parameter._VOLUME_Val                     = VC_SIMV.VOLUME_Val;
	Common_Mode_Parameter._FLOW_RATE                      = VC_SIMV.FLOW_RATE_Val;
	Peep_Status                   				          = PEEP_NOT_ACHEIVED;
	Expiratory_Valve_Lock_Delay   			              = OPENED;
	PEEP_Maintain_Parameter.PEEP_Cumulative_Val           = 1;
	PEEP_Maintain_Parameter.PEEP_Event_Count              = 1;
	PEEP_Maintain_Parameter.PEEP_Temp                     = 3;
	PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time    = 0;

	LOW_COMPLIANCE_DAC                                         = LOW_Compliance_DAC_value(Common_Mode_Parameter._FLOW_RATE);
	Volume_Mode_Blower_control.PREVIOUS_INITIAL_BLOWER_DAC_VAL = LOW_COMPLIANCE_DAC;


	Volume_control_Task.Volume_Stop_Value                = 10;
	INCREMENT_DAC_VAL_PER_ITERATION                      = 3;
	Fio2_Blending_Method                                 = VOLUME_CONTROLLED;
	O2_Initial_Parameter(VC_SIMV.FIO2_Val,Common_Mode_Parameter._FLOW_RATE);
	Alert_Flags_Reset();
	Next_Half_Breath_Cycle                              = Generate_Expiration_Cycle;
	Run_Current_Breathe_State                           = Run_Expiration_Cycle;
	Back_Up_Mode_Initial_Flag_Set                       = CLOSE;
	Mode_Initial_Flag_Set                               = CLOSE;
	Reset_Patient_Circuit_Diconnceted_Alert_Flags();
	vTaskSuspend(Pc_Cmv_Handler);
	vTaskSuspend(PIP_Control_Task_Handler);
	vTaskSuspend(Pc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Pc_Simv_Assist_ON_Handler);
	vTaskSuspend(Vc_Cmv_Handler);
	vTaskSuspend(Volume_Control_Task_Handler);
	vTaskSuspend(BACKUP_PC_Handler);

	vTaskSuspend(PSV_Handler);
	vTaskSuspend(PSV_PID_Handler);
	vTaskSuspend(CPAP_Handler);
	vTaskSuspend(BIPAP_Handler);
	vTaskSuspend(BIPAP_PID_Handler);
	vTaskSuspend(APRV_Handler);
	vTaskSuspend(APRV_ONE_TIME_Handler);
	vTaskResume(Alert_Handler);
	vTaskResume(One_Time_Handler);
	if(Assist_Control == ASSIST_ON)
	{
		vTaskResume(Vc_Simv_Assist_ON_Handler);
		vTaskResume(Volume_Control_Task_Handler);
		vTaskSuspend(Vc_Simv_Assist_OFF_Handler);
	}
	else if(Assist_Control == ASSIST_OFF)
	{
		Volume_Mode_Blower_control.PEEP_BLOWER_DAC_VAL      = DAC_VAL(Common_Mode_Parameter._PEEP_Val);
		Volume_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL = Volume_Mode_Blower_control.PEEP_BLOWER_DAC_VAL;
		vTaskResume(Vc_Simv_Assist_OFF_Handler);
		vTaskResume(Volume_Control_Task_Handler);
		vTaskSuspend(Vc_Simv_Assist_ON_Handler);
		Trigger_Process = ONE_TIME_TRIGGER;
	}
	vTaskSuspend(Mode_initial_Flag_Set_Handler);
}


/**
 * \brief PSV_Initial_Flags_SET.
 * This function resets flags for PSV mode and also starts PSV mode.
 * */
void PSV_Initial_Flags_SET()
{
	controlsys_Init(&Parker_valve_tune);          //parker
	Reset_Flow_Offset();
	O2_Control_PSV=0;
	GRAPH_VALUES.Maximum_oxygen_flow       = RESET;             //new O2
	Check_O2_Supply_Alert_Count_in_Apnea   = RESET;             //new O2
	if(Vent_Type == MASK_MODE)
	{
		PSV_Mask_Mode_initial_flags_sets();
	}
	else if(Vent_Type == ET_TUBE_MODE)
	{
		PSV_ET_TUBE_Mode_initial_flags_sets();
	}
}


/**
 * \brief CPAP_Initial_Flags_SET.
 * This function resets flags for CPAP mode and also starts CPAP mode.
 * */
void CPAP_Initial_Flags_SET()
{
	controlsys_Init(&Parker_valve_tune);          //parker
	Apnea_Mode                             = ON;
	Reset_Flow_Offset();
	CPAP_Average.CPAP_O2_Event_Count       = RESET;
	Cpap_Flow_Cummulative_val              = RESET;
	Flow_Sensor_Count                      = RESET;

	GRAPH_VALUES.Maximum_oxygen_flow       = RESET;             //new O2
	Check_O2_Supply_Alert_Count_in_Apnea   = RESET;             //new O2

	ExpValve_OPEN();
	APRV_current_breathe                   = APRV_IDLE;
	APRV_Next_Breathe                      = APRV_Idle;
	CPAP_Control.CPAP_Settle_Time          = 3000;
	Common_Mode_Parameter.TOLERANCE_EWAVE  = CPAP.TOLERANCE_EWAVE;
	Common_Mode_Parameter.TRIG_WINDOW      = CPAP.TRIG_WINDOW;
	Common_Mode_Parameter.TRIG_LMT         = CPAP.TRIG_LMT;
	//Common_Mode_Parameter.TRIG_TIME        = CPAP.TRIG_TIME;
	Patient_Trigger_Count                  = 0;
	Adjust_O2_DAC_AS_PER_FLOW_RATE_Count   = 0;

	if(CPAP.CPAP_O2_Val > 21)
	{
			if( (CPAP.CPAP_O2_Val == 100) && (CPAP.CPAP_Val >=25) )
			{
				CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL           = APNEA_DAC_VAL(CPAP.CPAP_Val );
				CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL           = (CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL - 400);
				CPAP_Mode_Blower_control.LAST_CPAP_BLOWER_DAC_VAL      = CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL;
				CPAP_Average.CPAP_AVG_VAL                              = CPAP.CPAP_Val;
			}
			else
			{
				CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL           = APNEA_DAC_VAL(CPAP.CPAP_Val );
				CPAP_Mode_Blower_control.LAST_CPAP_BLOWER_DAC_VAL      = CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL;
				CPAP_Average.CPAP_AVG_VAL                              = CPAP.CPAP_Val;
				Oxygen_Parameter.CPAP_O2_DAC                           = 0;
				Parkar_valve_Signal(Oxygen_Parameter.CPAP_O2_DAC);
			}

	}
	else
	{
		CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL           = APNEA_DAC_VAL(CPAP.CPAP_Val );
		CPAP_Mode_Blower_control.LAST_CPAP_BLOWER_DAC_VAL      = CPAP_Mode_Blower_control.CPAP_BLOWER_DAC_VAL;
		CPAP_Average.CPAP_AVG_VAL                              = CPAP.CPAP_Val;
		Valve_Condition                                        = OPEN;
		Oxygen_Parameter.Achieved_Oxygen                       = 21;
	    Parkar_valve_Signal(0);
	}



	vTaskSuspend(Pc_Cmv_Handler);
	vTaskSuspend(PIP_Control_Task_Handler);
	vTaskSuspend(Vc_Cmv_Handler);
	vTaskSuspend(Volume_Control_Task_Handler);
	vTaskSuspend(Pc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Pc_Simv_Assist_ON_Handler);
	vTaskSuspend(Vc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Vc_Simv_Assist_ON_Handler);
	vTaskSuspend(Alert_Handler);
	vTaskSuspend(One_Time_Handler);
	vTaskSuspend(BACKUP_PC_Handler);

	vTaskSuspend(PSV_Handler);
	vTaskSuspend(PSV_PID_Handler);
	vTaskSuspend(BIPAP_Handler);
	vTaskSuspend(BIPAP_PID_Handler);
	vTaskSuspend(APRV_Handler);
	vTaskSuspend(APRV_ONE_TIME_Handler);

	Reset_Patient_Circuit_Diconnceted_Alert_Flags();
	Blower_Signal(0);

	vTaskResume(CPAP_Handler);
	Back_Up_Mode_Initial_Flag_Set   = CLOSE;
	Mode_Initial_Flag_Set           = CLOSE;
	vTaskSuspend(Mode_initial_Flag_Set_Handler);
}


/**
 * \brief BIBAP_Initial_Flags_SET.
 * This function resets flags for BIPAP mode and also starts BIPAP mode.
 * */
void BIBAP_Initial_Flags_SET()
{
	controlsys_Init(&Parker_valve_tune);          //parker
	BIPAP_Peep_Average.BIPAP_O2_Event_Count = 0;
	O2_Control_Bipap                        = 0;    //new
	GRAPH_VALUES.Maximum_oxygen_flow       = RESET;             //new O2
	Check_O2_Supply_Alert_Count_in_Apnea   = RESET;             //new O2
	Apnea_Mode                              = ON;
	Reset_Flow_Offset();
	ExpValve_OPEN();
	Wait_Time                              = 1000;
	APRV_current_breathe                   = APRV_IDLE;
	APRV_Next_Breathe                      = APRV_Idle;
	BIPAP_Control.BIPAP_EPAP_Settle_Time   = 2000;
	Common_Mode_Parameter.TOLERANCE_EWAVE  = BIPAP.TOLERANCE_EWAVE;
	Common_Mode_Parameter.TRIG_WINDOW      = BIPAP.TRIG_WINDOW;
	Common_Mode_Parameter.TRIG_LMT         = BIPAP.TRIG_LMT;
	//Common_Mode_Parameter.TRIG_TIME        = BIPAP.TRIG_TIME;
	BIPAP.EPAP_COUNTER                     = BIPAP.EPAP_TIME;
	BIPAP_Control.Trigger_Check_Wait_Time = 500+ BIPAP.TRIG_TIME ;

	if(BIPAP.BIPAP_O2_Val > 21)
	{
		onetime                             = 1;
	}
	else
	{
		Oxygen_Parameter.Achieved_Oxygen   = 21;
	    Parkar_valve_Signal(0);
	}
	vTaskSuspend(Pc_Cmv_Handler);
	vTaskSuspend(PIP_Control_Task_Handler);
	vTaskSuspend(Vc_Cmv_Handler);
	vTaskSuspend(Volume_Control_Task_Handler);
	vTaskSuspend(Pc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Pc_Simv_Assist_ON_Handler);
	vTaskSuspend(Vc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Vc_Simv_Assist_ON_Handler);
	vTaskSuspend(Alert_Handler);
	vTaskSuspend(One_Time_Handler);
	vTaskSuspend(BACKUP_PC_Handler);
	vTaskSuspend(PSV_Handler);
	vTaskSuspend(PSV_PID_Handler);
	vTaskSuspend(CPAP_Handler);
	vTaskSuspend(APRV_Handler);
	vTaskSuspend(APRV_ONE_TIME_Handler);
	Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL   = APNEA_DAC_VAL(BIPAP.IPAP_Val );
	BIPAP_Mode_Blower_control.EPAP_BLOWER_DAC_VAL           = APNEA_DAC_VAL(BIPAP.EPAP_Val );
	BIPAP_Mode_Blower_control.LAST_EPAP_BLOWER_DAC_VAL      = BIPAP_Mode_Blower_control.EPAP_BLOWER_DAC_VAL;
	BIPAP_Pip_average.IPAP_AVG_VAL                          = BIPAP.IPAP_Val;
	BIPAP_Peep_Average.EPAP_AVG_VAL                         = BIPAP.EPAP_Val;
	Reset_Patient_Circuit_Diconnceted_Alert_Flags();
	Blower_Signal(0);
	vTaskResume(BIPAP_Handler);
	Back_Up_Mode_Initial_Flag_Set   = CLOSE;
	Mode_Initial_Flag_Set           = CLOSE;
	vTaskSuspend(Mode_initial_Flag_Set_Handler);
}


/**
 * \brief APRV_Initial_Flags_SET.
 * This function resets flags for APRV mode and also starts APRV mode.
 * */
void APRV_Initial_Flags_SET()
{
	controlsys_Init(&Parker_valve_tune);          //parker
	Avg_Flow                                              = 1;
	Flow_Count                                            = 0;
	Flow_Cummulative_Val                                  = 0;
	Check_Oxygen_Count                                    = 0;        //new
	Temp_Dac                                              = 1400;     //new
	inc_dac                                               = 2;        //new
	Oxygen_Blending_Status                                = OXYGEN_NOT_ACHIEVED;  //new
	Reset_Flow_Offset();
	OLD.last_vent_type                                    = ET_TUBE_MODE;
	Maximum_Limit                                         = 6;
	Apnea_Mode                                            = OFF;
	O2_DAC_AS_PER_FLOW_RATE_Count                         = 0;
	Common_Mode_Parameter._PIP_Val                        = APRV.P_HIGH;
	Common_Mode_Parameter._PEEP_Val                       = APRV.P_LOW;
	Common_Mode_Parameter._FIO2_Val                       = APRV.FIO2_VAL;
	vTaskSuspend(Pc_Cmv_Handler);
	vTaskSuspend(PIP_Control_Task_Handler);
	vTaskSuspend(Vc_Cmv_Handler);
	vTaskSuspend(Volume_Control_Task_Handler);
	vTaskSuspend(Pc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Pc_Simv_Assist_ON_Handler);
	vTaskSuspend(Vc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Vc_Simv_Assist_ON_Handler);
	vTaskSuspend(Alert_Handler);
	vTaskSuspend(One_Time_Handler);
	vTaskSuspend(BACKUP_PC_Handler);
	vTaskSuspend(PSV_Handler);
	vTaskSuspend(PSV_PID_Handler);
	vTaskSuspend(CPAP_Handler);
	APRV_PIP_average.PIP_Cumulative_Val                 = 1;
	APRV_PIP_average.PIP_Event_Count                    = 1;
	APRV_PEEP_average.PEEP_Cumulative_Val               = 1;
	APRV_PEEP_average.PEEP_Event_Count                  = 1;
	APRV_Next_Breathe                                   = APRV_EXPIRATION;
	APRV_current_breathe                                = APRV_EXP;
	APRV.T_LOW_VAL_COUNTER                              = APRV.T_LOW_VAL;
	PEEP_Maintain_Parameter.PEEP_Temp                   = APRV.P_LOW;
	PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time  =1;
	APRV_mode_blower_control.BLOWER_DAC_VAL = DAC_VAL(APRV.P_HIGH );
	Alert_Flags_Reset();
	Fio2_Blending_Method  = PRESSURE_CONTROLLED;
	O2_Initial_Parameter(APRV.FIO2_VAL,Dummy);
	vTaskResume(APRV_ONE_TIME_Handler);
	vTaskResume(APRV_Handler);
	Back_Up_Mode_Initial_Flag_Set   = CLOSE;
	Mode_Initial_Flag_Set           = CLOSE;
	vTaskSuspend(Mode_initial_Flag_Set_Handler);
}

/**
 * \brief Backup_PC_CMV_Initial_Flags_SET.
 * This function resets flags for Backup PCCMV mode and also starts Backup PC CMV mode.
 * */
void Backup_PC_CMV_Initial_Flags_SET()
{
	controlsys_Init(&Parker_valve_tune);          //parker
	Common_Mode_Parameter._PIP_Val                        = BACKUP_PC_CMV.PIP_Val;
	Common_Mode_Parameter._PEEP_Val                       = BACKUP_PC_CMV.PEEP_Val;
	Common_Mode_Parameter._RESPIRATORY_RATE_Val           = BACKUP_PC_CMV.RESPIRATORY_RATE_Val;
	Common_Mode_Parameter._FIO2_Val                       = BACKUP_PC_CMV.FIO2_Val;


	if(Vent_Type == MASK_MODE)
	{
		Pressure_Mode_blower_control.PEEP_BLOWER_DAC_VAL      = APNEA_DAC_VAL(Common_Mode_Parameter._PEEP_Val);
	}
	else if(Vent_Type == ET_TUBE_MODE)
	{
		Pressure_Mode_blower_control.PEEP_BLOWER_DAC_VAL      = DAC_VAL(Common_Mode_Parameter._PEEP_Val);
	}


	Pressure_Mode_blower_control.LAST_PEEP_BLOWER_DAC_VAL = Pressure_Mode_blower_control.PEEP_BLOWER_DAC_VAL;
	Reset_PC_CMV_BACKUP_Old_Flags_Reset();
    if(  (OLD.Last_RR_Val_PC_CMV_BACKUP != BACKUP_PC_CMV.RESPIRATORY_RATE_Val) || (OLD.Last_PIP_Val_PC_CMV_BACKUP    != BACKUP_PC_CMV.PIP_Val)
    		                                                                   || (OLD.Last_Rise_Time_PC_CMV_BACKUP  != BACKUP_PC_CMV.Rise_Time )
																		       || (OLD.Last_I_Val_PC_CMV_BACKUP      != BACKUP_PC_CMV.Inspiration_Time_Ratio)
																		       || (OLD.Last_E_Val_PC_CMV_BACKUP      != BACKUP_PC_CMV.Expiration_Time_Ratio)
																			   || (OLD.Last_PEEP_Val_PC_CMV_BACKUP   != BACKUP_PC_CMV.PEEP_Val)
																			   || (OLD.Last_O2_Val_PC_CMV_BACKUP     != BACKUP_PC_CMV.FIO2_Val )
																			   || (OLD.last_vent_type                != Vent_Type) )

    {
    	//BACKUP_PC_CMV.RISE_TIME_MS_Val       = ((float)BACKUP_PC_CMV.Rise_Time/100.00)*(BACKUP_PC_CMV.INSPIRATION_TIME) ;
    	PIP_control_Task.PIP_Acheived_Time_Ms          = (BACKUP_PC_CMV.RISE_TIME_MS_Val * 2);
    	OLD.Last_RR_Val_PC_CMV_BACKUP                  = BACKUP_PC_CMV.RESPIRATORY_RATE_Val;
    	OLD.Last_PIP_Val_PC_CMV_BACKUP                 = BACKUP_PC_CMV.PIP_Val;
    	OLD.Last_Rise_Time_PC_CMV_BACKUP               = BACKUP_PC_CMV.Rise_Time;
    	OLD.Last_I_Val_PC_CMV_BACKUP                   = BACKUP_PC_CMV.Inspiration_Time_Ratio;
    	OLD.Last_E_Val_PC_CMV_BACKUP                   = BACKUP_PC_CMV.Expiration_Time_Ratio;
    	OLD.Last_PEEP_Val_PC_CMV_BACKUP                = BACKUP_PC_CMV.PEEP_Val;
    	OLD.Last_O2_Val_PC_CMV_BACKUP                  = BACKUP_PC_CMV.FIO2_Val;
    	OLD.last_vent_type                             = Vent_Type;

    	Avg_Flow                                              = 1;
    	Flow_Count                                            = 0;
    	Flow_Cummulative_Val                                  = 0;
    	Check_Oxygen_Count                                    = 0;        //new
    	Temp_Dac                                              = 1400;     //new
    	inc_dac                                               = 2;        //new
    	Oxygen_Blending_Status                                = OXYGEN_NOT_ACHIEVED;  //new

    	if(Vent_Type == MASK_MODE)
    	{
    		OLD.LAST_BLOWER_ENDING_DAC_SIGNAL              =  APNEA_DAC_VAL(BACKUP_PC_CMV.PIP_Val);
    	}
    	else if(Vent_Type == ET_TUBE_MODE)
    	{
    		OLD.LAST_BLOWER_ENDING_DAC_SIGNAL              =  DAC_VAL(BACKUP_PC_CMV.PIP_Val);
    	}



    	PIP_control_Task.PIP_Control_Task_Delay           = 10;              //new
    	PIP_control_Task.Increase_Each_DAC_SET_CONST      = 30;              //new
    	BACKUP_PC_CMV.RISE_TIME_MS_Val                    = (BACKUP_PC_CMV.Rise_Time *100);

    	if(BACKUP_PC_CMV.RISE_TIME_MS_Val >= BACKUP_PC_CMV.INSPIRATION_TIME)               //new
    	{
    		BACKUP_PC_CMV.RISE_TIME_MS_Val =BACKUP_PC_CMV.INSPIRATION_TIME;
    	}
    	if(BACKUP_PC_CMV.RISE_TIME_MS_Val <=600)
    	{
    		BACKUP_PC_CMV.RISE_TIME_MS_Val =600;
    	}
    	BACKUP_PC_CMV.Rise_Time_percentage   = ((float)BACKUP_PC_CMV.RISE_TIME_MS_Val /BACKUP_PC_CMV.INSPIRATION_TIME) * 100.00;     //new
    	PIP_control_Task.PIP_Acheived_Time_Ms      	 =  (BACKUP_PC_CMV.RISE_TIME_MS_Val * 2);


    	OLD.LAST_Need_PID_Task_Delay                   =  NO;
    	OLD.LAST_Tune_PID_Task_Delay                   =  T_OPEN;
    	OLD.LAST_Low_PIP_Count                         =  RESET;
    	OLD.LAST_Early_Acheived_Ramp_Time_Percentage   =  RESET;
    	OLD.Last_Ramp_Time_PC_CMV_BACKUP               =  BACKUP_PC_CMV.RISE_TIME_MS_Val;
		PIP_Average_Parameter.PIP_Cumulative_Val       = 1;
		PIP_Average_Parameter.PIP_Event_Count          = 1;
		PEEP_Maintain_Parameter.PEEP_Cumulative_Val    = 1;
		PEEP_Maintain_Parameter.PEEP_Event_Count       = 1;
	    PEEP_Maintain_Parameter.PEEP_AVG_VAL           = 1;
	    PEEP_Maintain_Parameter.PEEP_Temp              = (BACKUP_PC_CMV.PEEP_Val / 2);
	    PEEP_Maintain_Parameter.Expiratory_Valve_Open_Time  = 0;
		Smooth_Ramp_Time_Val_Count                     = 0;
		Smooth_Ramp_Time_Val                           = 15;
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
    	Fio2_Blending_Method                           = PRESSURE_CONTROLLED;
		Backup_PC_CMV_SET_VALUE = CHANGE_HAPPEN;

    }
    else
    {
    	OLD.LAST_BLOWER_ENDING_DAC_SIGNAL              =  OLD.LAST_BLOWER_ENDING_DAC_SIGNAL;
    	OLD.LAST_PIP_Control_Task_Delay                =  OLD.LAST_PIP_Control_Task_Delay;
    	OLD.LAST_Increase_Each_DAC_SET_CONST           =  OLD.LAST_Increase_Each_DAC_SET_CONST;
    	OLD.LAST_Need_PID_Task_Delay                   =  OLD.LAST_Need_PID_Task_Delay;
    	OLD.LAST_Tune_PID_Task_Delay                   =  OLD.LAST_Tune_PID_Task_Delay;
    	OLD.LAST_Low_PIP_Count                         =  OLD.LAST_Low_PIP_Count;
    	OLD.LAST_Early_Acheived_Ramp_Time_Percentage   =  OLD.LAST_Early_Acheived_Ramp_Time_Percentage;
    	OLD.Last_Ramp_Time_PC_CMV_BACKUP               =  OLD.Last_Ramp_Time_PC_CMV_BACKUP;
    	Backup_PC_CMV_SET_VALUE = NO_CHANGES;
    }
    Alert_Flags_Reset();
	vTaskSuspend(Pc_Cmv_Handler);
	vTaskSuspend(PIP_Control_Task_Handler);
	vTaskSuspend(Vc_Cmv_Handler);
	vTaskSuspend(Volume_Control_Task_Handler);
	vTaskSuspend(Pc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Pc_Simv_Assist_ON_Handler);
	vTaskSuspend(Vc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Vc_Simv_Assist_ON_Handler);
	vTaskSuspend(Alert_Handler);
	vTaskSuspend(One_Time_Handler);
	vTaskSuspend(BACKUP_PC_Handler);
	vTaskSuspend(Oxygen_Blending_Handler);

	Back_Up_Mode_Initial_Flag_Set   = CLOSE;
	vTaskSuspend(Mode_initial_Flag_Set_Handler);
}



/**
 * \brief Reset_PC_CMV_Old_Flags_Reset.
 * This function Reset old reset flags for PC CMV..
 * */

void Reset_PC_CMV_Old_Flags_Reset()
{
	OLD.Last_PEEP_Val_PC_CMV_BACKUP  = 0;
	OLD.Last_PIP_Val_PC_CMV_BACKUP   = 0;
	OLD.Last_Rise_Time_PC_CMV_BACKUP = 0;
	OLD.Last_O2_Val_PC_CMV_BACKUP    = 0;
	OLD.Last_RR_Val_PC_CMV_BACKUP    = 0;
	OLD.Last_I_Val_PC_CMV_BACKUP     = 0;
	OLD.Last_E_Val_PC_CMV_BACKUP     = 0;
	OLD.Last_PEEP_Val_VC_CMV_BACKUP  = 0;
	OLD.Last_FLOW_Val_VC_CMV_BACKUP  = 0;
	OLD.Last_O2_Val_VC_CMV_BACKUP    = 0;
	OLD.Last_RR_Val_VC_CMV_BACKUP    = 0;
	OLD.Last_I_Val_VC_CMV_BACKUP     = 0;
	OLD.Last_E_Val_VC_CMV_BACKUP     = 0;
}

/**
 * \brief Reset_VC_CMV_Old_Flags_Reset.
 * This function Reset old reset flags for VCCMV.
 * */

void Reset_VC_CMV_Old_Flags_Reset()
{
	OLD.Last_PEEP_Val_PC_CMV_BACKUP  = 0;
	OLD.Last_PIP_Val_PC_CMV_BACKUP   = 0;
	OLD.Last_Rise_Time_PC_CMV_BACKUP = 0;
	OLD.Last_O2_Val_PC_CMV_BACKUP    = 0;
	OLD.Last_RR_Val_PC_CMV_BACKUP    = 0;
	OLD.Last_I_Val_PC_CMV_BACKUP     = 0;
	OLD.Last_E_Val_PC_CMV_BACKUP     = 0;
	OLD.Last_PEEP_Val_VC_CMV_BACKUP  = 0;
	OLD.Last_FLOW_Val_VC_CMV_BACKUP  = 0;
	OLD.Last_O2_Val_VC_CMV_BACKUP    = 0;
	OLD.Last_RR_Val_VC_CMV_BACKUP    = 0;
	OLD.Last_I_Val_VC_CMV_BACKUP     = 0;
	OLD.Last_E_Val_VC_CMV_BACKUP     = 0;
}


/**
 * \brief Reset_PC_SIMV_Old_Flags_Reset.
 * This function resets old flags for PC SIMV.
 * */
void Reset_PC_SIMV_Old_Flags_Reset()
{
	OLD.Last_PEEP_Val_PC_CMV_BACKUP  = 0;
	OLD.Last_PIP_Val_PC_CMV_BACKUP   = 0;
	OLD.Last_Rise_Time_PC_CMV_BACKUP = 0;
	OLD.Last_O2_Val_PC_CMV_BACKUP    = 0;
	OLD.Last_RR_Val_PC_CMV_BACKUP    = 0;
	OLD.Last_I_Val_PC_CMV_BACKUP     = 0;
	OLD.Last_E_Val_PC_CMV_BACKUP     = 0;
	OLD.Last_PEEP_Val_VC_CMV_BACKUP  = 0;
	OLD.Last_FLOW_Val_VC_CMV_BACKUP  = 0;
	OLD.Last_O2_Val_VC_CMV_BACKUP    = 0;
	OLD.Last_RR_Val_VC_CMV_BACKUP    = 0;
	OLD.Last_I_Val_VC_CMV_BACKUP     = 0;
	OLD.Last_E_Val_VC_CMV_BACKUP     = 0;
}

/**
 * \brief Reset_PC_CMV_Old_Flags_Reset.
 * This function Reset old reset flags for VC SIMV..
 * */

void Reset_VC_SIMV_Old_Flags_Reset()
{
	OLD.Last_PEEP_Val_PC_CMV_BACKUP  = 0;
	OLD.Last_PIP_Val_PC_CMV_BACKUP   = 0;
	OLD.Last_Rise_Time_PC_CMV_BACKUP = 0;
	OLD.Last_O2_Val_PC_CMV_BACKUP    = 0;
	OLD.Last_RR_Val_PC_CMV_BACKUP    = 0;
	OLD.Last_I_Val_PC_CMV_BACKUP     = 0;
	OLD.Last_E_Val_PC_CMV_BACKUP     = 0;
	OLD.Last_PEEP_Val_VC_CMV_BACKUP  = 0;
	OLD.Last_FLOW_Val_VC_CMV_BACKUP  = 0;
	OLD.Last_O2_Val_VC_CMV_BACKUP    = 0;
	OLD.Last_RR_Val_VC_CMV_BACKUP    = 0;
	OLD.Last_I_Val_VC_CMV_BACKUP     = 0;
	OLD.Last_E_Val_VC_CMV_BACKUP     = 0;
	OLD.Last_Volume_Val_VC_CMV_BACKUP= 0;
}


void Reset_PC_CMV_BACKUP_Old_Flags_Reset()
{
	OLD.Last_PEEP_Val_VC_CMV_BACKUP  = 0;
	OLD.Last_FLOW_Val_VC_CMV_BACKUP  = 0;
	OLD.Last_O2_Val_VC_CMV_BACKUP    = 0;
	OLD.Last_RR_Val_VC_CMV_BACKUP    = 0;
	OLD.Last_I_Val_VC_CMV_BACKUP     = 0;
	OLD.Last_E_Val_VC_CMV_BACKUP     = 0;
	OLD.Last_Volume_Val_VC_CMV_BACKUP= 0;
}


/**
 * \brief Reset_Patient_Circuit_Diconnceted_Alert_Flags.
 * */
void Reset_Patient_Circuit_Diconnceted_Alert_Flags()
{
	PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK             = CHECK;
	Alert_Count.Patient_Circuit_disconnected_Alert_Count = 0;
	Alert_Status                                         = NO_ALERT;
	LED_Alert();
	CLEAR_ALERT_BIT(FIRST_FRAME_UN,_ALERT_PATIENT_CIRCUIT_DISCONNECTED);
	//Send_Frame                                           = First_Frame;
	//Send_Bluetooth_Data_Farme                            = SEND_SAMPLED_PARAMETER_FRAME;
}


/**
 * \brief PSV_Initial_Flags_SET.
 * This function resets flags for PSV MASK mode and also starts PSV MASK mode.
 * */
void PSV_Mask_Mode_initial_flags_sets()
{
	Apnea_Mode                            = ON;
	PSV_Flow_Cummulative_val              = RESET;
	PSV_Flow_Sensor_Count                 = RESET;

	ExpValve_OPEN();
	Wait_Time                              = 1000;
	APRV_current_breathe                   = APRV_IDLE;
	APRV_Next_Breathe                      = APRV_Idle;
	PSV_Control.PSV_PEEP_Settle_Time       = 2000;
	Common_Mode_Parameter.TOLERANCE_EWAVE  = PSV.TOLERANCE_EWAVE;
	Common_Mode_Parameter.TRIG_WINDOW      = PSV.TRIG_WINDOW;
	Common_Mode_Parameter.TRIG_LMT         = PSV.TRIG_LMT;
	//Common_Mode_Parameter.TRIG_TIME        = PSV.TRIG_TIME;


	if(PSV.PSV_O2_Val > 21)
	{
		Adjust_O2_DAC_AS_PER_FLOW_RATE_Count   = 0;
		onetime                                = 1;
		Oxygen_Parameter.PSV_O2_DAC            = 1700;
		Parkar_valve_Signal(Oxygen_Parameter.PSV_O2_DAC);
	}
	else
	{
	    Parkar_valve_Signal(0);
	}

	vTaskSuspend(Pc_Cmv_Handler);
	vTaskSuspend(PIP_Control_Task_Handler);
	vTaskSuspend(Vc_Cmv_Handler);
	vTaskSuspend(Volume_Control_Task_Handler);
	vTaskSuspend(Pc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Pc_Simv_Assist_ON_Handler);
	vTaskSuspend(Vc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Vc_Simv_Assist_ON_Handler);
	vTaskSuspend(Alert_Handler);
	vTaskSuspend(One_Time_Handler);
	vTaskSuspend(BACKUP_PC_Handler);

	vTaskSuspend(CPAP_Handler);
	vTaskSuspend(BIPAP_Handler);
	vTaskSuspend(BIPAP_PID_Handler);
	vTaskSuspend(APRV_Handler);
	vTaskSuspend(APRV_ONE_TIME_Handler);
	Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = APNEA_DAC_VAL(PSV.PRESSURE_SUPPORT_Val );
	PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL           = APNEA_DAC_VAL(PSV.PEEP_Val );
	PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL      = (PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL);
	PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL              = PSV.PRESSURE_SUPPORT_Val;
	PSV_Peep_Average.PEEP_AVG_VAL                         = PSV.PEEP_Val;
	//Oxygen_Parameter.Achieved_Oxygen                      = (1 <<7) | Oxygen_Parameter.Achieved_Oxygen;
	Reset_Patient_Circuit_Diconnceted_Alert_Flags();
	Blower_Signal(0);
	vTaskResume(PSV_Handler);
	Back_Up_Mode_Initial_Flag_Set   = CLOSE;
	Mode_Initial_Flag_Set           = CLOSE;
	vTaskSuspend(Mode_initial_Flag_Set_Handler);
}


/**
 * \brief PSV_Initial_Flags_SET.
 * This function resets flags for PSV ET TUBE mode and also starts PSV ET TUBE mode.
 * */
void PSV_ET_TUBE_Mode_initial_flags_sets()
{
	Oxygen_Blending_Status = OXYGEN_NOT_ACHIEVED;
	Apnea_Mode                            = ON;
	PSV_Flow_Cummulative_val              = RESET;
	PSV_Flow_Sensor_Count                 = RESET;

	ExpValve_OPEN();
	Wait_Time                              = 500;
	Last_Wait_Time                         = Wait_Time;
	APRV_current_breathe                   = APRV_IDLE;
	APRV_Next_Breathe                      = APRV_Idle;
	PSV_Control.PSV_PEEP_Settle_Time       = 2000;
	Common_Mode_Parameter.TOLERANCE_EWAVE  = PSV.TOLERANCE_EWAVE;
	Common_Mode_Parameter.TRIG_WINDOW      = PSV.TRIG_WINDOW;
	Common_Mode_Parameter.TRIG_LMT         = PSV.TRIG_LMT;
	//Common_Mode_Parameter.TRIG_TIME        = PSV.TRIG_TIME;


	if(PSV.PSV_O2_Val > 21)
	{
		onetime                                = 1;
		Avg_Flow                               = 1;
		Flow_Count                             = 0;
		Flow_Cummulative_Val                   = 0;
		Check_Oxygen_Count                     = 0;        //new
		Temp_Dac                               = 1400;     //new
		inc_dac                                = 2;        //new

	}
	else
	{
		Oxygen_Parameter.Achieved_Oxygen   = 21;
	    Parkar_valve_Signal(0);
	}

	vTaskSuspend(Pc_Cmv_Handler);
	vTaskSuspend(PIP_Control_Task_Handler);
	vTaskSuspend(Vc_Cmv_Handler);
	vTaskSuspend(Volume_Control_Task_Handler);
	vTaskSuspend(Pc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Pc_Simv_Assist_ON_Handler);
	vTaskSuspend(Vc_Simv_Assist_OFF_Handler);
	vTaskSuspend(Vc_Simv_Assist_ON_Handler);
	vTaskSuspend(Alert_Handler);
	vTaskSuspend(One_Time_Handler);
	vTaskSuspend(BACKUP_PC_Handler);
	vTaskSuspend(CPAP_Handler);
	vTaskSuspend(BIPAP_Handler);
	vTaskSuspend(BIPAP_PID_Handler);
	vTaskSuspend(APRV_Handler);
	vTaskSuspend(APRV_ONE_TIME_Handler);
	Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = ( DAC_VAL(PSV.PRESSURE_SUPPORT_Val ));
	PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL           = ( DAC_VAL(PSV.PEEP_Val ) );
	PSV_Mode_Blower_control.LAST_PEEP_BLOWER_DAC_VAL      = (PSV_Mode_Blower_control.PEEP_BLOWER_DAC_VAL);
	PSV_Pip_average.PRESSURE_SUPPORT_AVG_VAL              = PSV.PRESSURE_SUPPORT_Val;
	PSV_Peep_Average.PEEP_AVG_VAL                         = PSV.PEEP_Val;
	Reset_Patient_Circuit_Diconnceted_Alert_Flags();
	Blower_Signal(0);
	vTaskResume(PSV_Handler);
	Back_Up_Mode_Initial_Flag_Set   = CLOSE;
	Mode_Initial_Flag_Set           = CLOSE;
	vTaskSuspend(Mode_initial_Flag_Set_Handler);
}


/**
 * \brief PSV_Initial_Flags_SET.
 * This function get offset values of sensor.
 * */
void Reset_Flow_Offset()
{
	if(Vent_Type == MASK_MODE)
	{
		if(OLD.last_vent_type == ET_TUBE_MODE)
		{
			MPX7002.offset                = MPX7002.current_reading;
			//MPX5010.offset                = MPX5010.current_reading;
			Backup_PC_CMV_SET_VALUE       = CHANGE_HAPPEN;
		}
	}
	else if(Vent_Type == ET_TUBE_MODE)
	{
		if(OLD.last_vent_type == MASK_MODE)
		{
			MPX7002.offset                = MPX7002.current_reading;
			//MPX5010.offset                = MPX5010.current_reading;
			Backup_PC_CMV_SET_VALUE       = CHANGE_HAPPEN;
		}
	}
	else
	{
		MPX7002.offset           = MPX7002.current_reading;
	}

}
