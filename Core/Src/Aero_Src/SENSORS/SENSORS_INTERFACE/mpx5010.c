/**
 *******************************************************************************
 * \file mpx5010.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Implementation of 5010 Pressure sensor Configuration.
 *
 * This file contains the Implementation of 5010 Pressure sensor, including
 * the initialization
 *******************************************************************************
 **/


/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include <SENSORS_INTERFACE/constants.h>
#include <SENSORS_INTERFACE/database.h>
#include <SENSORS_INTERFACE/mpx5010.h>
#include "math.h"
#include "main.h"

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

/**
  * @brief  Initialize the MPX5010 sensor.
  *         This function sets initial values for the sensor.
  */
void MPX5010_init()
{
	MPX5010.offset_minimum = 0;
	MPX5010.offset_maximum = 425;
	MPX5010.supply_voltage = 5000;
	MPX5010.sensitivity = 0.02266;
	MPX5010.smoothing_factor = 0.35;
	MPX5010.previous_reading = 0;
	MPX5010.operational_status = 1;
	MPX5010.sensor_ready = 0;
	MPX5010_calibrate();
}

/**
  * @brief  Calibrate the MPX5010 sensor.
  *         This function takes multiple readings to determine the sensor offset.
  *         It checks if the offset is within the allowable range.
  */
void MPX5010_calibrate()
{
	for(int i=0; i<CALIBRATION_COUNT; i++)
	{
		MPX5010_read();
		vTaskDelay(1);
	}
	uint16_t offset = MPX5010.current_reading;
	MPX5010.previous_reading = 0;
	MPX5010.current_reading = 0;
	if(offset<MPX5010.offset_minimum && offset>MPX5010.offset_maximum){
		MPX5010.operational_status = 0;
	}
	MPX5010.offset = offset;
	MPX5010.sensor_ready = 1;
}

/**
  * @brief  Read data from the MPX5010 sensor.
  *         This function uses Exponentially Weighted Moving Average filter and calculates the sensor value.
  *         References:
  *         		1. https://hackaday.com/2019/09/06/sensor-filters-for-coders/
  *         		2. https://electronics.stackexchange.com/questions/30370/fast-and-memory-efficient-moving-average-calculation
  *         		3. https://stackoverflow.com/questions/12636613/how-to-calculate-moving-average-without-keeping-the-count-and-data-total
  */

void MPX5010_read(){
	float raw_voltage = MPX5010_read_adc();
	MPX5010.current_reading = (uint16_t)((1-MPX5010.smoothing_factor)*MPX5010.previous_reading + MPX5010.smoothing_factor*raw_voltage);
	MPX5010.previous_reading = MPX5010.current_reading;
	MPX5010.value = (MPX5010.current_reading - MPX5010.offset)*MPX5010.sensitivity;
	GRAPH_VALUES.pressure = (uint8_t) round(MPX5010.value);
}

/**
  * @brief  Perform analog-to-digital value to voltage conversion for the MPX5010 sensor.
  * @retval Vout (in mV)
  */
float MPX5010_read_adc(){
	return (Adc.AdcData[0] * MPX5010.supply_voltage)/ADC_RESOLUTION;
}
