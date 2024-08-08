/**
 *******************************************************************************
 * @file PIP_Control.c
 * @author Sarath Kumar
 * @date 27/03/24
 * @brief Implementation to control a PIP.
 *
 * This file contains the implementation to control a PIP.
 *******************************************************************************
 * */


#include "PIP_Control.h"


uint8_t Minimum_Inc_Value_1=30;
uint8_t Minimum_Inc_Value_2=100;
uint8_t Maximum_Inc_Value=140;
uint8_t Deviation = 5;
uint8_t Maximum_Time_Limit=200;



/**
 * @brief PIP_Control_Task.
 * This task controls the entire PIP during the inspiration time.
 * */

void PIP_Control_Task(void *argument)
{
	while(1)
	{
		if(Run_Current_Breathe_State == Run_Inspiration_Cycle)
		{

			if(GRAPH_VALUES.pressure >= 60)
			{

				PIP_Status = PIP_ACHEIVED;
				if(Read_Time_vs_Pressure==READ_TIME_OPEN)
				{
					PIP_Acheieved();
					PIP_control_Task.PIP_Acheived_Time_Ms       = (PIP_control_Task.PIP_Control_Event_Occured*PIP_control_Task.PIP_Control_Task_Delay);
					Set_PIP_Status                              = SET_PIP_VALUE_ACHEIVED;
					Read_Time_vs_Pressure                       = READ_TIME_CLOSE;
				}


				Pressure_Mode_blower_control.BLOWER_DAC_VAL  = 0;
				Blower_Status =BlOWER_OFF;
			}
			else
			{
				if(Blower_Status == BlOWER_ON)
				{
					PIP_Control(Common_Mode_Parameter._PIP_Val);
				}
			}

		}
		vTaskDelay(PIP_control_Task.PIP_Control_Task_Delay);
	}
}


/**
 * @brief PIP_Control_Task.
 * This function is the core logic of PIP control.
 * */

void PIP_Control(uint8_t PIP_VAL)
{
	PIP_control_Task.PIP_Control_Event_Occured++;
	if (GRAPH_VALUES.pressure >= (PIP_VAL))
	{
		PIP_Status = PIP_ACHEIVED;
		if(Read_Time_vs_Pressure==READ_TIME_OPEN)
		{
			PIP_Acheieved();
			PIP_control_Task.PIP_Acheived_Time_Ms       = (PIP_control_Task.PIP_Control_Event_Occured*PIP_control_Task.PIP_Control_Task_Delay);
			Pressure_Mode_blower_control.BLOWER_DAC_VAL = Pressure_Mode_blower_control.LAST_BLOWER_DAC_VAL;
			Set_PIP_Status                              = SET_PIP_VALUE_ACHEIVED;
			Read_Time_vs_Pressure                       = READ_TIME_CLOSE;
		}

	}
	else if (PIP_Status == PIP_NOT_ACHEIVED)
	{
		if (Pressure_Mode_blower_control.LAST_BLOWER_DAC_VAL >= Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL)
		{
			Pressure_Mode_blower_control.LAST_BLOWER_DAC_VAL = Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL;
			PIP_Status = PIP_ACHEIVED;
		}
		else if (Pressure_Mode_blower_control.LAST_BLOWER_DAC_VAL < Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL)
		{
			Pressure_Mode_blower_control.LAST_BLOWER_DAC_VAL = Pressure_Mode_blower_control.BLOWER_DAC_VAL;
			Pressure_Mode_blower_control.BLOWER_DAC_VAL      = (Pressure_Mode_blower_control.LAST_BLOWER_DAC_VAL + (PIP_control_Task.Increase_Each_DAC_SET_CONST ));

			if( (Common_Mode_Parameter._PEEP_Val >= Smooth_Ramp_Time_Val) && (Common_Mode_Parameter._PIP_Val  >=30) )
			{
				if (Pressure_Mode_blower_control.BLOWER_DAC_VAL >= Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL)
				{
					Pressure_Mode_blower_control.BLOWER_DAC_VAL      = Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL;
					Pressure_Mode_blower_control.LAST_BLOWER_DAC_VAL = Pressure_Mode_blower_control.BLOWER_DAC_VAL;
				}
			}

			if(Pressure_Mode_blower_control.BLOWER_DAC_VAL >=4095)
				Pressure_Mode_blower_control.BLOWER_DAC_VAL=4095;

		}
	}
}


