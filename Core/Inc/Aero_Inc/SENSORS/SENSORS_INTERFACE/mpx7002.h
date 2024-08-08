/**
 *******************************************************************************
 * \file mpx7002.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of mpx7002 Sensor integration.
 *
 * This file contains the Declaration of mpx7002 Sensor integration,
 * including the initialization.
 *******************************************************************************
 **/

#ifndef INC_JEEVANLITE_PERIPHERALS_SENSORS_MPX7002_H_
#define INC_JEEVANLITE_PERIPHERALS_SENSORS_MPX7002_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include <SENSORS_INTERFACE/sensor.h>
#include <SENSORS_INTERFACE/utilities.h>
#include "main.h"
/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

SENSOR MPX7002;
KALMAN_CONSTANTS IIR;




/**
  * \brief  Initialize the MPX7002 sensor.
  */
void MPX7002_init();

/**
  * \brief  Calibrate the MPX7002 sensor.
  */
void MPX7002_calibrate();

/**
  * \brief  Read data from the MPX7002 sensor.
  */
void MPX7002_read();

/**
  * \brief  Perform analog-to-digital conversion for the MPX7002 sensor.
  */
float MPX7002_read_adc();

#endif /* INC_JEEVANLITE_PERIPHERALS_SENSORS_MPX7002_H_ */
