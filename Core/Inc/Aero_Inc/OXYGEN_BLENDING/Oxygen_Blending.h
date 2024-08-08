/**
 *******************************************************************************
 * \file Oxygen_Blending.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of Oxygen_Blending Concept.
 *
 * This file contains the Declaration of Oxygen_Blending Concept within a mode,
 * including the initialization
 *******************************************************************************
 **/

#ifndef INC_OXYGEN_BLENDING_H_
#define INC_OXYGEN_BLENDING_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/


#define O2_Sensor_Pin_Voltage Adc.AdcData[1]          ///< ADC pin for O2 manifold flow
#define Parkar_valve_Signal(Y)  DAC1->DHR12R2=Y;      ///< DAC2 Proportional Valve Control


/**
 * \brief Structure Representing O2 DAC values for modes.
 * it includes OXYGEN sensor offset values,maximum values and also Oxygen dac values.
 */

typedef struct
{
	float    OXYGEN_OFFSET_VAL;      ///< Envitec sensor offset value.
	float    OXYGEN_MAXIMUM_VAL;     ///< Envitec sensor Maximum value.
	uint8_t  Achieved_Oxygen;        ///< Envitec Achieved O2value.
	int      O2_DAC;                 ///< Et tube modes O2 dac value.
	int      CPAP_O2_DAC;            ///< Cpap mode O2 dac value.
	int      PSV_O2_DAC;             ///< Psv mode O2 dac value.
	int      BIPAP_O2_DAC;           ///< Bipap mode O2 dac value.

}Oxygen_parameter;


/**
 *\brief Enumeration for fio2_blending_methods.
 * To Check weather the mode is pressure controlled or volume controlled.
 */

typedef enum
{
	PRESSURE_CONTROLLED,      ///< used for Pccmv,pcsimv,backup pccmv.
	VOLUME_CONTROLLED         ///< used for Vccmv,Vcsimv
}fio2_blending_method;


/**
 *\brief Enumeration for oxygen_blending_status.
 * To Check weather the Oxygen value Reached or not.
 */

typedef enum
{
	OXYGEN_ACHIEVED,            ///< when oxygen Achieved
	OXYGEN_NOT_ACHIEVED         ///< when oxygen not Achieved
}oxygen_blending_status;


uint8_t Check_Oxygen_Count;     ///< Count the oxygen Delivered breath for flow based approach
uint8_t O2_Check;               ///< wait for 5 breath to check the alert
uint16_t Temp_Dac,inc_dac;      ///< used to start O2 value from minimum value
int8_t O2_Flow_Rate;            ///< store O2 flow rate value.

Oxygen_parameter Oxygen_Parameter;               ///< Oxygen Parameter for mode control
fio2_blending_method Fio2_Blending_Method;       ///< Fio2_Blending_Method
oxygen_blending_status Oxygen_Blending_Status;   ///< Oxygen_Blending_Status
uint8_t O2_DAC_AS_PER_FLOW_RATE_Count;           ///< O2_DAC_AS_PER_FLOW_RATE_Count
uint8_t O2_Low_Count;                            ///< Check O2_Low_Count
uint8_t O2_High_Count;                           ///< Check O2_High_Count
float O2_Sensor_Voltage;                         ///< Check O2 Sensor Voltage
uint8_t Maximum_Limit;
uint16_t Last_O2_Dac_Val;                        ///< Store Last O2 Dac value


float Achieved_Oxygen_float;                    ///< Achieved Oxygen value as float

xTaskHandle Oxygen_Blending_Handler;            ///< Oxygen Blending task Handler


/**
 * \brief Oxygen_Blending_Task.
 * */
void Oxygen_Blending_Task(void *argument);




/**
 * \brief Oxygen_Blending.
 * \param FIO2_Val Set fio2 value from mode.
 * */
void Oxygen_Blending(uint8_t);


/**
 * \brief Calculate_O2.
 * \pre None.
 * \param FIO2_Val Set fio2 value from mode.
 * \post None.
 * \return None.
 * */

void Calculate_O2(uint8_t);


/**
 * \brief Calculate_O2.
 * \param FIO2_Val Set fio2 value from mode.
 * \param Flowrate Set Flow rate to achieve a O2.
 * */
void O2_Initial_Parameter(uint8_t,uint8_t);


/**
 * \brief Calculate_O2.
 * \param FIO2_Val Set fio2 value from mode.
 * \param Achieved_Oxygen Achieved Oxygen from previous breathe.
 * */
void Low_Oxygen_Achieved(uint8_t,uint8_t);

/**
 * \brief High_Oxygen_Achieved.
 * \param FIO2_Val Set fio2 value from mode.
 * \param Achieved_Oxygen Achieved Oxygen from previous breathe.
 * */
void High_Oxygen_Achieved(uint8_t,uint8_t);


/**
 * \brief control O2 DAC value.
 * */
void O2_DAC_Control(uint8_t);





void Get_Oxygen_Value(uint8_t);
uint16_t Set_Initial_O2_DAC_VAL_Pressure_Modes(uint8_t);
uint16_t Set_Initial_O2_DAC_VAL_Volume_Modes(uint8_t);
uint16_t Set_Initial_CPAP_O2_DAC_VAL_Pressure_Modes(uint8_t);
void Servo_Position_Volume_Controlled_Mode(uint8_t);
void Servo_Position_Pressure_Controlled_Mode(uint8_t);
void CPAP_Servo_Position_Volume_Controlled_Mode(uint8_t);
void Servo_Info(uint8_t);

#endif /* INC_OXYGEN_BLENDING_H_ */
