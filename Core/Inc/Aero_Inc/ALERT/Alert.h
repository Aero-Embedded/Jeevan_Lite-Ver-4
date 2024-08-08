/**
 *******************************************************************************
 * \file Alert.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of an indicator for abnormal status of the ventilator.
 *
 * This file contains the Declaration of an indicator for abnormal status of the ventilator,
 * including the initialization
 *******************************************************************************
 **/

#ifndef INC_ALERT_H_
#define INC_ALERT_H_


#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"





#define Red_Led_ON()     GPIOD->ODR|=(1<<4)           ///< Alert indication Red Led ON
#define Red_Led_OFF()    GPIOD->ODR&=(~(1<<4))        ///< Alert indication Red Led OFF
#define Blue_Led_ON()    GPIOD->ODR|=(1<<3)           ///< Alert indication Blue Led ON
#define Blue_Led_OFF()   GPIOD->ODR&=(~(1<<3))        ///< Alert indication Blue Led OFF
#define Green_Led_ON()   GPIOD->ODR|=(1<<5)           ///< Alert indication Green Led ON
#define Green_Led_OFF()  GPIOD->ODR&=(~(1<<5))        ///< Alert indication Green Led OFF



#define SET_ALERT_BIT(frame,bitname)    (DEVICE_ALERT_DATA_SEND.frame.FRAMEBits.bitname = 1 )   ///< Function macro for set alerts
#define CLEAR_ALERT_BIT(frame,bitname)  (DEVICE_ALERT_DATA_SEND.frame.FRAMEBits.bitname = 0 )   ///< Function macro for clear alerts
#define DEVICE_ALERT_DATA_SEND_LENGTH 6                                                         ///< alert data frame length



/**
 * \brief Structure Alert data first frame used as one byte.
 * it includes power status,oxygen blending based alerts
 */


typedef struct __attribute__((packed)) {

	volatile uint8_t _ALERT_PLUGGED_IN:1;                    ///< 230v ac supply
	volatile uint8_t _ALERT_BATTERY_MODE:1;                  ///< 29v dc supply
	volatile uint8_t _ALERT_20_PER_BATTERY_DRAIN:1;          ///< battery drain condition
	volatile uint8_t _ALERT_OXYGEN_SUPPLY_FAILED:1;          ///< Problem related oxygen supply input
	volatile uint8_t _ALERT_PATIENT_CIRCUIT_DISCONNECTED:1;  ///< patient circuit disconnection
	volatile uint8_t _ALERT_FLOW_SENSOR_WRONG_DIRECTION:1;   ///< The proximal flow sensor is connected in the wrong direction.
	volatile uint8_t _ALERT_NEBULISER_ON:1;                  ///< Nenulizer on
	volatile uint8_t _ALERT_NEBULISER_OFF:1;                 ///< Nebulizer off

} FIRST_FRAME ;


/**
 * \brief Structure Alert data second frame used as one byte.
 * it includes Leak,apnea alerts.
 */

typedef struct __attribute__((packed)) {

	volatile uint8_t _ALERT_APNEA:1;                      ///< update apnea status
	volatile uint8_t _ALERT_LOW_SPO2:1;                   ///< low spo2 alert
	volatile uint8_t _ALERT_PRESSURE_LIMITATION:1;        ///< pressure limitation alert
	volatile uint8_t _ALERT_LEAK:1;                       ///< leak alert
	volatile uint8_t _ALERT_NC1:1;                        ///< not implemented
	volatile uint8_t _ALERT_NC2:1;                        ///< not implemented
	volatile uint8_t _ALERT_NC3:1;                        ///< not implemented
	volatile uint8_t _ALERT_NC4:1;                        ///< not implemented

} SECOND_FRAME ;


/**
 * \brief Structure Alert data third frame used as one byte.
 * it includes high pip,low pip.
 */

typedef struct __attribute__((packed)) {

	volatile uint8_t _ALERT_HIGH_PIP:1;
	volatile uint8_t _ALERT_LOW_PIP:1;
	volatile uint8_t _ALERT_HIGH_PEEP:1;
	volatile uint8_t _ALERT_LOW_PEEP:1;
	volatile uint8_t _ALERT_HIGH_TIDAL_VOLUME:1;
	volatile uint8_t _ALERT_LOW_TIDAL_VOLUME:1;
	volatile uint8_t _ALERT_HIGH_MINUTE_VOLUME:1;
	volatile uint8_t _ALERT_LOW_MINUTE_VOLUME:1;

} THIRD_FRAME ;


/**
 * \brief Structure Alert data fourth frame used as one byte.
 * it includes high fio2,low fio2.
 */

