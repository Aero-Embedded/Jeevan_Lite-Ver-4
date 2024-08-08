/**
 *******************************************************************************
 * @file Oxygen_Blending.c
 * @author Sarath Kumar
 * @date 27/03/24
 * @brief Implementation of Oxygen_Blending Concept.
 *
 * This file contains the implementation of Oxygen_Blending Concept within a
 * mode, including the initialization
 *******************************************************************************
 * */




/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include "Oxygen_Blending.h"

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

uint16_t O2_DAC_VALUE=0;            ///< control proportional valve
uint32_t O2_Flow_Event_Count;       ///< calculate O2 flow event attempt
uint32_t O2_Flow_Cumulative_Val;    ///< calculate O2 flow cumulative value
uint8_t O2_Flow_Rate_Need;          ///< calculate O2 flow rate needed
uint16_t Extra_DAC=0;               ///< in case need extra opening of proportional valve
uint8_t O2_Maximum_Limit=5;
uint8_t Last_Avg_Flow;              ///< Achieved O2 flow rate of last breath
uint8_t Additional_O2_DAC = 40;



/**
 * \brief Tune_O2.
 * \param FIO2_Val Set fio2 value from mode.
 * \param Achieved_Oxygen Achieved Oxygen from previous breathe.
 * */

static void Tune_O2(uint8_t,uint8_t);

/**
 *\brief Adjust_O2_Flow.
 *\param FIO2_Val Set fio2 value from mode.
 * */
float Adjust_O2_Flow(uint8_t);



/**
 * @brief Oxygen_Blending_Task.
 * This Task calls the Oxygen_Blending function.
 * */

void Oxygen_Blending_Task(void *argument)
{
	while(1)
	{
			Oxygen_Blending(Common_Mode_Parameter._FIO2_Val);
			vTaskDelay(Two_Millisecond_Delay);

	}
}


/**
 * @brief Oxygen_Blending.
 * This function mixes the oxygen level in the air during the mode to achieve a set value of FIO2.
 * @param FIO2_Val Set fio2 value from mode.
 * */

void Oxygen_Blending(uint8_t FIO2_Val)
{
	if(Run_Current_Breathe_State == Run_Inspiration_Cycle)
	{
		if(Fio2_Blending_Method == PRESSURE_CONTROLLED)
		{
			if(GRAPH_VALUES.pressure >= Common_Mode_Parameter._PIP_Val )
			{
				 if(Vent_Type == MASK_MODE)
				 {
					 Oxygen_Blending_Status   =  OXYGEN_ACHIEVED;
				 }
				 else if(Vent_Type == ET_TUBE_MODE)
				 {
					 Parkar_valve_Signal(0);
				     Oxygen_Blending_Status   =  OXYGEN_ACHIEVED;
				 }

			}
			if(GRAPH_VALUES.pressure>= 60 )
			{
				Parkar_valve_Signal(0);
				Oxygen_Blending_Status=OXYGEN_ACHIEVED;
			}
			else
			{
				if(FIO2_Val > 21)
				{
						if( (O2_Check < 4) && (Check_Oxygen_Count < 10) )
						{
							if(Oxygen_Blending_Status !=OXYGEN_ACHIEVED)
							{
								if(GRAPH_VALUES.oxygen_flow <= O2_Flow_Rate)
								{
									Oxygen_Parameter.O2_DAC=controlsys_Update(&Parker_valve_tune, O2_Flow_Rate , GRAPH_VALUES.oxygen_flow);
									Parkar_valve_Signal(Oxygen_Parameter.O2_DAC);
								}
								else
								{
									Oxygen_Parameter.O2_DAC=controlsys_Update(&Parker_valve_tune, O2_Flow_Rate , GRAPH_VALUES.oxygen_flow);
									Parkar_valve_Signal(Oxygen_Parameter.O2_DAC);
								}
							}
						}
				}

			}

		}
		else if(Fio2_Blending_Method == VOLUME_CONTROLLED)
		{
			if(GRAPH_VALUES.volume >= (Common_Mode_Parameter._VOLUME_Val-Volume_control_Task.Volume_Stop_Value))
			{
				Parkar_valve_Signal(0);
				Oxygen_Blending_Status=OXYGEN_ACHIEVED;
			}
			if(GRAPH_VALUES.pressure>= 60 )
			{
				Parkar_valve_Signal(0);
				Oxygen_Blending_Status=OXYGEN_ACHIEVED;
			}
			else
			{
				if(FIO2_Val > 21)
				{
						if( (O2_Check < 4) && (Check_Oxygen_Count < 10) )
						{
							if(Oxygen_Blending_Status !=OXYGEN_ACHIEVED)
							{
								if(GRAPH_VALUES.oxygen_flow <= O2_Flow_Rate)
								{
									Oxygen_Parameter.O2_DAC=controlsys_Update(&Parker_valve_tune, O2_Flow_Rate , GRAPH_VALUES.oxygen_flow);
									Parkar_valve_Signal(Oxygen_Parameter.O2_DAC);
								}
								else
								{
									Oxygen_Parameter.O2_DAC=controlsys_Update(&Parker_valve_tune, O2_Flow_Rate , GRAPH_VALUES.oxygen_flow);
									Parkar_valve_Signal(Oxygen_Parameter.O2_DAC);
								}
							}
						}
				}
			}
		}
	}
	else if(Run_Current_Breathe_State == Run_Expiration_Cycle)
	{
		 if(Vent_Type == MASK_MODE)
		 {

		 }
		 else if(Vent_Type == ET_TUBE_MODE)
		 {
			 if(Running_Mode == Aprv)
			 {

			 }
			 else
			 {
				 Parkar_valve_Signal(0);
			 }

		 }
	}
	if(Oxygen_Blending_Status == OXYGEN_NOT_ACHIEVED)
	{

	}
}



