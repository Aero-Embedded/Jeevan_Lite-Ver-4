/**
 *******************************************************************************
 * \file Volume_control.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief implementation of inspiration time Volume control mode.
 *
 * This file contains the implementation inspiration time Volume control mode.
 *******************************************************************************
 **/


#include "Volume_control.h"




uint16_t Blower_Cut_Off_Time=300;
int8_t Cut_Off_flow_Rate_Level=20;
int8_t Cut_Off_flow_Rate_Val,Cut_Off_flow_Rate_Val_high;
int8_t Cut_Off_flow_Rate_Val_high_limit = 15;
uint16_t Time_Limit=400;
uint8_t Stop_Value = 8;
uint16_t Flow_Detect_Time=800;


/**
 * @brief Volume_Control_Task.
 * This Task control the inspiration and expiration blower dac control
 * */

void Volume_Control_Task (void *argument)
{
	while(1)
	{
		if(Run_Current_Breathe_State == Run_Inspiration_Cycle)
		{
				if(GRAPH_VALUES.pressure >= 60)
				{
					Volume_Mode_Blower_control.BLOWER_DAC_VAL=0;
					Blower_Status =BlOWER_OFF;
				}
				else
				{
					if(Blower_Status == BlOWER_ON)
					{
						  if(Stop_Increment_Count <= Stop_Value)
						  {
							  Volume_Control(Common_Mode_Parameter._VOLUME_Val,Common_Mode_Parameter._FLOW_RATE,Volume_control_Task.Volume_Stop_Value);
						  }
						  else
						  {
							  Flow_Restriction_Control(Common_Mode_Parameter._VOLUME_Val,Common_Mode_Parameter._FLOW_RATE,Volume_control_Task.Volume_Stop_Value);
						  }
					   }
				}
		}
		vTaskDelay(10);
	}
}


/**
 * @brief Volume_Control_Task.
 * This function control flow upto to set tidal volume to be achieved.
 * */


void Volume_Control(uint16_t Set_Volume_Val,uint8_t Set_Flow_Rate,uint8_t Volume_Stop_Value)
{
	if(Run_Current_Breathe_State == Run_Inspiration_Cycle)
	{
		Set_Volume_Val= (Set_Volume_Val-Volume_Stop_Value);
			if(GRAPH_VALUES.volume  <= Set_Volume_Val)
			{
				if(Volume_Array_count <(VOL_ARR_SIZE-1))
				{
					Volume_Achieved_Time++;
					Volume_Array[Volume_Array_count]=GRAPH_VALUES.flow;
					Volume_Array_count++;
					if(Volume_Array_count>=(VOL_ARR_SIZE - 1))
						Volume_Array_count=(VOL_ARR_SIZE - 1);
				}


						 if(Volume_Achieved_Status == VOLUME_NOT_ACHIEVED)
						 {
							 Volume_Mode_Blower_control.BLOWER_DAC_VAL          =   Volume_Mode_Blower_control.INITIAL_BLOWER_DAC_VAL+INCREMENT_DAC_VAL_PER_ITERATION;
							 Volume_Mode_Blower_control.INITIAL_BLOWER_DAC_VAL  =   Volume_Mode_Blower_control.BLOWER_DAC_VAL;

									if(Volume_Mode_Blower_control.BLOWER_DAC_VAL >= 1800)   //new
									{
										Blower_Status =BlOWER_OFF;
										Volume_Mode_Blower_control.BLOWER_DAC_VAL = 0;
									}

									if(TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED > Blower_Cut_Off_Time)  //new
									{
										if(GRAPH_VALUES.flow <=5)
										{
											Blower_Status =BlOWER_OFF;
											Volume_Mode_Blower_control.BLOWER_DAC_VAL = 0;
											if(Alert_Check < 4)
											{
												Oxygen_Blending_Status=OXYGEN_ACHIEVED;
												Parkar_valve_Signal(0);
											}
										}
									}

									if(TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED > 100)  //new
									{
										Flow_Cummulative_Val  += GRAPH_VALUES.flow;
										Flow_Count++;
									}

									if(TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED < Flow_Detect_Time)   //new
									{
										if(GRAPH_VALUES.flow >= Set_Flow_Rate)
										{
											  if(Maximum_Flow_Achieved  == FLOW_NOT_ACHIEVED)
											  {
													Maximum_Flow_Achieved                     = FLOW_ACHIEVED;
													//TIME_Base_parameter.FLOW_ACHIEVED_TIMER   = Time_Limit;
											  }
									    }
									}

									if(Alert_Check < 4)
									{
										Cut_Off_Region(Common_Mode_Parameter._FLOW_RATE);
									}


						 }
			}
			else
			{
			    Total_Volume_Achieved_Time                        = (Volume_Achieved_Time *10);
				Volume_Acheieved();
				Volume_Achieved_Status                            = VOLUME_ACHIEVED;
				Volume_Mode_Blower_control.BLOWER_DAC_VAL         = RESET;
				Volume_Mode_Blower_control.INITIAL_BLOWER_DAC_VAL = Volume_Mode_Blower_control.PREVIOUS_INITIAL_BLOWER_DAC_VAL;

			}

		}
}


