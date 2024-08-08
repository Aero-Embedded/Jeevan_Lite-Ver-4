/**
 *******************************************************************************
 * \file mpx5010dp.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief implementation of mpx5010dp O2 pressure sensor integration.
 *
 * This file contains the implementation of mpx5010dp O2 pressure sensor integration,
 * including the initialization.
 *******************************************************************************
 **/


/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include <SENSORS_INTERFACE/ads1115.h>
#include <SENSORS_INTERFACE/constants.h>
#include <SENSORS_INTERFACE/database.h>
#include <SENSORS_INTERFACE/mpx5010dp.h>
#include "math.h"

#include "main.h"
/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/


//double dp_to_oxygen_flow(float dp){
//	dp = dp + (dp*0.07);
//	if(dp==0 || dp<0){
//		return 0;
//	}
//
//	double flow = ((0.000231*dp*dp*dp)-(0.0348*dp*dp)+(2.42*dp)+5.63);
//	if(flow<0){
//		return 0.0;
//	}
//	return flow;
//}


/**
  * @brief  dp_to_oxygen_flow.
  * This function Converting differential pressure to O2 flow from mpx5010 milli volt.
  * @retval double O2 flow value.
  */
double dp_to_oxygen_flow(float dp){

	if(dp==0 || dp<0.05){
		return 0;
	}
	double flow = -1;
	if(dp<0.5){
		flow = ((56.5*dp*dp*dp)-(58.9*dp*dp)+(27.1*dp)+0.101);
	}else{
		dp = dp + (dp*0.07);
		flow = ((0.000231*dp*dp*dp)-(0.0348*dp*dp)+(2.42*dp)+5.63);
	}
	if(flow<0){
		return 0.0;
	}
	return flow;
}


/**
  * @brief  compute_oxygen_flow.
  *         This function Calculate flow crossing in O2 manifold.
  */

void compute_oxygen_flow(){
	float dp = MPX5010DP.value*10;
	int8_t direction_bit = 1;
	if(dp<0){
		direction_bit = -1;
	}
	float flow = dp_to_oxygen_flow(direction_bit*dp);
	GRAPH_VALUES.oxygen_flow = MPX5010DP.smoothing_factor*GRAPH_VALUES.oxygen_flow + (1-MPX5010DP.smoothing_factor)*flow;

	if(GRAPH_VALUES.Maximum_oxygen_flow < GRAPH_VALUES.oxygen_flow)
	{
		GRAPH_VALUES.Maximum_oxygen_flow  =  GRAPH_VALUES.oxygen_flow;
	}
}

/**
  * @brief  Initialize the MPX5010DP sensor.
  *         This function sets initial values for the sensor.
  */
void MPX5010DP_init(){
	MPX5010DP.offset_minimum = 0;
	MPX5010DP.offset_maximum = 425;
	MPX5010DP.supply_voltage = 5000;
	MPX5010DP.sensitivity = 450.00862;
	MPX5010DP.smoothing_factor = 0.35;
	MPX5010DP.previous_reading = 0;
	MPX5010DP.operational_status = 1;
	MPX5010DP.sensor_ready = 0;
	MPX5010DP_calibrate();
}

/**
  * @brief  Calibrate the MPX5010DP sensor.
  *         This function takes multiple readings to determine the sensor offset.
  *         It checks if the offset is within the allowable range.
  */
void MPX5010DP_calibrate(){
	for(int i=0; i<CALIBRATION_COUNT; i++){
		MPX5010DP_read();
		vTaskDelay(1);
		if(i==999)
			xSemaphoreGive(binarysem);
	}
	uint16_t offset = MPX5010DP.current_reading;
	MPX5010DP.previous_reading = 0;
	MPX5010DP.current_reading = 0;
	if(offset<MPX5010DP.offset_minimum && offset>MPX5010DP.offset_maximum){
		MPX5010DP.operational_status = 0;
	}
	MPX5010DP.offset = offset;
	MPX5010DP.sensor_ready = 1;


}

/**
  * @brief  Read data from the MPX5010DP sensor.
  *         This function uses Exponentially Weighted Moving Average filter and calculates the sensor value.
  *         References:
  *         		1. https://hackaday.com/2019/09/06/sensor-filters-for-coders/
  *         		2. https://electronics.stackexchange.com/questions/30370/fast-and-memory-efficient-moving-average-calculation
  *         		3. https://stackoverflow.com/questions/12636613/how-to-calculate-moving-average-without-keeping-the-count-and-data-total
  */
void MPX5010DP_read(){
	float raw_voltage = MPX5010DP_read_adc();
	MPX5010DP.current_reading = (uint16_t)((1-MPX5010DP.smoothing_factor)*MPX5010DP.previous_reading + MPX5010DP.smoothing_factor*raw_voltage);
	MPX5010DP.previous_reading = MPX5010DP.current_reading;
	MPX5010DP.value = (MPX5010DP.current_reading-MPX5010DP.offset)/MPX5010DP.sensitivity;
	compute_oxygen_flow();
}

/**
  * @brief  Perform analog-to-digital value to voltage conversion for the MPX5010DP sensor.
  * @retval Vout (in mV)
  */
float MPX5010DP_read_adc(){
	uint16_t muxPort = ADS1115_MUX_AIN0;
	float result;
    // Call the function
    HAL_StatusTypeDef status = ADS1115_Read(muxPort, &result,&ADS1115_2_I2C_Handler);
    // Check if the function call was successful
    if (status == HAL_OK) {
    	_5010DP_Sensor_ADS1115 = SERVICE_PASS;
        return result;
    }
    _5010DP_Sensor_ADS1115 = SERVICE_FAIL;
	return 0;
}

