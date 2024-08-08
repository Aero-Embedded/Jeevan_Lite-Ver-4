/**
 *******************************************************************************
 * @file contraint.c
 * @author Sarath Kumar
 * @date 27/03/24
 * @brief check sensors's minimum and maximum value.

 * This file contains the implementation of Limit a minimum and maximum value of sensor.
 *******************************************************************************
 * */


#include <SENSORS_INTERFACE/utilities.h>

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/


/**
 * @brief constraint.
 * check minimum and maximum value of sensor.
 * @param Running value.
 * @param minimum value.
 * @param maximum value.
 * */

int16_t constraint(int16_t value, int16_t minimum, int16_t maximum){
   if (value > maximum){
	   return maximum;
	}else if (value < minimum){
	   return minimum;
	}else{
	   return value;
	}
}
