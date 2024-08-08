/**
 *******************************************************************************
 * \file Bluetooth_Graph.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of Blue-tooth data reception and transmission.
 *
 * This file contains the Declaration of Blue-tooth data reception and transmission.
 * including the initialization
 *******************************************************************************
 **/

#ifndef INC_BLUETOOTH_GRAPH_H_
#define INC_BLUETOOTH_GRAPH_H_

#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"




/**
 * \brief Structure Representing Graph values for modes.
 * data send to bluetooth.
 */

typedef struct
{
	uint16_t _header ;             ///< data header.
	uint8_t  _length ;             ///< data length.
	uint8_t  _Pressure_Val;        ///< data PIP value.
	int16_t  _Flow_Val;            ///< Flow value.
	int16_t  _Volume_Val;          ///< volume value.
	uint8_t  _Status;              ///< status value.
	uint8_t  _CRC8;                ///< crc value.
}GRAPH_PACKET_SEND ;

/**
 * \brief Structure Representing Mode parameter values for modes.
 * data receive from tab.
 */

typedef struct
{
	uint16_t header;                      ///< data header.
	uint8_t  length;                      ///< data length.
	uint8_t  Control_Byte;                ///< inspiration or expiration cycle.
	uint8_t  PIP_PS_Phigh;                ///< PIP value.
	uint8_t  PEEP_CPAP_Plow;              ///< PEEP value.
	uint16_t VTI_VS;                      ///< volume value.
	uint8_t  FiO2;                        ///< Oxygen value.
	uint8_t  RR;                          ///< Respiratory rate value.
	uint8_t  T_high;                      ///< inspiration Time.
	uint8_t  T_low;                       ///< Expiration time.
	uint8_t  Rise_Time;                   ///< Rise time value.
	uint8_t  Flow_Limit_FiO2_Flow_Limit;  ///< Flow limit value.
	uint8_t  Apnea_Time;                  ///< Apnea time.
	uint8_t  Trigger_Time;                ///< Trigger time.
	uint8_t  Trigger_Limit;               ///< Trigger Limit.
    uint8_t  Vent_Type_Min_O2;            ///< Mask mode or et tube mode.
    uint8_t  Max_O2;                      ///< Maximum O2 sensor value.
    uint8_t  CRC8;                        ///< crc.
}RECEIVE_GRAPH_PACKET ;


/**
 *\brief Enumeration for wait time for uart initialization.
 */

typedef enum
{
	BlueTooth_Wait_Time = 0,              ///< step1.
	Bluetooth_Init ,                      ///< step2.
	Bluetooth_Send_Data                   ///< step3.
}Bluetooth_T;


/**
 *\brief Enumeration didn't used in the code.
 */

typedef enum
{
	BlueTooth_Status_Wait_Time = 0,
	Bluetooth_Status_Init,
	Bluetooth_Status_Send_Data,

}Bluetooth_Status_T;


/**
 *\brief Enumeration Transmitting graph header.
 */


typedef enum
{
	Send_BlueTooth_Graph_Header             = 0x4C2A,       ///< for mode graph header.
	Send_BlueTooth_Alert_Header             = 0x4D2A,       ///< for alert header.
	Send_BlueTooth_Sampled_Parameter_Header = 0x532A,       ///< sampled parameter header.
	Send_BlueTooth_Calibration_Header       = 0x542B,       ///< calibration frame header.
	Send_BlueTooth_Service_Header           = 0x532C        ///< service frame header.

}Send_Graph_header;

/**
 *\brief Enumeration Reception graph header.
 */
typedef enum
{
	Receive_BlueTooth_Graph_Header = 0x4D2B,          ///< for mode graph header.
	Receive_BlueTooth_Alert_Header = 0x412B,          ///< for alert header.
	Receive_Nebuliser_Header       = 0x462B,          ///< nebuliser header.
	Receive_Calibration_Header     = 0x542C,          ///< calibration frame header.
	Receive_Service_Header         = 0x532C           ///< service frame header.
}Receive_Graph_header;



