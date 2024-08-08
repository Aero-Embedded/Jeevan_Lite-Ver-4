/**
 *******************************************************************************
 * \file Volume_Control.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of inspiration time Volume control mode.
 *
 * This file contains the Declaration inspiration time Volume control mode variables
 *******************************************************************************
 **/

#ifndef INC_VOLUME_CONTROL_H_
#define INC_VOLUME_CONTROL_H_

#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"


/**
 * \brief Structure Controls volume and flow achieved status.
 *
 */
typedef struct
{
	uint8_t  Volume_Stop_Value;      ///< Volume stopping position value
	uint8_t  Flow_Max_Achieved;      ///< maximum flow achieved status
	uint16_t Achieved_Volume;        ///< achieved volume
	uint8_t  LOW_Flow_Max_Achieved;  ///< if low flow achieved status

}volume_control_Task;


/**
 * \brief Structure control  Blower Character..
 */

typedef struct
{
	uint16_t BLOWER_DAC_VAL;                     ///< blower dac value
	uint16_t INITIAL_BLOWER_DAC_VAL;             ///< initial dac value in inspiration time
	uint16_t INITIAL_BLOWER_DAC_VAL_TRIGGER;     ///< initial dac value in inspiration time in vc simv mode
	uint16_t PREVIOUS_INITIAL_BLOWER_DAC_VAL;    ///< previous breath initial dac value in inspiration time
	float Blower_Signal_Voltage;                 ///< blower dac converted as volatge
	uint16_t PEEP_BLOWER_DAC_VAL;                ///< peep maintain blower dac in simv mode
	uint16_t LAST_PEEP_BLOWER_DAC_VAL;           ///< last peep blower dac value
}Volume_mode_blower_control;

/**
 * \brief enumeration for volume achieved status.
 */
typedef enum
{
	VOLUME_NOT_ACHIEVED,     ///< when volume not achieved time.
	VOLUME_ACHIEVED          ///< when volume achieved time.
}volume_achieved_status;


/**
 * \brief enumeration for Flow achieved status.
 */
typedef enum
{
	FLOW_NOT_ACHIEVED,       ///< when Flow not achieved time.
	FLOW_ACHIEVED            ///< when Flow not achieved time.
}flow_achieved_status;

/**
 * \brief enumeration for task delay
 */
typedef enum
{
	One_Millisecond_Delay    =1,          ///< one milli second delay
	Two_Millisecond_Delay    =2,          ///< Two milli second delay
	Ten_Millisecond_Delay    =10,         ///< Ten milli second delay
	Twenty_MilliSecond_Delay =20,         ///< Twenty milli second delay
	Hundred_Millisecond_Delay=100,        ///< hundred milli second delay
	Four_Second_Delay        =4000,       ///< four thousand milli second delay
}Task_Wait_time;

float Flow_Cummulative_Val;       ///< calculate flow average
float Flow_Count;                 ///< calculate flow average
int16_t Avg_Flow;                 ///< calculate flow average

volume_control_Task Volume_control_Task;                 ///< Structure variable
Volume_mode_blower_control Volume_Mode_Blower_control;   ///< Structure variable

volume_achieved_status Volume_Achieved_Status;   ///< Enumeration variable
flow_achieved_status Maximum_Flow_Achieved;      ///< Enumeration variable

xTaskHandle Volume_Control_Task_Handler;      ///< Task handler variable
void Volume_Control_Task (void *argument);    ///< volume control task

/**
 * \brief Volume_Control.
 * */
void Volume_Control(uint16_t,uint8_t,uint8_t);

/**
 * \brief Flow_Restriction_Control.
 * */
void Flow_Restriction_Control(uint16_t,uint8_t,uint8_t);

/**
 * \brief VOLUME_STOP_POINT.
 * */
void VOLUME_STOP_POINT(uint16_t,uint16_t);
/**
 * \brief TUNE_INITIAL_DAC_VAL.
 * */
uint16_t TUNE_INITIAL_DAC_VAL(uint16_t, uint8_t ,uint8_t );
/**
 * \brief flow_Rate_control.
 * */
void flow_Rate_control(uint8_t);
/**
 * \brief Cut_Off_Region.
 * */
void Cut_Off_Region(uint8_t);

#endif /* INC_VOLUME_CONTROL_H_ */