/**
 * @brief INCREASE_EACH_DAC_SET_CONST.
 * This function controls the ramptime of pip.
 * */

void INCREASE_EACH_DAC_SET_CONST(uint8_t PIP_VAL, uint16_t Rise_Time_MS)
{
	if(PIP_control_Task.PIP_Acheived_Time_Ms <=  (Rise_Time_MS-50) )
	{
		PIP_ACHEIVED_EARLY(PIP_VAL,Rise_Time_MS);
	}
	else if(PIP_control_Task.PIP_Acheived_Time_Ms >=  (Rise_Time_MS+50) )
	{
		PIP_ACHEIVED_SLOWLY(Rise_Time_MS);
	}
	else
	{
		PIP_ACHEIVED_NORMALY();
	}

	    _PIP_NOT_ACHEIVED();
}




/**
 * @brief PIP_ACHEIVED_EARLY.
 * This function controls the ramp time of PIP because the PIP is achieved earlier than the set ramp time.
 * */
void PIP_ACHEIVED_EARLY(uint8_t PIP_VAL,uint16_t Rise_Time_MS)
{
	if(Set_PIP_Status == SET_PIP_VALUE_NOT_ACHEIVED)
	{

	}
	else
	{
		PIP_control_Task.Early_Acheived_Ramp_Time_Percentage = (uint16_t) (  (PIP_control_Task.PIP_Acheived_Time_Ms  / ((float)Rise_Time_MS)) * 100);

		if(PIP_control_Task.Early_Acheived_Ramp_Time_Percentage <= 70)
		{
			PIP_control_Task.Increase_Each_DAC_SET_CONST = (PIP_control_Task.Increase_Each_DAC_SET_CONST - 5 );
			    if(PIP_control_Task.Increase_Each_DAC_SET_CONST <= Minimum_Inc_Value_1)
			    {
			    	PIP_control_Task.Increase_Each_DAC_SET_CONST = Minimum_Inc_Value_1;
			    	Need_PID_Task_Delay                          = YES;
			    }
		}

		else if(  (PIP_control_Task.Early_Acheived_Ramp_Time_Percentage <= 80) && (PIP_control_Task.Early_Acheived_Ramp_Time_Percentage > 70) )
		{
			PIP_control_Task.Increase_Each_DAC_SET_CONST = (PIP_control_Task.Increase_Each_DAC_SET_CONST - 2 );
		    if(PIP_control_Task.Increase_Each_DAC_SET_CONST <= Minimum_Inc_Value_1)
		    {
		    	PIP_control_Task.Increase_Each_DAC_SET_CONST = Minimum_Inc_Value_1;
		    	Need_PID_Task_Delay                          = YES;
		    }
		}

		else if(  (PIP_control_Task.Early_Acheived_Ramp_Time_Percentage <= 90) && (PIP_control_Task.Early_Acheived_Ramp_Time_Percentage > 80) )
		{
			PIP_control_Task.Increase_Each_DAC_SET_CONST = (PIP_control_Task.Increase_Each_DAC_SET_CONST - 1 );

		    if(PIP_control_Task.Increase_Each_DAC_SET_CONST <= Minimum_Inc_Value_1)
		    {
		    	PIP_control_Task.Increase_Each_DAC_SET_CONST = Minimum_Inc_Value_1;
		    	Need_PID_Task_Delay                          = YES;
		    }
		}



		if(Need_PID_Task_Delay == YES)
		{
			if(PIP_Average_Parameter.Maximum_PIP_Acheived  < PIP_VAL)
			{
				PIP_control_Task.Low_PIP_Count++;
				 if(PIP_control_Task.Low_PIP_Count>=5)
				 {
					PIP_control_Task.PIP_Control_Task_Delay = PIP_control_Task.PIP_Control_Task_Delay;
					PIP_control_Task.Low_PIP_Count=6;
					Tune_PID_Task_Delay = T_LOCK;
				 }

			}
			else if(PIP_control_Task.Early_Acheived_Ramp_Time_Percentage  >= PIP_control_Task.Last_Early_Acheived_Ramp_Time_Percentage)
			{
				if(Tune_PID_Task_Delay == T_OPEN)
				{
					  if(PIP_control_Task.Early_Acheived_Ramp_Time_Percentage<60)
					  {
						  PIP_control_Task.PIP_Control_Task_Delay = (PIP_control_Task.PIP_Control_Task_Delay+10);
					  }
					  else if( (PIP_control_Task.Early_Acheived_Ramp_Time_Percentage<70) && (PIP_control_Task.Early_Acheived_Ramp_Time_Percentage>=60) )
					  {
						  PIP_control_Task.PIP_Control_Task_Delay = (PIP_control_Task.PIP_Control_Task_Delay+6);
					  }
					  else if( (PIP_control_Task.Early_Acheived_Ramp_Time_Percentage<=80) && (PIP_control_Task.Early_Acheived_Ramp_Time_Percentage=70) )
					  {
						  PIP_control_Task.PIP_Control_Task_Delay = (PIP_control_Task.PIP_Control_Task_Delay+4);
					  }
					  else
					  {
						  PIP_control_Task.PIP_Control_Task_Delay = (PIP_control_Task.PIP_Control_Task_Delay+1);
					  }

					  if( PIP_control_Task.PIP_Control_Task_Delay>=Maximum_Time_Limit)
					  {
						  PIP_control_Task.PIP_Control_Task_Delay = Maximum_Time_Limit;
						  Need_PID_Task_Delay = NO;
					  }
				}
			}
		}

	}
}