/**
 * @brief Calculate_O2.
 * This function limits the minimum and maximum values of the achieved O2.
 * @pre None.
 * @post None.
 * @param FIO2_Val Set fio2 value from mode.
 * @return None.
 * */


void Calculate_O2(uint8_t FIO2_Val)
{
	if(FIO2_Val > 21)
	{
		Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;

		if(Oxygen_Parameter.Achieved_Oxygen >=100)
		    Oxygen_Parameter.Achieved_Oxygen = 100;
		if(Oxygen_Parameter.Achieved_Oxygen<=20)
			Oxygen_Parameter.Achieved_Oxygen = 20;
	}
	else
	{
		Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;

		if(Oxygen_Parameter.Achieved_Oxygen >=100)
		    Oxygen_Parameter.Achieved_Oxygen = 100;
		if(Oxygen_Parameter.Achieved_Oxygen<=20)
			Oxygen_Parameter.Achieved_Oxygen = 20;

		Oxygen_Parameter.O2_DAC=0;
		Parkar_valve_Signal(0);
	}
}



/**
 * @brief Calculate_O2.
 * This function calculates the required flow rate of O2 based on the set flow rate.
 * @param FIO2_Val Set fio2 value from mode.
 * @param Flowrate Set Flow rate to achieve a O2.
 * */
void O2_Initial_Parameter(uint8_t FIO2_Val,uint8_t Flowrate)
{

	if(Fio2_Blending_Method == PRESSURE_CONTROLLED)
	{
		vTaskResume(Oxygen_Blending_Handler);
	}
	else
	{
		O2_Flow_Rate   = ( Flowrate* ( (float)( FIO2_Val - 21) / 100) );
		if(O2_Flow_Rate <10)
		{
			O2_Flow_Rate = 10;
		}
		vTaskResume(Oxygen_Blending_Handler);
	}


}



/**
 * @brief Low_Oxygen_Achieved.
 * This function is used to increase the proportional valve opening when low oxygen levels are detected."
 * @param FIO2_Val Set fio2 value from mode.
 * @param Achieved_Oxygen Achieved Oxygen from previous breathe.
 * */