typedef struct __attribute__((packed)) {

	volatile uint8_t _ALERT_HIGH_FIO2:1;
	volatile uint8_t _ALERT_LOW_FIO2:1;
	volatile uint8_t _ALERT_HIGH_FLOW:1;
	volatile uint8_t _ALERT_LOW_FLOW:1;
	volatile uint8_t _ALERT_HIGH_PULSE:1;
	volatile uint8_t _ALERT_LOW_PULSE:1;
	volatile uint8_t _ALERT_HIGH_RR:1;
	volatile uint8_t _ALERT_LOW_RR:1;



} FOURTH_FRAME ;


/**
 * \brief Structure Alert data fifth frame used as one byte.
 * it is dummy byte.
 */

typedef struct __attribute__((packed)) {

	volatile uint32_t _ALERT_NC:1;

} FIFTH_FRAME ;

/**
 * \brief Structure Alert data sixth frame used as one byte.
 * it is dummy byte.
 */
typedef struct __attribute__((packed)) {

	volatile uint32_t _ALERT_NC:1;
} SIXTH_FRAME ;


/**
 * \brief Structure Alert data whole frame used as one structure.
 * it includes header files.
 */

typedef struct __attribute__((packed)) {
	uint16_t _header; // 2
    uint8_t  _length; // 1
	union {
	 volatile unsigned char FIRST_BYTES;
	 FIRST_FRAME FRAMEBits ;
	}FIRST_FRAME_UN;

	union {
	 volatile unsigned char SECOND_BYTES;
	 SECOND_FRAME FRAMEBits ;
	}SECOND_FRAME_UN;

	union {
	 volatile unsigned char THIRD_BYTES;
	 THIRD_FRAME FRAMEBits ;
	}THIRD_FRAME_UN;

	union {
	 volatile unsigned char FOURTH_BYTES;
	 FOURTH_FRAME FRAMEBits ;
	}FOURTH_FRAME_UN;

	union {
	 volatile unsigned char FIFTH_BYTES;
	 FIFTH_FRAME FRAMEBits ;
	}FIFTH_FRAME_UN;

	union {
	  volatile unsigned char SIXTH_BYTES;
	  SIXTH_FRAME FRAMEBits ;
	 }SIXTH_FRAME_UN;



	volatile uint8_t _CRC8;

}Device_Alert_Data_Send;


/**
 * \brief Structure Define the alert range for each type of alert.
 */
typedef struct  __attribute__((packed))
{
	uint16_t  _header ;
	uint8_t   _length ;
	uint8_t _RANGE_PIP_MIN;
	uint8_t _RANGE_PIP_MAX;

	uint16_t _RANGE_VT_MIN;
	uint16_t _RANGE_VT_MAX;

	uint8_t _RANGE_RR_MIN;
	uint8_t _RANGE_RR_MAX;

	uint8_t _RANGE_MINT_VOL_MIN;
	uint8_t _RANGE_MINT_VOL_MAX;

	uint8_t _RANGE_SPO2_MIN;


	uint8_t _RANGE_PULSE_MIN;
	uint8_t _RANGE_PULSE_MAX;

	uint8_t _RANGE_LEAK;
	uint8_t _RANGE_NC1;
	uint8_t _CRC8;
} ALERT_RANGE_PACKET;

/**
 * \brief Structure extract  alert range for each type of alert.
 */
typedef struct
{
	uint8_t  _RANGE_PIP_MIN_Val;
	uint8_t  _RANGE_PIP_MAX_Val;
	uint16_t _RANGE_VT_MIN_Val;
	uint16_t _RANGE_VT_MAX_Val;
	uint8_t  _RANGE_RR_MIN_Val;
	uint8_t  _RANGE_RR_MAX_Val;
	uint8_t _RANGE_MINT_VOL_MIN_Val;
	uint8_t _RANGE_MINT_VOL_MAX_Val;
	uint8_t  _RANGE_SPO2_MIN_Val;
	uint8_t  _RANGE_PULSE_MIN_Val;
	uint8_t  _RANGE_PULSE_MAX_Val;
	uint16_t  _RANGE_Leak;
	uint8_t  _RANGE_NC1;

}ALERT_RANGE;

/**
 * \brief Structure Variables used to determine the alert.
 */
typedef struct
{
	uint8_t PIP_Alert_Count;
	uint8_t PEEP_Alert_Count;
	uint8_t Tidal_Volume_Alert_Count;
	uint8_t FIO2_Supply_Alert_Count;
	uint8_t FIO2_Alert_Count;
	uint8_t Leak_Alert_Count;
	uint16_t Patient_Circuit_disconnected_Alert_Count;
	uint16_t Proximal_Flow_Sensor_reversed_Alert_Count;
}Alert_count;


