/**
 *******************************************************************************
 * \file wsen10101.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of Worth sensor integration.
 *
 * This file contains the Declaration of Worth sensor integration,
 * including the initialization
 *******************************************************************************
 **/

#ifndef INC_JEEVANLITE_PERIPHERALS_SENSORS_WSEN10101_H_
#define INC_JEEVANLITE_PERIPHERALS_SENSORS_WSEN10101_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include <SENSORS_INTERFACE/sensor.h>

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

/**
  * @brief  WSEN10101 Sensor - Defined as a generic sensor
  */

SENSOR WSEN10101;

/**
  * @brief  Initialize the WSEN10101 sensor.
  */
void WSEN10101_init();

/**
  * @brief  Calibrate the WSEN10101 sensor.
  */
void WSEN10101_calibrate();

/**
  * @brief  Read data from the WSEN10101 sensor.
  */
void WSEN10101_read();

/**
  * @brief  Perform analog-to-digital conversion for the WSEN10101 sensor.
  */
float WSEN10101_read_adc();

#endif /* INC_JEEVANLITE_PERIPHERALS_SENSORS_WSEN10101_H_ */
