/*
 * Service.h
 *
 *  Created on: Dec 27, 2023
 *      Author: asus
 */

#ifndef INC_AERO_INC_SERVICE_SERVICE_H_
#define INC_AERO_INC_SERVICE_SERVICE_H_


#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"




typedef struct __attribute__((packed))
{
	uint16_t  _header ;
	uint8_t   _length ;
	uint8_t   SERVICE_STATUS;
	uint16_t  BLOWER_SIGNAL;
	uint16_t  PROPORTIONAL_VALVE_SIGNAL;
	uint8_t   SENSOR_OFFSET;
	uint8_t   BLOWER_ENABLE;
	uint8_t   EXPIRATORY_VALVE_CONTROL;
	uint8_t   NEBULIZER_CONTROL;
	uint8_t   NC2;
	uint8_t   NC3;
	uint8_t   NC4;
	uint8_t   NC5;
	uint8_t   NC6;
	uint8_t   NC7;
	uint8_t   NC8;
	uint8_t   _CRC8;

} SERVICE_PACKET;


typedef struct
{
	uint8_t   Service_Status;
	uint16_t  Blower_Signal;
	uint16_t  Proportional_Valve_Signal;
	uint8_t   Sensor_Offset;
	uint8_t   Blower_Enable;
	uint8_t   Expiratory_Valve_Control;
	uint8_t   Nebulizer_Control;
}Service;


typedef struct __attribute__((packed))
{
	    uint16_t _header ;
		uint8_t  _length ;
		uint8_t   Sensor1_LSB;
		uint8_t   Sensor1_MSB;
		uint8_t   Sensor2_LSB;
		uint8_t   Sensor2_MSB;
		uint8_t   Sensor3_LSB;
		uint8_t   Sensor3_MSB;
		uint8_t   _CRC8;

} Send_Service_Packet;



typedef enum
{
	SERVICE_FAIL          = 0X00,
	SERVICE_PASS          = 0X01,

}Service_Result;

void Send_Serviced_Packet();
Send_Service_Packet SEND_SERVICE_PACKET;

Service SERVICE;
xTaskHandle Service_Handler;

void Split_Service_Packets(SERVICE_PACKET *);

void SERVICE_Task(void *argument);

#endif /* INC_AERO_INC_SERVICE_SERVICE_H_ */
