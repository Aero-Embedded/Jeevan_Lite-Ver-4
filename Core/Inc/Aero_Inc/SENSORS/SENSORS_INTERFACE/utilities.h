/******************************************************************************/
/*																			  */
/* Project	    : JeevanLite			     		                          */
/*																			  */
/*----------------------------------------------------------------------------*/
/*                                                                 			  */
/* @file		: enum.h                                 	        	      */
/* @author		: GOWTHAMAAN, ABS2020E007                                 	  */
/* @date		: 20-11-2023, 15:31											  */
/*                                                           				  */
/*----------------------------------------------------------------------------*/
/* @brief       : ENUM Declaration            		  						  */
/******************************************************************************/

#ifndef INC_JEEVANLITE_SHARED_UTILITIES_H_
#define INC_JEEVANLITE_SHARED_UTILITIES_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include "stdint.h"

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/

/**
 * @brief Kalman Filter Initial Conditions
 *
 * error_measurement: Measurement Uncertainty
 * error_estimate: Estimation Uncertainty - Can be initialized with the same value as error_measurement since the Kalman filter will adjust its value.
 * process_variance: Process Variance - usually a small number between 0.001 and 1
 * */
typedef struct{
	float error_measurement;
	float error_estimate;
	float process_variance;
}KALMAN_CONSTANTS;

/**
 * @brief Constrain a value within a specified range.
 *
 * This function takes a value and ensures that it falls within the specified
 * minimum and maximum range. If the value is outside this range, it is
 * constrained to the nearest boundary.
 *
 * @param value The value to be constrained.
 * @param minimum The minimum allowed value.
 * @param maximum The maximum allowed value.
 *
 * @return The constrained value within the specified range.
 */
int16_t constraint(int16_t value, int16_t minimum, int16_t maximum);

/**
 * @brief Kalman Filter
 * */
float kalman_filter(float mea, float *err_estimate, float err_measure, float q, int8_t last_estimate);

#endif /* INC_JEEVANLITE_SHARED_UTILITIES_H_ */
