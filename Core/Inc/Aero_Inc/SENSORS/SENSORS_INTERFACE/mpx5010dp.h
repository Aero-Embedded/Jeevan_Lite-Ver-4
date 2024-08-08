/**
 *******************************************************************************
 * \file mpx5010dp.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of mpx5010dp O2 pressure sensor integration.
 *
 * This file contains the Declaration of mpx5010dp O2 pressure sensor integration,
 * including the initialization.
 *******************************************************************************
 **/

#ifndef INC_JEEVANLITE_PERIPHERALS_SENSORS_MPX5010DP_H_
#define INC_JEEVANLITE_PERIPHERALS_SENSORS_MPX5010DP_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include <SENSORS_INTERFACE/sensor.h>

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

SENSOR MPX5010DP;

/**
  * \brief  Initialize the MPX5010DP sensor.
  */
void MPX5010DP_init();

/**
  * \brief  Calibrate the MPX5010DP sensor.
  */
void MPX5010DP_calibrate();

/**
  * \brief  Read data from the MPX5010DP sensor.
  */
void MPX5010DP_read();

/**
  * \brief  Perform analog-to-digital conversion for the MPX5010DP sensor.
  */
float MPX5010DP_read_adc();


#endif /* INC_JEEVANLITE_PERIPHERALS_SENSORS_MPX5010DP_H_ */
