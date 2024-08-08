/**
 *******************************************************************************
 * \file digital_filters.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Implementation of Sensor Filter.
 *
 * This file contains the Implementation of Filter for Sensor values.
 *******************************************************************************
 **/



#include <SENSORS_INTERFACE/utilities.h>
#include "stdint.h"
#include "math.h"


/**
  * @brief  kalman_filter
  *         function neglects the noise in the sensor.
  * */
float kalman_filter(float mea, float *err_estimate, float err_measure, float q, int8_t last_estimate){
	  float kalman_gain = *err_estimate/(*err_estimate + err_measure);
	  float current_estimate = last_estimate + kalman_gain * (mea - last_estimate);
	  *err_estimate =  (1.00 - kalman_gain)*(*err_estimate) + fabsf(last_estimate-current_estimate)*q;
	  return current_estimate;
}
