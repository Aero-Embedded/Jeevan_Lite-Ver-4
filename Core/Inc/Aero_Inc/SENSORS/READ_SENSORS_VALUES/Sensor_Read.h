/**
 *******************************************************************************
 * \file Sensor_Read.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of All sensor data's reading area.
 *
 * This file contains the Declaration of All sensor data's reading area.
 *******************************************************************************
 **/

#ifndef INC_SENSOR_READ_H_
#define INC_SENSOR_READ_H_



#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"


/**
 * \brief READ_SENSOR_TASK.
 * This Function calls all sensor Reading functions for every 2 milli-seconds.
 * */
void READ_SENSOR_TASK();


/**
 * \brief CALIBRATE_SENSOR_TASK.
 * This function is called from sensor offset task.
 * */
void CALIBRATE_SENSOR_TASK();



/**
 * \brief MPX5010_read.
 * This function is Calculate the runtime pressure value in cmh20.
 * */

void MPX5010_read();


/**
 * \brief MPX7002_read.
 * This function is Calculate the runtime Flow value in L/min.
 * */
void MPX7002_read();


/**
 * \brief MPX5010DP_read.
 * This function is Calculate the runtime O2 Flow value in L/min.
 * */
void MPX5010DP_read();


/**
 * \brief MPX5010_init.
 * This function is initialize pressure sensor.
 * */
void MPX5010_init();


/**
 * \brief MPX7002_init.
 * This function is initialize flow sensor.
 * */
void MPX7002_init();

/**
 * \brief MPX5010DP_init.
 * This function is  initialize O2 flow sensor.
 * */
void MPX5010DP_init();

xTaskHandle Sensor_Read_Handler;        ///> Sensor read handler Task


/**
 * \brief Sensor_Read_Task.
 * This task call READ_SENSOR_TASK function.
 * */
void Sensor_Read_Task(void *argument);


#endif /* INC_SENSOR_READ_H_ */
