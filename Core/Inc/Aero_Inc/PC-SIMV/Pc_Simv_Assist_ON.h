/**
 *******************************************************************************
 * \file Pc_Simv_Assist_ON.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief implementation of Pc_Simv_Assist_ON mode.
 *
 * This file contains the implementation Pc_Simv_Assist_ON mode variables
 *******************************************************************************
 **/

#ifndef INC_AERO_INC_PC_SIMV_PC_SIMV_ASSIST_ON_H_
#define INC_AERO_INC_PC_SIMV_PC_SIMV_ASSIST_ON_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/
#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"
/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

#define E_TIME_TOLERANCE 30u     ///< ignore expiratory phase of initial 30 %of time



/**
 * \brief Structure Representing PCSIMV Mode parameter variables.
 * it includes expiration time ,rise time percentage variables.
 */


typedef struct
{
	uint8_t  PIP_Val;                      ///< PIP.Parameter.
	uint8_t  PEEP_Val;                     ///< PEEP Parameter.
	uint8_t  RESPIRATORY_RATE_Val;         ///< RR Parameter.
	uint8_t  FIO2_Val;                     ///< FIO2 Parameter.
	float    RISE_TIME_MS_Val;             ///< RISE TIME CALCULATED VALUE.
	uint32_t INSPIRATION_TIME;             ///< INSPIRATION TIME Parameter.
	uint32_t EXPIRATION_TIME;              ///< EXPIRATION TIME  Parameter.
	uint8_t  Inspiration_Time_Ratio;       ///< INSPIRATION TIME RATIO Calculated value.
	uint8_t  Expiration_Time_Ratio;        ///< EXPIRATION TIME RATIO Calculated value.
	uint8_t  Rise_Time;                    ///< RISE TIME Parameter.
	uint8_t  TRIG_TYPE;                    ///< pressure trigger or flow trigger
	float    TRIG_LMT;                     ///< trigger limit
	uint16_t  TRIG_TIME;                   ///< trigger time
	uint16_t CALC_TRIG_VAL;                ///< trigger time calculated
	uint16_t TOLERANCE_EWAVE;              ///< tolerance of expiration time
	uint16_t TRIG_WINDOW;                  ///< calculated value trigger window
	uint8_t  Assist_Control;               ///< assist control on/off
	uint8_t  Rise_Time_percentage;         ///< ACHIEVED RISETIME PERCENTAGE.

}PC_SIMV_mode_parameter;


/**
 *\brief Enumeration for assist type.
 * To set a running pcsimv mode is assist on or off.
 */


typedef enum
{
	ASSIST_ON,       ///< Assist on type
	ASSIST_OFF       ///< Assist off type
}assist;

/**
 *\brief Enumeration for trigger type.
 * To Select a pressure trigger or flow trigger.
 */

typedef enum
{
	Pressure_Trigger,        ///< Pressure trigger.
	Flow_Trigger             ///< Flow trigger.
}trigger_type;


/**
 *\brief Enumeration for Patient trigger status.
 * To Know about patient trigger status.
 */

typedef enum
{
	NO_PATIENT_TRIGGER,           ///<   no patient trigger
	PATIENT_TRIGGER_HAPPEN        ///<   patient trigger happen
}Patient_trigger;

xTaskHandle Pc_Simv_Assist_ON_Handler;        ///< pc simv asssit on handler

/**
 *\brief Pc_Simv_Assist_ON_Task.
 * This task is used for control a pc simv mode.
 */
void Pc_Simv_Assist_ON_Task(void *argument);

/**
 * \brief PC_SIMV_Inspiration_Time_Variables_Reset.
 * */

void PC_SIMV_Inspiration_Time_Variables_Reset();

/**
 * \brief PC_SIMV_Expiration_Time_Variables_Reset.
 * */
void PC_SIMV_Expiration_Time_Variables_Reset();

/**
 * \brief PC_SIMV_Assist_ON_Inspiration_Time_Variables_Reset.
 * */
void PC_SIMV_Assist_ON_Inspiration_Time_Variables_Reset();

/**
 * \brief PC_SIMV_Assist_ON_Expiration_Time_Variables_Reset.
 * */
void PC_SIMV_Assist_ON_Expiration_Time_Variables_Reset();




PC_SIMV_mode_parameter PC_SIMV;           ///<  PCSIMV MODE structure variable.
assist Assist_Control;                    ///<  assist control structure variable.
trigger_type Trigger_Type;                ///<  trigger type structure variable.
Patient_trigger Patient_Trigger;          ///<  patient trigger status strucure variable.

#endif /* INC_AERO_INC_PC_SIMV_PC_SIMV_ASSIST_ON_H_ */