/**
 * @brief PIP_ACHEIVED_SLOWLY.
 * This function controls the ramp time of PIP because the PIP is achieved slower than the set ramp time.
 * */
void PIP_ACHEIVED_SLOWLY(uint16_t Rise_Time_MS)
{
	PIP_control_Task.Slowly_Acheived_Ramp_Time_Percentage = (uint16_t) ((  PIP_control_Task.PIP_Acheived_Time_Ms  / ((float)Rise_Time_MS) ) * 100);


	if( (Tune_PID_Task_Delay == T_OPEN) && (Need_PID_Task_Delay == YES) )                  // new
	{
		  if(PIP_control_Task.Slowly_Acheived_Ramp_Time_Percentage>130)
		  {
			  PIP_control_Task.PIP_Control_Task_Delay = (PIP_control_Task.PIP_Control_Task_Delay-10);
		  }
		  else if( (PIP_control_Task.Slowly_Acheived_Ramp_Time_Percentage<130) && (PIP_control_Task.Slowly_Acheived_Ramp_Time_Percentage>120) )
		  {
			  PIP_control_Task.PIP_Control_Task_Delay = (PIP_control_Task.PIP_Control_Task_Delay-6);
		  }
		  else if( (PIP_control_Task.Slowly_Acheived_Ramp_Time_Percentage<=120) && (PIP_control_Task.Slowly_Acheived_Ramp_Time_Percentage=110) )
		  {
			  PIP_control_Task.PIP_Control_Task_Delay = (PIP_control_Task.PIP_Control_Task_Delay-4);
		  }
		  else
		  {
			  PIP_control_Task.PIP_Control_Task_Delay = (PIP_control_Task.PIP_Control_Task_Delay-1);
		  }

		  if( PIP_control_Task.PIP_Control_Task_Delay<=10)
		  {
			  PIP_control_Task.PIP_Control_Task_Delay = 10;
			  Need_PID_Task_Delay = NO;
			  Tune_PID_Task_Delay = T_OPEN;
		  }


	}
	else
	{
		if( (PIP_control_Task.Slowly_Acheived_Ramp_Time_Percentage >= 130)  )
		{
			PIP_control_Task.Increase_Each_DAC_SET_CONST = (PIP_control_Task.Increase_Each_DAC_SET_CONST + 5 );
			if(PIP_control_Task.Increase_Each_DAC_SET_CONST >= Maximum_Inc_Value)
			{
				PIP_control_Task.Increase_Each_DAC_SET_CONST = Maximum_Inc_Value;

			}
		}
		else if(  (PIP_control_Task.Slowly_Acheived_Ramp_Time_Percentage> 120) && (PIP_control_Task.Slowly_Acheived_Ramp_Time_Percentage <130) )
		{
			PIP_control_Task.Increase_Each_DAC_SET_CONST = (PIP_control_Task.Increase_Each_DAC_SET_CONST + 2 );
			if(PIP_control_Task.Increase_Each_DAC_SET_CONST >= Maximum_Inc_Value)
			{
				PIP_control_Task.Increase_Each_DAC_SET_CONST = Maximum_Inc_Value;

			}
		}
		else if( (PIP_control_Task.Slowly_Acheived_Ramp_Time_Percentage<=120) && (PIP_control_Task.Slowly_Acheived_Ramp_Time_Percentage >= 110) )
		{
			PIP_control_Task.Increase_Each_DAC_SET_CONST = (PIP_control_Task.Increase_Each_DAC_SET_CONST + 1 );
			if(PIP_control_Task.Increase_Each_DAC_SET_CONST >= Maximum_Inc_Value)
			{
				PIP_control_Task.Increase_Each_DAC_SET_CONST = Maximum_Inc_Value;

			}
		}
	}



}

