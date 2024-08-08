/**
 *******************************************************************************
 * \file Two_Milli_Second_Task.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of time based concept.
 *
 * This file contains the Declaration of time based concept.
 *******************************************************************************
 **/

#ifndef INC_TWO_MILLI_SECOND_TASK_H_
#define INC_TWO_MILLI_SECOND_TASK_H_


#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"



int Current_Pressure,Over_Pressure_Count;

xTaskHandle Time_Trace_Handler;           ///< task variable

/**
 *\brief Time_Trace_Task.
 * This task controls all time-based functions.
 */
void Time_Trace_Task(void *argument);


/**
 *\brief INSPIRATION_TIME_TRACE.
 */
void INSPIRATION_TIME_TRACE();
/**
 *\brief EXPIRATION_TIME_TRACE.
 */
void EXPIRATION_TIME_TRACE();
/**
 *\brief BREATH_COUNT.
 */
void BREATH_COUNT();
/**
 *\brief PC_SIMV_BLOWER_WAIT_TIME_TRACE.
 */
void PC_SIMV_BLOWER_WAIT_TIME_TRACE();
/**
 *\brief VC_SIMV_BLOWER_WAIT_TIME_TRACE.
 */
void VC_SIMV_BLOWER_WAIT_TIME_TRACE();
/**
 *\brief PSV_APNEA_COUNTER_TRACE.
 */
void PSV_APNEA_COUNTER_TRACE();
/**
 *\brief BIPAP_APNEA_COUNTER_TRACE.
 */
void BIPAP_APNEA_COUNTER_TRACE();
/**
 *\brief CPAP_APNEA_COUNTER_TRACE.
 */
void CPAP_APNEA_COUNTER_TRACE();
/**
 *\brief APRV_APNEA_COUNTER_TRACE.
 */
void APRV_APNEA_COUNTER_TRACE();
/**
 *\brief INITIAL_WAIT_TIME.
 */
void INITIAL_WAIT_TIME();

#endif /* INC_TWO_MILLI_SECOND_TASK_H_ */
