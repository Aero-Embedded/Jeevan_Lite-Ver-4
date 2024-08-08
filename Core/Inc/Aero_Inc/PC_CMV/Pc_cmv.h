/**
 *******************************************************************************
 * \file Pc_cmv.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of PCCMV mode.
 *
 * This file contains the Declaration PCCMV mode variables
 *******************************************************************************
 **/

#ifndef INC_PC_CMV_H_
#define INC_PC_CMV_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/


#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/


#define One_Minite_In_MS 60000             ///< one minute for breath update


/**
 * \brief Structure Representing PCCMV Mode parameter variables.
 * it includes expiration time ,rise time percentage variables.
 */

typedef struct
{
	uint8_t  PIP_Val;                  ///< PIP Parameter.
	uint8_t  PEEP_Val;                 ///< PEEP Parameter.
	uint8_t  RESPIRATORY_RATE_Val;     ///< RR Parameter.
	uint8_t  FIO2_Val;                 ///< FIO2 Parameter.
	float    RISE_TIME_MS_Val;         ///< RISE TIME CALCULATED VALUE.
	uint32_t INSPIRATION_TIME;         ///< INSPIRATION TIME Parameter.
	uint32_t EXPIRATION_TIME;          ///< EXPIRATION TIME  Parameter.
	uint8_t  Inspiration_Time_Ratio;   ///< INSPIRATION TIME RATIO Calculated value.
	uint8_t  Expiration_Time_Ratio;    ///< EXPIRATION TIME RATIO Calculated value.
	uint8_t  Rise_Time;                ///< RISE TIME Parameter.
	uint8_t  Rise_Time_percentage;     ///< ACHIEVED RISETIME PERCENTAGE.

}PC_CMV_mode_parameter;


/**
 *\brief Enumeration for Ventilation type.
 * To identify the mode type as ET tube or mask mode.
 */

typedef enum
{
	ET_TUBE_MODE,         ///< Et tube mode Ventilation.
	MASK_MODE,            ///< mask mode Ventilation.
	NO_MODE               ///< idle.
}Vent_type;

Vent_type Vent_Type;    ///< type of ventilation.



/**
 *\brief Enumeration for Ramp time status.
 * To check whether the set PIP value is achieved during ramp time.
 */
typedef enum
{
	Smooth_Ramp_Time_Val_Pending,      ///<  Ramptime not in set time.
	Smooth_Ramp_Time_Val_Finished      ///<  Ramptime in set time.
}smooth_ramp_time;


/**
 *\brief Enumeration for Breath count.
 *To count the number of breaths delivered by the machine or taken by the patient.
 */

typedef enum
{
	Leave_Count_The_Breath,         ///<  Don't count the breath.
	Count_The_Breath                ///<  Count the breath.
}breath;


breath Breath_Count;                   ///< enumeration variable

smooth_ramp_time Smooth_Ramp_Time;     ///< Ramptime structure variable
PC_CMV_mode_parameter PC_CMV;          ///< pccmv mode parameter variable


xTaskHandle Pc_Cmv_Handler;           ///< pccmv mode handler

/**
 * \brief Pc_Cmv_Task.
 * */
void Pc_Cmv_Task (void *argument);


/**
 * \brief PC_CMV_Inspiration_Time_Variables_Reset.
 * */
void PC_CMV_Inspiration_Time_Variables_Reset();

/**
 * \brief PC_CMV_Expiration_Time_Variables_Reset.
 * */
void PC_CMV_Expiration_Time_Variables_Reset();

/**
 * \brief PIP_Not_Acheieved.
 * */
void PIP_Not_Acheieved();

/**
 * \brief PIP_Acheieved.
 * */
void PIP_Acheieved();

/**
 * \brief Find_Average_Flow.
 * */
void Find_Average_Flow();           //new


#endif /* INC_PC_CMV_H_ */