/**
 * \brief Enumeration store the information about alert status.
 */
typedef enum
{
	NO_ALERT,
	ALERT_HAPPEN
}alert_status;

/**
 * \brief Enumeration store the information about apnea alert status.
 */
typedef enum
{
	NO_APNEA_ALERT_HAPPEN,
	APNEA_ALERT_HAPPEN
}apnea_alert;


/**
 * \brief Enumeration for UART decides which frame needs to be sent at a time.
 */
typedef enum
{
	SEND_GRAPH_FRAME,
	SEND_ALERT_FRAME,
	SEND_CALIBRATION_FRAME,
	SEND_SAMPLED_PARAMETER_FRAME,
	SEND_SERVICE_FRAME
}send_bluetooth_data_farme;

/**
 * \brief Enumeration Reidentify whether an alert needs to be checked or not.
 */
typedef enum
{
	CHECK,
	DO_NOT_CHECK

}PATIENT_CIRCUIT_DISCONNECTED_ALERT_check;






PATIENT_CIRCUIT_DISCONNECTED_ALERT_check PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK,PROXIMAL_SENSOR_REVERSE_ALERT_CHECK;    ///< Enumeration variable


Device_Alert_Data_Send DEVICE_ALERT_DATA_SEND ;         ///< structure variable
ALERT_RANGE Alert_Range;                                ///< structure variable
Alert_count Alert_Count;                                ///< structure variable
alert_status Alert_Status;                              ///< Enumeration variable
apnea_alert Apnea_Alert;                                ///< Enumeration variable
send_bluetooth_data_farme Send_Bluetooth_Data_Farme;    ///< Enumeration variable
uint8_t Alert_Check;


xTaskHandle Alert_Handler;              ///< alert task variable

/**
 * \brief Alert_Task.
 * */
void Alert_Task (void *argument);

/**
 * \brief Split_Alert_Parameter_Packets.
 * */
void Split_Alert_Parameter_Packets(ALERT_RANGE_PACKET *Receive_Graph_Packet);
/**
 * \brief Check_Alert.
 * */
void Check_Alert();

/**
 * \brief SEND_ALERT_PACKET.
 * */
void SEND_ALERT_PACKET();

/**
 * \brief Pip_Alert.
 * */
void Pip_Alert(uint8_t);

/**
 * \brief Alert_Inspiration_Time_Parameter.
 * */
void Alert_Inspiration_Time_Parameter();

/**
 * \brief Clear_All_Alert_Bits.
 * */
void Clear_All_Alert_Bits();

/**
 * \brief UART5_Alert_data_Debug.
 * */
void UART5_Alert_data_Debug();

/**
 * \brief Peep_Alert.
 * */
void Peep_Alert(uint8_t);

/**
 * \brief Tidal_Volume_Alert.
 * */
void Tidal_Volume_Alert(int);

/**
 * \brief Minite_Volume_Alert.
 * */
void Minite_Volume_Alert(int);

/**
 * \brief Alert_Flags_Reset.
 * */
void Alert_Flags_Reset();
/**
 * \brief Respiratory_Rate_Alert.
 * */
void Respiratory_Rate_Alert();
/**
 * \brief Alert_Expiration_Time_Parameter.
 * */
void Alert_Expiration_Time_Parameter();
/**
 * \brief Oxygen_Supply_Alert.
 * */
void Oxygen_Supply_Alert(uint8_t,uint8_t);
/**
 * \brief Oxygen_Blending_Alert.
 * */
void Oxygen_Blending_Alert(uint8_t,uint8_t);
/**
 * \brief Patient_Circuit_Disconnected_Alert.
 * */
void Patient_Circuit_Disconnected_Alert();
/**
 * \brief Leak_Alert.
 * */
void Leak_Alert();
/**
 * \brief Proximal_Flow_Sensor_Reverse_Direction.
 * */
void Proximal_Flow_Sensor_Reverse_Direction();
/**
 * \brief LED_Alert.
 * */
void LED_Alert();
/**
 * \brief AC_Supply_or_Battery_Supply_Indication.
 * */
void AC_Supply_or_Battery_Supply_Indication();
/**
 * \brief Hide_Alert_1.
 * */
void Hide_Alert_1();
/**
 * \brief Hide_Alert_2.
 * */
void Hide_Alert_2();
/**
 * \brief Read_Battery_Voltage.
 * */
void Read_Battery_Voltage();

#endif /* INC_ALERT_H_ */
