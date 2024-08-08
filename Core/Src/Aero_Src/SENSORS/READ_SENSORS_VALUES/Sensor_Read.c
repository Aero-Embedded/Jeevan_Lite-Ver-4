/**
 *******************************************************************************
 * \file Sensor_Read.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Implementation of All sensor data's reading area.
 *
 * This file contains the Implementation of All sensor data's reading area.
 *******************************************************************************
 **/


#include "Sensor_Read.h"



uint32_t Blower_Wait_Time_Calib=0;             ///< never used
uint8_t Calibration_Start=0;                   ///< never used

controlsys Parker_valve_tune = { 10,30, 0, 0.02, 1500, 3000, 1500, 3000, 0.002 };   ///< initialization for pid concept

/**
 * @brief Sensor_Read_Task.
 * This task call READ_SENSOR_TASK function.
 * */

void Sensor_Read_Task(void *argument)
{
	while(1)
	{
		READ_SENSOR_TASK();
		vTaskDelay(Two_Millisecond_Delay);
	}
}

/**
 * @brief READ_SENSOR_TASK.
 * This Function calls all sensor Reading functions for every 2 milli-seconds.
 * */
void READ_SENSOR_TASK()
{
		MPX5010_read();
		MPX7002_read();
		MPX5010DP_read();
		OOM202_read();
		Read_Battery_Voltage();
}

/**
 * @brief CALIBRATE_SENSOR_TASK.
 * This function is called from sensor offset task.
 * */

void CALIBRATE_SENSOR_TASK()
{
	controlsys_Init(&Parker_valve_tune);
	MPX5010_init();
	MPX7002_init();
	MPX5010DP_init();
	OOM202_init();
}

