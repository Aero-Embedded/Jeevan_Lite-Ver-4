/**
 *******************************************************************************
 * \file mpx5010.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of 5010 Pressure sensor Configuration.
 *
 * This file contains the Declaration of 5010 Pressure sensor, including
 * the initialization
 *******************************************************************************
 **/

#ifndef INC_JEEVANLITE_PERIPHERALS_SENSORS_MPX5010_H_
#define INC_JEEVANLITE_PERIPHERALS_SENSORS_MPX5010_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include <SENSORS_INTERFACE/sensor.h>

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

/**
  * \brief  MPX5010 Sensor - Defined as a generic sensor
  */

SENSOR MPX5010;

/**
  * \brief  Initialize the MPX5010 sensor.
  */
void MPX5010_init();

/**
  * \brief  Calibrate the MPX5010 sensor.
  */
void MPX5010_calibrate();

/**
  * \brief  Read data from the MPX5010 sensor.
  */
void MPX5010_read();

/**
  * \brief  Perform analog-to-digital conversion for the MPX5010 sensor.
  */
float MPX5010_read_adc();

#endif /* INC_JEEVANLITE_PERIPHERALS_SENSORS_MPX5010_H_ */
