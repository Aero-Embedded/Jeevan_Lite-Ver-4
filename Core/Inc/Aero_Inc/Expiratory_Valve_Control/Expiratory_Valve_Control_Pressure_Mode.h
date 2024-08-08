/**
 *******************************************************************************
 * \file Expiratory_Valve_Control_Pressure_Mode.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of Expiratory_Valve_Control_Pressure_Mode Concept.
 *
 * This file contains the Declaration of Expiratory_Valve_Control_Pressure_Mode Concept within a mode,
 *******************************************************************************
 **/

#ifndef INC_EXPIRATORY_VALVE_CONTROL_PRESSURE_MODE_H_
#define INC_EXPIRATORY_VALVE_CONTROL_PRESSURE_MODE_H_

#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"


/**
 * \brief control PEEP_Control_For_Expiratory_Valve_Open_Time_In_Pressure_Modee.
 * */
uint16_t PEEP_Control_For_Expiratory_Valve_Open_Time_In_Pressure_Mode(uint8_t,uint8_t,uint32_t,uint8_t,uint16_t);

#endif /* INC_EXPIRATORY_VALVE_CONTROL_PRESSURE_MODE_H_ */