void Low_Oxygen_Achieved(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{

	O2_Low_Count++;
	if(O2_Low_Count >1)
	{
		if(  ( Achieved_Oxygen)  < ( FIO2_Val) && (Achieved_Oxygen  >= (FIO2_Val - 2) ) )
		{
			Oxygen_Parameter.O2_DAC = (Oxygen_Parameter.O2_DAC+ET_O2_Envitec_Tune_Value1);
		}
		else if(  ( Achieved_Oxygen)  < ( FIO2_Val - 2 ) && (Achieved_Oxygen  > (FIO2_Val - 5) ) )
		{
			Oxygen_Parameter.O2_DAC = (Oxygen_Parameter.O2_DAC+ET_O2_Envitec_Tune_Value2);
		}
		else if(  ( Achieved_Oxygen)  <= ( FIO2_Val - 5 )  )
		{
			Oxygen_Parameter.O2_DAC = (Oxygen_Parameter.O2_DAC+ET_O2_Envitec_Tune_Value3);
		}
		O2_Low_Count = 0;
	}

	if(Oxygen_Parameter.O2_DAC>=3300)
	{
		Oxygen_Parameter.O2_DAC=3300;
	}
	O2_High_Count = 0;
	Parkar_valve_Signal(Oxygen_Parameter.O2_DAC);
}



/**
 * @brief High_Oxygen_Achieved.
 * This function is used to decrease the proportional valve opening when high oxygen levels are detected.
 * @param FIO2_Val Set fio2 value from mode.
 * @param Achieved_Oxygen Achieved Oxygen from previous breathe.
 * */
void High_Oxygen_Achieved(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{
		O2_High_Count++;
		if(O2_High_Count >1)
		{
			if(  ( Achieved_Oxygen)  > ( FIO2_Val ) && (Achieved_Oxygen  <= (FIO2_Val + 2) ) )
			{
				Oxygen_Parameter.O2_DAC = (Oxygen_Parameter.O2_DAC-ET_O2_Envitec_Tune_Value1);
			}
			else if(  ( Achieved_Oxygen)  > ( FIO2_Val + 2 ) && (Achieved_Oxygen  < (FIO2_Val + 5) ) )
			{
				Oxygen_Parameter.O2_DAC = (Oxygen_Parameter.O2_DAC-ET_O2_Envitec_Tune_Value2);
			}
			else if(  ( Achieved_Oxygen)  >= ( FIO2_Val + 5 )  )
			{
				Oxygen_Parameter.O2_DAC = (Oxygen_Parameter.O2_DAC-ET_O2_Envitec_Tune_Value3);
			}
			O2_High_Count = 0;
		}

		if(Oxygen_Parameter.O2_DAC<=1400)
		{
			Oxygen_Parameter.O2_DAC=1400;
		}
		O2_Low_Count = 0;

	Parkar_valve_Signal(Oxygen_Parameter.O2_DAC);
}



/**
 * @brief Tune_O2.
 *This function checks whether the achieved O2 value is high or low."
 * @param FIO2_Val Set fio2 value from mode.
 * @param Achieved_Oxygen Achieved Oxygen from previous breathe.
 * */
static void Tune_O2(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{
	    if (Achieved_Oxygen < (FIO2_Val-1))
		{
			Low_Oxygen_Achieved(FIO2_Val,Achieved_Oxygen);
		}
		else if (Achieved_Oxygen > (FIO2_Val))
		{
			High_Oxygen_Achieved(FIO2_Val,Achieved_Oxygen);
		}
		else
			Parkar_valve_Signal(Oxygen_Parameter.O2_DAC);

		Last_O2_Dac_Val = Oxygen_Parameter.O2_DAC;

		Parkar_valve_Signal(Oxygen_Parameter.O2_DAC);
}


/**
 * @brief Adjust O2 Flow.
 * This function calculates the flow percentage required to achieve the desired O2 level.
 * @param FIO2_Val Set fio2 value from mode.
 * */

float Adjust_O2_Flow(uint8_t FIO2_Val)
{
	if(FIO2_Val >= 80)
	{
		return (((FIO2_Val - 25)*0.25)/75)+0.85;
	}
	else
	{
		return (((FIO2_Val - 25)*0.25)/75)+0.75;
	}

}

/**
 * @brief O2 DAC Control.
 * This function is always called when the O2 set value is above 21."
 * @param FIO2_Val Set fio2 value from mode.
 * */


void O2_DAC_Control(uint8_t FIO2_Val)
{
	if(Fio2_Blending_Method == PRESSURE_CONTROLLED)
	{
		if(FIO2_Val  > 21)
		{
			Check_Oxygen_Count++;

			if(Check_Oxygen_Count == 10)
			{
				Oxygen_Parameter.O2_DAC = (Oxygen_Parameter.O2_DAC + Additional_O2_DAC);
			}

			if(Check_Oxygen_Count >= 10)
			{
			   Oxygen_Blending_Status           = OXYGEN_NOT_ACHIEVED;
			   Tune_O2(FIO2_Val,Oxygen_Parameter.Achieved_Oxygen);
			   if(Check_Oxygen_Count>=13)
				   Check_Oxygen_Count=13;

			}
			else if( O2_Check < 4)
			{
				Oxygen_Blending_Status=OXYGEN_NOT_ACHIEVED;
				O2_Flow_Rate   = (Adjust_O2_Flow(FIO2_Val)*Max_Flow* ( (float)( FIO2_Val - 21) / 100) );
				if(O2_Flow_Rate <=2)
				{
					O2_Flow_Rate = 2;
				}
				Last_Avg_Flow = Avg_Flow;
				Avg_Flow=0;
			}
		}
		else
		{
			Oxygen_Blending_Status=OXYGEN_NOT_ACHIEVED;
			Parkar_valve_Signal(0);
		}
	}
	else if(Fio2_Blending_Method == VOLUME_CONTROLLED)
	{
		if(FIO2_Val  > 21)
		{
			Check_Oxygen_Count++;

			if(Check_Oxygen_Count == 10)
			{
				Oxygen_Parameter.O2_DAC = (Oxygen_Parameter.O2_DAC + Additional_O2_DAC);
			}

			if(Check_Oxygen_Count >= 10)
			{
			   Oxygen_Blending_Status           = OXYGEN_NOT_ACHIEVED;
			   Tune_O2(FIO2_Val,Oxygen_Parameter.Achieved_Oxygen);
			   if(Check_Oxygen_Count>=13)
				   Check_Oxygen_Count=13;
			}
			else if(O2_Check < 4)
			{
				Oxygen_Blending_Status=OXYGEN_NOT_ACHIEVED;
				O2_Flow_Rate   = ( Avg_Flow* ( (float)( FIO2_Val - 21) / 100) );
				if(O2_Flow_Rate <=2)
				{
					O2_Flow_Rate = 2;
				}
				Last_Avg_Flow = Avg_Flow;
				Avg_Flow=0;
			}

		}
		else
		{
			Oxygen_Blending_Status=OXYGEN_NOT_ACHIEVED;
			Parkar_valve_Signal(0);
		}
	}
}








