/*
 * Service.c
 *
 *  Created on: Dec 27, 2023
 *      Author: asus
 */


#include "Service.h"


extern SENSOR MPX7002,MPX5010DP,MPX5010;


void Split_Service_Packets(SERVICE_PACKET * Receive_Graph_Packet)
{
	SERVICE.Service_Status               =  (Receive_Graph_Packet->SERVICE_STATUS);
	SERVICE.Blower_Signal                =  (Receive_Graph_Packet->BLOWER_SIGNAL);
	SERVICE.Proportional_Valve_Signal    =  (Receive_Graph_Packet->PROPORTIONAL_VALVE_SIGNAL);
	SERVICE.Sensor_Offset                =  (Receive_Graph_Packet->SENSOR_OFFSET);
	SERVICE.Blower_Enable                =  (Receive_Graph_Packet->BLOWER_ENABLE);
	SERVICE.Expiratory_Valve_Control	 =  (Receive_Graph_Packet->EXPIRATORY_VALVE_CONTROL);
	SERVICE.Nebulizer_Control       	 =  (Receive_Graph_Packet->NEBULIZER_CONTROL);

	if(SERVICE.Service_Status == 1)
	{
		Device_Monitoring                    =  YES;
		vTaskResume(Service_Handler);
	}
	else
	{
		Device_Monitoring                    =  NO;
		ExpValve_OPEN();
		Blower_ON();
		Blower_Signal(0);
		Nebuliser_OFF();
		Parkar_valve_Signal(0);
		vTaskSuspend(Service_Handler);
	}
}

void Send_Serviced_Packet()
{

	SEND_SERVICE_PACKET._header = Send_BlueTooth_Service_Header;
	SEND_SERVICE_PACKET._length = 7;
	SEND_SERVICE_PACKET._CRC8   = chksum8_cal((unsigned char*)&SEND_SERVICE_PACKET.Sensor1_LSB,(SEND_SERVICE_PACKET._length-1));
	HAL_UART_Transmit_IT(&huart6,(uint8_t*)&SEND_SERVICE_PACKET,sizeof(SEND_SERVICE_PACKET));
	Send_Bluetooth_Data_Farme = SEND_GRAPH_FRAME;

}

void SERVICE_Task(void *argument)
{
	while(1)
	{
		if(SERVICE.Sensor_Offset == 1)
		{
			SEND_SERVICE_PACKET.Sensor1_LSB           =  (MPX5010.current_reading);
			SEND_SERVICE_PACKET.Sensor1_MSB           =  (MPX5010.current_reading>>8);
			SEND_SERVICE_PACKET.Sensor2_LSB           =  (MPX7002.current_reading);
			SEND_SERVICE_PACKET.Sensor2_MSB           =  (MPX7002.current_reading>>8);
			SEND_SERVICE_PACKET.Sensor3_LSB           =  (MPX5010DP.current_reading);
			SEND_SERVICE_PACKET.Sensor3_MSB           =  (MPX5010DP.current_reading>>8);
			Send_Bluetooth_Data_Farme                 = SEND_SERVICE_FRAME;
			vTaskDelay(2000);

			SEND_SERVICE_PACKET.Sensor1_LSB           =  (OOM202.offset_minimum);
			SEND_SERVICE_PACKET.Sensor1_MSB           =  (OOM202.offset_minimum>>8);
			SEND_SERVICE_PACKET.Sensor2_LSB           =  (OOM202.offset_maximum);
			SEND_SERVICE_PACKET.Sensor2_MSB           =  (OOM202.offset_maximum >> 8);

			SEND_SERVICE_PACKET.Sensor3_LSB           =  0;

			SEND_SERVICE_PACKET.Sensor3_LSB           =  ( (Proximal_Sensor_ADS1115 <<0)  | SEND_SERVICE_PACKET.Sensor3_LSB);
			SEND_SERVICE_PACKET.Sensor3_LSB           =  ( (_5010DP_Sensor_ADS1115  <<1)  | SEND_SERVICE_PACKET.Sensor3_LSB);
			SEND_SERVICE_PACKET.Sensor3_LSB           =  ( (ENVITEC_Sensor_ADS1115  <<2)  | SEND_SERVICE_PACKET.Sensor3_LSB);

			SEND_SERVICE_PACKET.Sensor3_MSB           =  0;

			Send_Bluetooth_Data_Farme                 = SEND_SERVICE_FRAME;
			//Blower_Signal(0);
			//Parkar_valve_Signal(0);
			vTaskDelay(Hundred_Millisecond_Delay);
			SERVICE.Sensor_Offset = 0;
		}

		if(SERVICE.Blower_Enable == 1)
		{
			Blower_ON();
		}
		if(SERVICE.Blower_Enable == 0)
		{
			Blower_OFF();
		}
		if(SERVICE.Expiratory_Valve_Control == 1)
		{
			ExpValve_CLOSE();
		}
		if(SERVICE.Expiratory_Valve_Control == 0)
		{
			ExpValve_OPEN();
		}
		if(SERVICE.Nebulizer_Control  == 0)
		{
			Nebuliser_OFF();
		}
		if(SERVICE.Nebulizer_Control  == 1)
		{
			Nebuliser_ON();
		}

			Blower_Signal(SERVICE.Blower_Signal);
			Parkar_valve_Signal(SERVICE.Proportional_Valve_Signal);

		vTaskDelay(2);
	}
}