/**
 * @brief VOLUME_STOP_POINT.
 * This function control stop the blower before volume reach.
 * */

void VOLUME_STOP_POINT(uint16_t Set_Volume,uint16_t Achieved_Volume)
{
	static uint8_t count;
	static uint8_t count_2;
       if(Achieved_Volume > (Set_Volume + 5))
	   {
    	   count++;
			if (count >= 3)
			{
				Volume_control_Task.Volume_Stop_Value = Volume_control_Task.Volume_Stop_Value + 5;
				count   = 0;
				count_2 = 0;

				if(Volume_control_Task.Volume_Stop_Value >= 100)
					Volume_control_Task.Volume_Stop_Value = 100;
			}
		}
       else if(Achieved_Volume < (Set_Volume - 10))
	   {
    	   count_2++;
			if (count_2 >= 3)
			{
				Volume_control_Task.Volume_Stop_Value = Volume_control_Task.Volume_Stop_Value - 5;
				count_2 = 0;
				count   = 0;
				if(Volume_control_Task.Volume_Stop_Value <= 5)
									Volume_control_Task.Volume_Stop_Value = 5;
			}
		}
       else
       {
			count_2 = 0;
			count   = 0;
       }
}


/**
 * @brief TUNE_INITIAL_DAC_VAL.
 * function selects the initial DAC value based on the last breath achieved flow rate.
 * */


uint16_t TUNE_INITIAL_DAC_VAL(uint16_t Previous_Dac_Val, uint8_t Flow_Rate,uint8_t Flow_Max)
{
	uint16_t DAC_VALUE=Previous_Dac_Val;
	    if (Flow_Max > (Flow_Rate +5))           //new
		{
			if (Flow_Max > (Flow_Rate + 10))
			{
				DAC_VALUE = Previous_Dac_Val - 20;
			}
			else if ((Flow_Max <= (Flow_Rate + 10))&& (Flow_Max > (Flow_Rate + 8)))
			{
				DAC_VALUE = Previous_Dac_Val - 4;
			}
			else if ((Flow_Max <= (Flow_Rate + 8)) && (Flow_Max > (Flow_Rate + 5)))
			{
				DAC_VALUE = Previous_Dac_Val - 2;
			}
			if (DAC_VALUE < 500)
			{
				DAC_VALUE = 500;
			}
		}
		else if ( (Flow_Max < (Flow_Rate) ))                                         //new
		{
			if ( (Volume_control_Task.LOW_Flow_Max_Achieved < (Flow_Rate) ) && (Volume_control_Task.LOW_Flow_Max_Achieved > (Flow_Rate- 3) ))          //new
			{
				DAC_VALUE = Previous_Dac_Val + 10;
			}
			else if ((Volume_control_Task.LOW_Flow_Max_Achieved <= (Flow_Rate-3) ) && (Volume_control_Task.LOW_Flow_Max_Achieved > (Flow_Rate-8) ))                                   //new
			{
				DAC_VALUE = Previous_Dac_Val + 20;
			}
			else if ((Volume_control_Task.LOW_Flow_Max_Achieved <= (Flow_Rate-8) )  )                                   //new
			{
				DAC_VALUE = Previous_Dac_Val + 30;
			}
			else
			{
				DAC_VALUE = Previous_Dac_Val + 30;
			}

			    if (DAC_VALUE >= 1600)
				{
					DAC_VALUE = 1600;
				}
		}
		else
		{
			DAC_VALUE     = Previous_Dac_Val;
		}
	    return DAC_VALUE;
}


/**
 * @brief LOW_Compliance_DAC_value.
 * This function selects the initial DAC value based on the flow rate when the mode start condition is met.
 * */


