/**
 *******************************************************************************
 * \file PIP_Control.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration to control a PIP.
 *
 * This file contains the Declaration to control a PIP.
 *******************************************************************************
 **/

#ifndef INC_PIP_CONTROL_H_
#define INC_PIP_CONTROL_H_


#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"

/**
 * \brief The structure contains PIP control within the core logic variables
 */
typedef struct
{
	int16_t PIP_Control_Task_Delay;
	int16_t PIP_Control_Event_Occured;
	int16_t PIP_Acheived_Time_Ms;
	int16_t Increase_Each_DAC_SET_CONST;
	uint8_t  Early_Acheived_Ramp_Time_Percentage;
	uint8_t  Last_Early_Acheived_Ramp_Time_Percentage;
	uint8_t  Low_PIP_Count;
	uint16_t  Slowly_Acheived_Ramp_Time_Percentage;

}pip_control_Task;

/**
 * \brief The structure contains blower dac signals for PIP and peep.
 */
typedef struct
{
	uint16_t BLOWER_DAC_VAL;
	uint16_t LAST_BLOWER_DAC_VAL;
	float    BLOWER_VOLTAGE_SIGNAL;
	uint16_t BLOWER_ENDING_DAC_SIGNAL;
	float Blower_Signal_Voltage;
	uint16_t PEEP_BLOWER_DAC_VAL;
	uint16_t LAST_PEEP_BLOWER_DAC_VAL;

}Pressure_mode_blower_control;


/**
 * \brief Enumeration of the status regarding PIP achievement.
 */
typedef enum
{
	PIP_NOT_ACHEIVED,
	PIP_ACHEIVED
}pip_status;


/**
 * \brief Enumeration of the status regarding the PIP achieved time.
 */
typedef enum
{
	READ_TIME_OPEN,
	READ_TIME_CLOSE
}Read_time_vs_pressure;


/**
 * \brief Enumeration of the status indicating whether the Set PIP value has been achieved or not.
 */
typedef enum
{
	SET_PIP_VALUE_NOT_ACHEIVED,
	SET_PIP_VALUE_ACHEIVED
}Set_pip_Status;

/**
 * \brief Enumeration of whether the expiratory valve needs a delay or not.
 */
typedef enum
{
	NO,
	YES
}Need_pid_Task_Delay;

/**
 * \brief Enumeration of whether the expiratory valve needs a lock or not.
 */
typedef enum
{
	T_OPEN,
	T_LOCK
}Tune_pid_Task_Delay;

pip_control_Task PIP_control_Task;                             ///< Structure variable
pip_status PIP_Status;                                         ///< Enumeration variable
Read_time_vs_pressure Read_Time_vs_Pressure;                   ///< Enumeration variable
Set_pip_Status Set_PIP_Status;                                 ///< Enumeration variable
Need_pid_Task_Delay Need_PID_Task_Delay;                       ///< Enumeration variable
Tune_pid_Task_Delay Tune_PID_Task_Delay;                       ///< Enumeration variable
Pressure_mode_blower_control Pressure_Mode_blower_control;     ///< Structure variable
uint8_t count1,count2;


xTaskHandle PIP_Control_Task_Handler;                          ///< Task variable

/**
 * \brief PIP_Control_Task.
 * */
void PIP_Control_Task(void *argument);

/**
 * \brief PIP_Control.
 * */
void PIP_Control(uint8_t);

/**
 * \brief PIP_ACHEIVED_EARLY.
 * */
void PIP_ACHEIVED_EARLY(uint8_t,uint16_t);

/**
 * \brief PIP_ACHEIVED_SLOWLY.
 * */
void PIP_ACHEIVED_SLOWLY(uint16_t);

/**
 * \brief PIP_ACHEIVED_NORMALY.
 * */
void PIP_ACHEIVED_NORMALY();

/**
 * \brief _PIP_NOT_ACHEIVED.
 * */
void _PIP_NOT_ACHEIVED();

/**
 * \brief BLOWER_ENDING_DAC_SIGNAL_TUNE.
 * */
void BLOWER_ENDING_DAC_SIGNAL_TUNE(uint8_t,uint32_t,uint8_t);

/**
 * \brief INCREASE_EACH_DAC_SET_CONST.
 * */
void INCREASE_EACH_DAC_SET_CONST(uint8_t,uint16_t);


#endif /* INC_PIP_CONTROL_H_ */
