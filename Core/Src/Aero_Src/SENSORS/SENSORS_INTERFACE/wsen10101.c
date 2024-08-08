/**
 *******************************************************************************
 * \file wsen10101.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of Worth sensor integration.
 *
 * This file contains the Declaration of Worth sensor integration,
 * including the initialization
 *******************************************************************************
 **/


/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include <SENSORS_INTERFACE/ads1115.h>
#include <SENSORS_INTERFACE/constants.h>
#include <SENSORS_INTERFACE/database.h>
#include <SENSORS_INTERFACE/wsen10101.h>
#include "math.h"

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

double dp_to_flow_wsen(float dp){
	if(dp<0.01){
		return 0.0;
	}
	return ((-3.65*dp*dp) + (49.5*dp) - 3.4);
	//return ((0.1512*dp*dp*dp) - (3.3424*dp*dp) + (41.657*dp));
}


/**
  * @brief  dp_to_flow_wsen_1 WSEN10101 sensor.
  * This function calculates the differential pressure value.
  */

double dp_to_flow_wsen_1(float dp){

	if(dp<0.01){
		return 0.0;
	}

	return ((0.72*dp*dp*dp) - (8.01*dp*dp) + (56.7*dp) - 6.18);
	//return ((-2.44*dp*dp) + (40.2*dp) + 0.448);
}

/**
  * @brief  compute_flow_wsen WSEN10101 sensor.
  * This function converts the differential pressure (dp) value into flow rate.
  */

void compute_flow_wsen(){
	float dp = WSEN10101.value * 10;
	int8_t direction_bit = 1;
	if(dp<0){
		direction_bit = -1;
	}
	float flow = dp_to_flow_wsen(direction_bit*dp)*direction_bit;
	float flow1 = dp_to_flow_wsen_1(direction_bit*dp)*direction_bit;
	GRAPH_VALUES.flow = (int16_t)round(WSEN10101.smoothing_factor*GRAPH_VALUES.flow + (1-WSEN10101.smoothing_factor)*flow);
	GRAPH_VALUES.oxygen_flow = (int16_t)round(WSEN10101.smoothing_factor*GRAPH_VALUES.oxygen_flow + (1-WSEN10101.smoothing_factor)*flow1);
	GRAPH_VALUES.volume += (int16_t)((GRAPH_VALUES.flow/60)*FLOW_DEL_T);
	if(GRAPH_VALUES.volume>VOLUME_MAX){
		GRAPH_VALUES.volume = 0;
	}
}


/**
  * @brief  Initialize the WSEN10101 sensor.
  *         This function sets initial values for the sensor.
  */
void WSEN10101_init(){
	WSEN10101.offset_minimum = 2250;
	WSEN10101.offset_maximum = 2750;
	WSEN10101.supply_voltage = 5000;
	WSEN10101.sensitivity = 0.5;
	WSEN10101.smoothing_factor = 0.05;
	WSEN10101.previous_reading = 0;
	WSEN10101.operational_status = 1;
	WSEN10101.sensor_ready = 0;
	WSEN10101_calibrate();
}

/**
  * @brief  Calibrate the WSEN10101 sensor.
  *         This function takes multiple readings to determine the sensor offset.
  *         It checks if the offset is within the allowable range.
  */
void WSEN10101_calibrate(){
	for(int i=0; i<CALIBRATION_COUNT; i++){
		WSEN10101_read();
	}
	uint16_t offset = WSEN10101.current_reading;
	WSEN10101.previous_reading = 0;
	WSEN10101.current_reading = 0;
	if(offset<WSEN10101.offset_minimum && offset>WSEN10101.offset_maximum){
		WSEN10101.operational_status = 0;
	}
	WSEN10101.offset = offset;
	WSEN10101.sensor_ready = 1;
}

/**
  * @brief  Read data from the WSEN10101 sensor.
  *         This function uses Exponentially Weighted Moving Average filter and calculates the sensor value.
  *         References:
  *         		1. https://hackaday.com/2019/09/06/sensor-filters-for-coders/
  *         		2. https://electronics.stackexchange.com/questions/30370/fast-and-memory-efficient-moving-average-calculation
  *         		3. https://stackoverflow.com/questions/12636613/how-to-calculate-moving-average-without-keeping-the-count-and-data-total
  */
void WSEN10101_read(){
	float sum = 0, raw_voltage=0;
	raw_voltage = WSEN10101_read_adc();
	for(int i=0;i<15;i++){
		raw_voltage = (1-WSEN10101.smoothing_factor)*raw_voltage + WSEN10101.smoothing_factor*WSEN10101_read_adc();
		sum+= raw_voltage;
	}
	WSEN10101.current_reading = (uint16_t)((1-WSEN10101.smoothing_factor)*WSEN10101.previous_reading + WSEN10101.smoothing_factor*sum/15);
	WSEN10101.previous_reading = WSEN10101.current_reading;
	WSEN10101.value = round(WSEN10101.current_reading-WSEN10101.offset)*WSEN10101.sensitivity/1000;
	compute_flow_wsen();
}

/**
  * @brief  Perform analog-to-digital value to voltage conversion for the WSEN10101 sensor.
  * @retval Vout (in mV)
  */
float WSEN10101_read_adc(){
	uint16_t muxPort = ADS1115_MUX_AIN3;
	float result;
    // Call the function
    HAL_StatusTypeDef status = ADS1115_Read(muxPort, &result, &ADS1115_1_I2C_Handler);
    // Check if the function call was successful
    if (status == HAL_OK) {
        return round(result);
    }
	return -1;
}

