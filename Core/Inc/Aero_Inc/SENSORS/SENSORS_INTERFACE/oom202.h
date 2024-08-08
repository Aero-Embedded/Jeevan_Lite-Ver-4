/**
 *******************************************************************************
 * \file oom202.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of Oxygen_Sensor_Integeration.
 *
 * This file contains the Declaration of Oxygen_Sensor_Integeration.,
 * including the initialization
 *******************************************************************************
 **/

#ifndef INC_JEEVANLITE_PERIPHERALS_SENSORS_OOM202_H_
#define INC_JEEVANLITE_PERIPHERALS_SENSORS_OOM202_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include <SENSORS_INTERFACE/sensor.h>

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

/**
  * @brief  OOM202 Sensor - Defined as a generic sensor
  */

SENSOR OOM202;

/**
  * @brief  Initialize the OOM202 sensor.
  */
void OOM202_init();

/**
  * @brief  Calibrate the OOM202 sensor.
  */
void OOM202_calibrate(uint8_t);

/**
  * @brief  Calculate the efficiency of the OOM202 sensor.
  */
void OOM202_calculate_efficiency();

/**
  * @brief  Read data from the OOM202 sensor.
  */
void OOM202_read();

/**
  * @brief  Perform analog-to-digital conversion for the OOM202 sensor.
  */
float OOM202_read_adc();


#endif /* INC_JEEVANLITE_PERIPHERALS_SENSORS_OOM202_H_ */
