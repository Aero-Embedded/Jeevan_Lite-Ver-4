/******************************************************************************/
/*																			  */
/* Project	    : JeevanLite			     		                          */
/*																			  */
/*----------------------------------------------------------------------------*/
/*                                                                 			  */
/* @file		: sensor.h                                 	        	  */
/* @author		: GOWTHAMAAN, ABS2020E007                                 	  */
/* @date		: 21-11-2023, 16:16											  */
/*                                                           				  */
/*----------------------------------------------------------------------------*/
/* @brief       : Sensor Structure           		  				  		  */
/******************************************************************************/

#ifndef INC_JEEVANLITE_PERIPHERALS_SENSORS_SENSOR_H_
#define INC_JEEVANLITE_PERIPHERALS_SENSORS_SENSOR_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include "stdint.h"

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

/**
 * @brief Sensor - Structure
 *
 * This structure represents a generic sensor.
 */
typedef struct
{
    uint16_t offset;              /**< Sensor offset value */
    uint16_t offset_minimum;      /**< Minimum allowable offset */
    uint16_t offset_maximum;      /**< Maximum allowable offset */
    uint16_t supply_voltage;      /**< Supply voltage for the sensor */
    float value;               	  /**< Current sensor reading value */
    uint16_t current_reading;     /**< Current ADC reading from the sensor */
    uint16_t previous_reading;    /**< Previous ADC reading from the sensor */
    uint8_t operational_status;   /**< Operational status of the sensor */
    uint8_t sensor_ready;         /**< Flag indicating if the sensor is ready */
    float sensitivity;            /**< Sensitivity of the sensor */
    float smoothing_factor;       /**< Smoothing factor for sensor readings */
    uint8_t efficiency;			  /**< Sensor Efficiency*/
} SENSOR;


#endif /* INC_JEEVANLITE_PERIPHERALS_SENSORS_SENSOR_H_ */