uint16_t LOW_Compliance_DAC_value(uint8_t Flow_Rate)
{
	uint16_t Temp_DAC;

	if(Flow_Rate >= 60)
		Temp_DAC = 900;
	else if( (Flow_Rate < 60) && (Flow_Rate >= 50) )         //new
		Temp_DAC = 850;
	else if( (Flow_Rate < 50) && (Flow_Rate >= 40) )
		Temp_DAC = 800;
	else if( (Flow_Rate < 40) && (Flow_Rate >= 30) )
		Temp_DAC = 750;
	else if( (Flow_Rate < 30) && (Flow_Rate >= 20) )
		Temp_DAC = 650;
	else if( (Flow_Rate < 20) && (Flow_Rate >= 10) )
		Temp_DAC = 650;
	else if( (Flow_Rate < 10) && (Flow_Rate > 0) )
		Temp_DAC = 650;

	return Temp_DAC;
}


/**
 * @brief Cut_Off_Region.
 * This function checks if any abnormal flow occurs during the inspiration time and stops the blower if detected.
 * */

void Cut_Off_Region(uint8_t Set_Flow_Rate)
{
	  if( (Maximum_Flow_Achieved  == FLOW_ACHIEVED) )
	  {
				  Cut_Off_flow_Rate_Val=Set_Flow_Rate - Cut_Off_flow_Rate_Level;
				  if(Cut_Off_flow_Rate_Val<5)
				  {
					  Cut_Off_flow_Rate_Val =5;
				  }
				  if( (GRAPH_VALUES.flow < (Cut_Off_flow_Rate_Val)) )
				  {
					  Blower_Status                             = BlOWER_OFF;
					  Volume_Mode_Blower_control.BLOWER_DAC_VAL = 0;
					  if(Stop_Increment_Count > Stop_Value)
					  {
								Flow_increment    = Flow_fine;
					  }
						Oxygen_Blending_Status=OXYGEN_ACHIEVED;
						Parkar_valve_Signal(0);
				 }


			      Cut_Off_flow_Rate_Val_high=Set_Flow_Rate + Cut_Off_flow_Rate_Val_high_limit;

				  if( (GRAPH_VALUES.flow > (Cut_Off_flow_Rate_Val_high)) )
				  {
						if(Patient_Trigger  == NO_PATIENT_TRIGGER)
						{
						  Blower_Status =BlOWER_OFF;
						  Volume_Mode_Blower_control.BLOWER_DAC_VAL = 0;
						  Oxygen_Blending_Status=OXYGEN_ACHIEVED;
						  Parkar_valve_Signal(0);
						}
				  }
	  }
}

/**
 * @brief Flow_Restriction_Control.
 * This function didnt have above function.
 * */

void Flow_Restriction_Control(uint16_t Set_Volume_Val,uint8_t Set_Flow_Rate,uint8_t Volume_Stop_Value)
{
	    Set_Volume_Val= (Set_Volume_Val-Volume_Stop_Value);

		if(GRAPH_VALUES.volume  <= Set_Volume_Val)
		{
					 if(Volume_Achieved_Status == VOLUME_NOT_ACHIEVED)
					 {
						 Volume_Mode_Blower_control.BLOWER_DAC_VAL  = (Volume_Mode_Blower_control.PREVIOUS_INITIAL_BLOWER_DAC_VAL+200);
								if(TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED > Blower_Cut_Off_Time)  //new
								{
									if(GRAPH_VALUES.flow <=5)
									{
										Blower_Status =BlOWER_OFF;
										Volume_Mode_Blower_control.BLOWER_DAC_VAL = 0;
										if(Alert_Check < 4)
										{
											Oxygen_Blending_Status=OXYGEN_ACHIEVED;
											Parkar_valve_Signal(0);
										}
									}
								}

								if(TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED > 100)  //new
								{
									Flow_Cummulative_Val  += GRAPH_VALUES.flow;
									Flow_Count++;
								}

								if(TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED < Flow_Detect_Time)   //new
								{
									if(GRAPH_VALUES.flow >= Set_Flow_Rate)
									{
										  if(Maximum_Flow_Achieved  == FLOW_NOT_ACHIEVED)
										  {
												Maximum_Flow_Achieved                     = FLOW_ACHIEVED;
												//TIME_Base_parameter.FLOW_ACHIEVED_TIMER   = Time_Limit;
										  }
								    }
								}

					 }
		}
		else
		{
		    Total_Volume_Achieved_Time                        = (Volume_Achieved_Time *10);
			Volume_Acheieved();
			Volume_Achieved_Status                            = VOLUME_ACHIEVED;
			Volume_Mode_Blower_control.BLOWER_DAC_VAL         = RESET;
		}
}
