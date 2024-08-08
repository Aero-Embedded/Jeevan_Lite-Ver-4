/**
 *******************************************************************************
 * \file mpx7002.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Implementation of mpx7002 Sensor integration.
 *
 * This file contains the Implementation of mpx7002 Sensor integration,
 * including the initialization.
 *******************************************************************************
 **/


/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include <SENSORS_INTERFACE/ads1115.h>
#include <SENSORS_INTERFACE/constants.h>
#include <SENSORS_INTERFACE/database.h>
#include <SENSORS_INTERFACE/mpx7002.h>
#include "math.h"
#include "main.h"
/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/


uint16_t ADS1115_7002_FLOW_SENSOR_OFFSET;            //new
void Get_AW_Flow_ADS1115_OFFSET();                   //new
void Trigger_Flow();                                 //new


/**
  * @brief  compute_flow.
  * This function is used calculate dp to flow of 2002 sensor.
  */

double dp_to_flow(float dp){
	if(dp<0.1){
		return 0.0;
	}
	if(dp>1){
		dp = dp + (dp*0.05);
	}
	double flow = ((0.72*dp*dp*dp) - (8.01*dp*dp) + (56.7*dp) - 6.18);
	return flow;
}


/**
  * @brief  compute_flow.
  * This function is used for finally calculate Proximal sensor flow and volume.
  */

void compute_flow(){

	float Flow_Volume;
	int16_t temp_dp = (int16_t) (MPX7002.value * 1000.0);
	double dp = temp_dp/100.0;
	int8_t direction_bit = 1;
	if(dp<0){
		direction_bit = -1;
	}
	GRAPH_VALUES.flow = round(dp_to_flow(direction_bit*dp)*direction_bit);
	if(GRAPH_VALUES.flow> Max_Flow)
	{
		Max_Flow= GRAPH_VALUES.flow;
	}
	GRAPH_VALUES.volume += (((float)GRAPH_VALUES.flow/60)*FLOW_DEL_T);
	if(GRAPH_VALUES.volume>VOLUME_MAX){
		GRAPH_VALUES.volume = 0;
	}
	if(Volume_max <GRAPH_VALUES.volume)
	{
		Volume_max=GRAPH_VALUES.volume;
	}
}


/**
  * @brief  Initialize the MPX7002 sensor.
  *         This function sets initial values for the sensor.
  */
void MPX7002_init(){
	MPX7002.offset_minimum = 2250;
	MPX7002.offset_maximum = 2750;
	MPX7002.supply_voltage = 5000;
	MPX7002.sensitivity = 1;
	MPX7002.smoothing_factor = 0.15;
	MPX7002.previous_reading = 0;
	MPX7002.operational_status = 1;
	MPX7002.sensor_ready = 0;
	IIR.error_measurement = 2;
	IIR.error_estimate = 2;
	IIR.process_variance = 0.1;
	MPX7002_calibrate();
}

/**
  * @brief  Calibrate the MPX7002 sensor.
  *         This function takes multiple readings to determine the sensor offset.
  *         It checks if the offset is within the allowable range.
  */
void MPX7002_calibrate(){
	for(int i=0; i<CALIBRATION_COUNT; i++){
		MPX7002_read();
		Get_AW_Flow_ADS1115_OFFSET();                 //new
		vTaskDelay(1);
	}
	uint16_t offset = MPX7002.current_reading;
	MPX7002.previous_reading = 0;
	MPX7002.current_reading = 0;
	if(offset<MPX7002.offset_minimum && offset>MPX7002.offset_maximum){
		MPX7002.operational_status = 0;
	}
	MPX7002.offset = offset;
	MPX7002.sensor_ready = 1;
}