/**
 * @brief PIP_ACHEIVED_NORMALY.
 * This function works fine when ramp time witin a set value.
 * */
void PIP_ACHEIVED_NORMALY()
{
	PIP_control_Task.Increase_Each_DAC_SET_CONST = PIP_control_Task.Increase_Each_DAC_SET_CONST;
}

/**
 * @brief _PIP_NOT_ACHEIVED.
 * This function checks and controls whether the set PIP has been achieved or not.
 * */
void _PIP_NOT_ACHEIVED()
{
	if(Set_PIP_Status == SET_PIP_VALUE_NOT_ACHEIVED)
	{
		PIP_control_Task.Increase_Each_DAC_SET_CONST = (PIP_control_Task.Increase_Each_DAC_SET_CONST + 1 );
		if(PIP_control_Task.Increase_Each_DAC_SET_CONST >= Maximum_Inc_Value)
		{
			PIP_control_Task.Increase_Each_DAC_SET_CONST = Maximum_Inc_Value;

		}

		 PIP_control_Task.PIP_Control_Task_Delay = (PIP_control_Task.PIP_Control_Task_Delay-1);
		  if( PIP_control_Task.PIP_Control_Task_Delay<=10)
		  {
			  PIP_control_Task.PIP_Control_Task_Delay = 10;
		  }
	}
}


/**
 * @brief BLOWER_ENDING_DAC_SIGNAL_TUNE.
 * This function controls the ending DAC value to maintain the PIP.
 * */
void BLOWER_ENDING_DAC_SIGNAL_TUNE(uint8_t PIP_VAL,uint32_t Inspiration_Time,uint8_t Rise_Time_Percent)
{
	if(Inspiration_Time<=800 || Rise_Time_Percent>=85 )
    {
		    if(PIP_Average_Parameter.Maximum_PIP_Acheived > (PIP_VAL+1))   //new
			{
		    	count1++;
		    	count2=0;
		    	if(count1>2)
		    	{
		    		if(PIP_Average_Parameter.Maximum_PIP_Acheived > (PIP_VAL+3))
		    		{
		    			Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL=(Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL-20);
		    		}
		    		else
		    		{
		    			Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL=(Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL-5);
		    		}

		    		count1=0;
					if(Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL<=500)
					{
						Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL=500;
					}
		    	}
			}
			else if(PIP_Average_Parameter.Maximum_PIP_Acheived < PIP_VAL)
			{
				count2++;
				count1=0;
				if(count2>2)
				{
					if(PIP_Average_Parameter.Maximum_PIP_Acheived < (PIP_VAL-3))
					{
						Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL=Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL+20;
					}
					else
					{
						Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL=Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL+5;
					}

				    count2=0;
						if(  Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL  >= 4095)
						{
							Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = 4095;
						}
				}
			}
			else
			{
				count1=0;
				count2=0;
				Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL;
			}
    }

 else
 {
	if(PIP_Average_Parameter.PIP_AVG_VAL > (PIP_VAL ))
	{
		count1++;
		count2=0;
		if(count1>0)
		{
			Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = (Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL-Deviation);
			count1=0;
			if(Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL<=500)
				Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL=500;
		}
	}
	else if(PIP_Average_Parameter.PIP_AVG_VAL < (PIP_VAL) )
	{
		count2++;
		count1=0;
		if(count2>0)
		{
			Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = (Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL+Deviation);
			count2=0;
			if(Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL >= 4095)
				Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = 4095;
		}
	}
	else
	{
		count1=0;
		count2=0;
		Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL = Pressure_Mode_blower_control.BLOWER_ENDING_DAC_SIGNAL;
	}
 }
}
