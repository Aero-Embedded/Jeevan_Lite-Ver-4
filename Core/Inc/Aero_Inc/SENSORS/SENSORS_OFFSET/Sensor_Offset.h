/**
 *******************************************************************************
 * \file Sensor_Offset.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Implementation of Sensor offset calculation.
 *
 * This file contains the Implementation of Sensor offset calculation,
 * including the initialization.
 *******************************************************************************
 **/

#ifndef INC_SENSOR_OFFSET_H_
#define INC_SENSOR_OFFSET_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"


/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

xTaskHandle Sensor_Offset_Handler;    ///< Sensor offset handler task
xTaskHandle One_Time_Handler;         ///< One_Time_Handler task


/**
 * \brief Sensor_Offset_Task.
 * It is used to call the sensor offset function from within this task.
 */
void Sensor_Offset_Task(void *argument);

/**
 * \brief One_Time_Task.
 * It interchanges the phase of the breath cycle.
 */
void One_Time_Task(void *argument);

#endif /* INC_SENSOR_OFFSET_H_ */