/**
  * @brief  Read data from the MPX7002 sensor.
  *         This function uses Exponentially Weighted Moving Average filter and calculates the sensor value.
  *         References:
  *         		1. https://hackaday.com/2019/09/06/sensor-filters-for-coders/
  *         		2. https://electronics.stackexchange.com/questions/30370/fast-and-memory-efficient-moving-average-calculation
  *         		3. https://stackoverflow.com/questions/12636613/how-to-calculate-moving-average-without-keeping-the-count-and-data-total
  */
void MPX7002_read(){
	float sum = 0, raw_voltage=0;
	raw_voltage = MPX7002_read_adc();

	Trigger_Flow();

	sum+= raw_voltage;
	MPX7002.current_reading = (uint16_t)((1-MPX7002.smoothing_factor)*MPX7002.previous_reading + MPX7002.smoothing_factor*sum/1);
	MPX7002.previous_reading = MPX7002.current_reading;
	MPX7002.value = (MPX7002.current_reading-MPX7002.offset)*MPX7002.sensitivity/1000;
	compute_flow();
}

/**
  * @brief  Perform analog-to-digital value to voltage conversion for the MPX7002 sensor.
  * @retval Vout (in mV)
  */
float MPX7002_read_adc(){
	uint16_t muxPort = ADS1115_MUX_AIN0;
	float result;
    // Call the function
    HAL_StatusTypeDef status = ADS1115_Read(muxPort, &result, &ADS1115_1_I2C_Handler);
    // Check if the function call was successful
    if (status == HAL_OK) {
    	Proximal_Sensor_ADS1115  = SERVICE_PASS;
        return round(result);
    }
 	Proximal_Sensor_ADS1115 =SERVICE_FAIL;
	return -1;
}

/**
  * @brief  Trigger_Flow.
  * this function is used for find flow trigger accuracy.
  */

void Trigger_Flow()                                    //new
{
	Raw_volt_Flow_Trigger_Temp           = (Trigger_Ads1115_Raw_Adc - ADS1115_7002_FLOW_SENSOR_OFFSET); //14450
	Raw_volt_Flow_Trigger                = (uint16_t)((float) Raw_volt_Flow_Trigger_Temp/10);
	switch(Raw_volt_Flow_Trigger)
	{
	    case 1:
		    FLOW_TRIGGER  =1;
		break;
	    case 2:
	    	FLOW_TRIGGER  =2;
	    break;
	    case 3:
	    	FLOW_TRIGGER  =3;
		break;
		case 4:
			FLOW_TRIGGER  =4;
		break;
	    case 5:
	    	FLOW_TRIGGER  =5;
	    break;
	    case 6:
	    	FLOW_TRIGGER  =6;
	    break;
	    case 7:
	    	FLOW_TRIGGER  =7;
		break;
		case 8:
			FLOW_TRIGGER  =8;
		break;
	    case 9:
	    	FLOW_TRIGGER  =9;
		break;
		case 10:
			FLOW_TRIGGER  =10;
		break;
		case 11:
			FLOW_TRIGGER  =11;
		break;
		case 12:
			FLOW_TRIGGER  =12;
		break;
		case 13:
			FLOW_TRIGGER  =13;
		break;
		case 14:
			FLOW_TRIGGER  =14;
		break;
		case 15:
			FLOW_TRIGGER  =15;
		break;
		case 16:
			FLOW_TRIGGER  =16;
		break;
		case 17:
			FLOW_TRIGGER  =17;
		break;
		case 18:
			FLOW_TRIGGER  =18;
		break;
		case 19:
			FLOW_TRIGGER  =19;
		break;
		case 20:
			FLOW_TRIGGER  =20;
		break;
		default:
			FLOW_TRIGGER  = 0;
		break;


	}

}

/**
  * @brief  Get_AW_Flow_ADS1115_OFFSET.
  * This function is used for ADS1115 RAW volt offset.
  */

void Get_AW_Flow_ADS1115_OFFSET()
{
	ADS1115_7002_FLOW_SENSOR_OFFSET = Trigger_Ads1115_Raw_Adc;
}
