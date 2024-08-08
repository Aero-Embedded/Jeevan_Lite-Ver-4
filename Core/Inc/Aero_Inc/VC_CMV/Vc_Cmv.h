/**
 *******************************************************************************
 * \file Vc_cmv.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of VCCMV mode.
 *
 * This file contains the Declaration VCCMV mode variables
 *******************************************************************************
 **/

#ifndef INC_VC_CMV_H_
#define INC_VC_CMV_H_

#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"

/**
 * \brief Structure Representing VCCMV Mode parameter variables.
 * it includes expiration time ,rise time percentage variables.
 */

typedef struct
{
	uint16_t VOLUME_Val;               ///< Volume Parameter.
	uint8_t  PEEP_Val;                 ///< PEEP Parameter.
	uint8_t  RESPIRATORY_RATE_Val;     ///< RR Parameter.
	uint8_t  FIO2_Val;                 ///< FIO2 Parameter.
	uint32_t INSPIRATION_TIME;         ///< INSPIRATION_TIME Parameter.
	uint32_t EXPIRATION_TIME;          ///< EXPIRATION_TIME Parameter.
	uint8_t  Inspiration_Time_Ratio;   ///< Inspiration_Time_Ratio Parameter.
	uint8_t  Expiration_Time_Ratio;    ///< Expiration_Time_Ratio Parameter.
	uint8_t  FLOW_RATE_Val;            ///< FLOW_RATE_Val Parameter.

}VC_CMV_mode_parameter;


/**
 *\brief Enumeration for Flow achieved status type.
 * To identify whether the flow has been achieved or not.
 */
typedef enum
{
	Flow_fine,            ///< flow achieved normal.
	Low_Flow_Problem,     ///< low flow achieved.
	High_Flow_Problem     ///< high flow achieved.
}Flow_status;

/**
 *\brief Enumeration for Set Flow achieved or not.
 * To identify whether the achieved flow has been detected within a set value.
 */
typedef enum
{
	Not_Detect,           ///< Flow not detect.
	Detect                ///< Flow detected.
}set_flow_value;



/**
 *\brief Enumeration for blower need to on or off in vc cmv mode control algorithm.
 * To update the blower DAC value using this enumeration.
 */
typedef enum
{
	BlOWER_ON,     ///< BlOWER_ON.
	BlOWER_OFF     ///< BlOWER_OFF.
}Blower_stat;

Flow_status Flow_increment;   ///< Enumeration variable

set_flow_value Set_Flow_value;    ///< Enumeration variable
uint16_t LOW_COMPLIANCE_DAC;      ///< identify compliance dac variable
int8_t Low_Flow_Rate;             ///< detect low flow rate variable
uint8_t Stop_Increment_Count,Last_Flow_increment;   ///< Stop_Increment_Count, Last_Flow_increment variable

VC_CMV_mode_parameter VC_CMV;         ///< Structure variable

xTaskHandle Vc_Cmv_Handler;           ///< Task handler name
void Vc_Cmv_Task (void *argument);    ///< Task variable



/**
 * \brief VC_CMV_Inspiration_Time_Variables_Reset.
 * */
void VC_CMV_Inspiration_Time_Variables_Reset();
/**
 * \brief VC_CMV_Expiration_Time_Variables_Reset.
 * */
void VC_CMV_Expiration_Time_Variables_Reset();
/**
 * \brief Volume_Not_Acheieved.
 * */
void Volume_Not_Acheieved();
/**
 * \brief Volume_Acheieved.
 * */
void Volume_Acheieved();
/**
 * \brief Reset_Blower_Status.
 * */
void Reset_Blower_Status();
/**
 * \brief LOW_Compliance_DAC_value.
 * */
uint16_t LOW_Compliance_DAC_value(uint8_t);


#endif /* INC_VC_CMV_H_ */