/**
 *\brief Structure Representing uart data reception buffer.
 */

typedef struct
{
	uint16_t Bluetooth_Transmit_Delay;    ///< uart task transmitting delay.
	uint8_t Bluetooth_RX_BUF[20];         ///< uart receive array.
	uint8_t UART_RX_BUF_CURR[20];         ///< uart receive array for rearrange the data.
}Bluetooth_parameter;


xQueueHandle Bluetooth_Receive;          ///< reception of Bluetooth in queue.


GRAPH_PACKET_SEND Graph_Data;           ///< structure variable for send graph data.

Bluetooth_parameter Bluetooth_Parameter;   ///< structure variable for Bluetooth_parameter.

Bluetooth_T Bluetooth;                     ///< enumeration variable.
Bluetooth_Status_T Bluetooth_Status;       ///< enumeration variable.
Send_Graph_header Send_Graph_Header;       ///< enumeration variable.
Receive_Graph_header Receive_Graph_Header; ///< enumeration variable.


xTaskHandle Bluetooth_data_Split_Handler;   ///< Task handle  variable.
xTaskHandle Bluetooth_Handler;              ///< Task handle  variable.
xTaskHandle Bluetooth_Receiver_Handler;     ///< Task handle  variable.


/**
 *\brief Bluetooth task.
 */
void Bluetooth_Task(void *argument);

/**
 *\brief check sum function.
 */
uint8_t chksum8(const unsigned char *buff, size_t len);

/**
 *\brief UART6 initialization.
 */
void UART6_Init();

/**
 *\brief UART5 initialization.
 */
void UART5_Init();

/**
 *\brief Bluetooth_Graph_Data_Send.
 */
void Bluetooth_Graph_Data_Send();

/**
 *\brief whole Bluetooth_Data_Send.
 */
void Bluetooth_Data_Send();

/**
 *\brief Receive bluetooth data task.
 */
void BlueTooth_Receiver_Task(void *argument);


/**
 *\brief split bluetooth received data function.
 */

void Bluetooth_Packet_Data_Split(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet);

/**
 *\brief split bluetooth received data task.
 */


void Bluetooth_data_split_Task (void *argument);


/**
 *\brief split PC-CMV buffer from bluetooth received data .
 */
void Pc_Cmv_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet);


/**
 *\brief split VC-CMV buffer from bluetooth received data .
 */
void Vc_Cmv_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet);


/**
 *\brief split PC-SIMV buffer from bluetooth received data .
 */
void Pc_Simv_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet);


/**
 *\brief split VC-SIMV buffer from bluetooth received data .
 */
void Vc_Simv_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet);

/**
 *\brief split psv buffer from bluetooth received data .
 */
void PSV_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet);

/**
 *\brief split CPAP buffer from bluetooth received data .
 */
void CPAP_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet);

/**
 *\brief split BIPAP buffer from bluetooth received data .
 */
void BIPAP_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet);

/**
 *\brief split APRV buffer from bluetooth received data .
 */
void APRV_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet);
/**
 *\brief split BACKUP PC-CMV buffer from bluetooth received data .
 */
void Backup_Pc_Cmv_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet);
/**
 *\brief DIDN'T USED .
 */
void Backup_Vc_Cmv_Mode_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet);

/**
 *\brief split HFNC buffer from bluetooth received data .
 */
void HFNC_Packet_Data(RECEIVE_GRAPH_PACKET *Receive_Graph_Packet);

/**
 *\brief split SUCTION buffer from bluetooth received data .
 */
void SUCTION_Packet_Data();

/**
 *\brief used for debugging purpose .
 */
void UART5_GRAPH_DATA_DEBUG();

/**
 *\brief used for debugging purpose .
 */
void USB_GRAPH_DATA_DEBUG();


#endif /* INC_BLUETOOTH_GRAPH_H_ */
