/*
 * Task_Create.h
 *
 *  Created on: Jan 18, 2023
 *      Author: asus
 */

#ifndef INC_TASK_CREATE_H_
#define INC_TASK_CREATE_H_

#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"


typedef struct
{
	uint16_t AdcData[3];
}ADC_Pheriperal;
ADC_Pheriperal Adc;

uint32_t free_heap_size;


void Initial_Task_Create();
void ADS1115_initialisation();

#endif /* INC_TASK_CREATE_H_ */
