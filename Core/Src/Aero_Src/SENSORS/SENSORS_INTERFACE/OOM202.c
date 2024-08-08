/**
 *******************************************************************************
 * \file oom202.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Implementation of Envitec O2 sensor integration.
 *
 * This file contains the Implementation of Oxygen_Blending Concept within a mode,
 * including the initialization
 *******************************************************************************
 **/


/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/


#include <SENSORS_INTERFACE/ads1115.h>
#include <SENSORS_INTERFACE/constants.h>
#include <SENSORS_INTERFACE/database.h>
#include <SENSORS_INTERFACE/oom202.h>
#include "math.h"

#include "main.h"


/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/



/**
  * @brief  Compute FiO2 Percentage
  *         Function maps Sensor voltage and FiO2 percentage.
  */
float compute_fio(uint16_t voltage){
	return (((voltage - OOM202.offset_minimum)*79.2)/(OOM202.offset_maximum - OOM202.offset_minimum))+20.8;
}

/**
  * @brief  Initialize the OOM202 sensor.
  *         This function sets initial values for the sensor.
  */
void OOM202_init(){
	OOM202.smoothing_factor = 0.3;
	OOM202.offset_maximum = 70;//842;
	OOM202.offset_minimum = 13;//137;
	OOM202.sensitivity = 0.1316;
	OOM202.supply_voltage = 3300;
	OOM202.previous_reading = 0;
	OOM202.operational_status = 1;
	OOM202.sensor_ready = 0;

}

/**
  * @brief  Calibrate the OOM202 sensor.
  *         This function takes multiple readings to determine the sensor offset.
  *         It checks if the offset is within the allowable range.
  */
void OOM202_calibrate(uint8_t level){
	for(int i=0; i<CALIBRATION_COUNT; i++){
		OOM202_read();
		vTaskDelay(1);

	}
	uint16_t offset = OOM202.current_reading;
	OOM202.previous_reading = 0;
	OOM202.current_reading = 0;
	if(level){
		OOM202.offset_maximum = offset;
	}else{
		OOM202.offset_minimum = offset;
	}
	OOM202.sensor_ready = 1;
	OOM202_calculate_efficiency();
}

/**
  * @brief  Calculate Efficiency of the OOM202 sensor..
  *         The function uses the minimum and maximum offset to calculate the Efficiency of the sensor
  */
void OOM202_calculate_efficiency(){
	OOM202.efficiency = (uint8_t)round(OOM202.sensitivity*(OOM202.offset_maximum - OOM202.offset_minimum));
	if(OOM202.efficiency <= 75){
		OOM202.operational_status = 0;
	}
}

/**
  * @brief  Read data from the OOM202 sensor.
  *         This function uses Exponentially Weighted Moving Average filter and calculates the sensor value.
  *         References:
  *         		1. https://hackaday.com/2019/09/06/sensor-filters-for-coders/
  *         		2. https://electronics.stackexchange.com/questions/30370/fast-and-memory-efficient-moving-average-calculation
  *         		3. https://stackoverflow.com/questions/12636613/how-to-calculate-moving-average-without-keeping-the-count-and-data-total
  */

void OOM202_read(){
	float raw_voltage = OOM202_read_adc();
	OOM202.current_reading = (uint16_t)((1-OOM202.smoothing_factor)*OOM202.previous_reading + (OOM202.smoothing_factor*raw_voltage));
	OOM202.previous_reading = OOM202.current_reading;
	OOM202.value = compute_fio(OOM202.current_reading);

	if(OOM202.value > 100)
		OOM202.value = 100;
	if(OOM202.value <20)
		OOM202.value = 20;

	GRAPH_VALUES.oxygen = (int8_t) OOM202.value;
}

/**
  * @brief  Perform analog-to-digital value to voltage conversion for the OOM202 sensor.
  * @retval Vout (in mV)
  */
float OOM202_read_adc(){
	uint16_t muxPort = ADS1115_MUX_AIN0;
	float result;
    // Call the function
    HAL_StatusTypeDef status = ADS1115_Read(muxPort,&result, &ADS1115_3_I2C_Handler);
    // Check if the function call was successful
    if (status == HAL_OK) {
    	ENVITEC_Sensor_ADS1115 = SERVICE_PASS;
        return result;
    }
    ENVITEC_Sensor_ADS1115 = SERVICE_FAIL;
	return 0;
}
