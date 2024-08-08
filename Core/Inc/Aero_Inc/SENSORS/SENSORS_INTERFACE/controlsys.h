/**
 *******************************************************************************
 * \file controlsys.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of control system.
 *
 *******************************************************************************
 **/

#ifndef CONTROLSYS_H
#define CONTROLSYS_H


#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"


/**
 * \brief Structure Representing members used for pid.

 */


typedef struct {

	float Kp;
	float Ki;
	float Kd;
	float tau;
	float a1;
	float a2;
	float b1;
	float b2;
	float T;
	float area;
	float prevdif;
	float rate;
	float prevmea;		
	float out;

} controlsys;


/**
 * \brief controlsys_Init.
 * \param controlsys.
 * */
void  controlsys_Init(controlsys *cs);

/**
 * \brief controlsys_Update.
 * \return The DAC value corresponds to the proportional valve opening
 * */
float controlsys_Update(controlsys *cs, float set, float mea);


controlsys Parker_valve_tune;
#endif
