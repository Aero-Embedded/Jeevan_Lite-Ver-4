/**
 * ******************************************************************************
 * \file HFNC.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Implementation of HFNC Feature.
 *
 * This file contains the Declaration of HFNC control Function, Including HFNC Data Split and
 * Alert Control.
 * ******************************************************************************
 **/

#ifndef INC_AERO_INC_HFNC_HFNC_H_
#define INC_AERO_INC_HFNC_HFNC_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/


#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"


/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/


/**
 * \brief Structure Representing HFNC_Mode_Parameter variables.
 * it includes OXYGEN Flow rate and oxygen level.
 */

typedef struct
{
	uint16_t _Flow_Rate;       ///< flow rate value variable.
	uint8_t _FIO2_Val;         ///< Fio2 value variable.

}HFNC_Mode_Parameter;


xTaskHandle HFNC_Handler;     ///< HFNC Task handler.



/**
 * \brief HFNC_Task.
 **/

void HFNC_Task (void *argument);


/**
 * \brief HFNC_DAC_Value.
 **/
uint16_t HFNC_DAC_Value(uint8_t);

HFNC_Mode_Parameter HFNC;     ///< HFNC mode variables.
uint8_t FLOW_AVG;             ///< FLOW AVERAGE variables.



#endif /* INC_AERO_INC_HFNC_HFNC_H_ */
