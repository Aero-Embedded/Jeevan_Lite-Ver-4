/**
 *******************************************************************************
 * \file ads1115.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of ADS1115 Module Configuration.
 *
 * This file contains the Declaration of ADS1115 Module Configuration, including
 * the initialization
 *******************************************************************************
 **/

#ifndef INC_JEEVANLITE_PERIPHERALS_ANALOG_ADS1115_H_
#define INC_JEEVANLITE_PERIPHERALS_ANALOG_ADS1115_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include "stdint.h"
#include "stm32f4xx_hal.h"

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/
uint16_t Trigger_Ads1115_Raw_Adc;
uint8_t Proximal_Sensor_ADS1115,_5010DP_Sensor_ADS1115,ENVITEC_Sensor_ADS1115;
/**
 * @name ADS1115 Device Address
 * @{
 */
#define ADS1115_ADDRESS (0X48<<1) ///< Default Address.
/** @} */

/**
 * @name ADS1115 Operational Status (OS) Configuration
 * @{
 */
#define ADS1115_OS (0b0 << 7) ///< Default Operational Status value. */
/** @} */

/**
 * @name ADS1115 Multiplexer Configuration
 * @{
 */
#define ADS1115_MUX_AIN0 (0b100 << 4) ///< Analog input 0 configuration.
#define ADS1115_MUX_AIN1 (0b101 << 4) ///< Analog input 1 configuration.
#define ADS1115_MUX_AIN2 (0b110 << 4) ///< Analog input 2 configuration.
#define ADS1115_MUX_AIN3 (0b111 << 4) ///< Analog input 3 configuration.
/** @} */

/**
 * @name ADS1115 Programmable Gain Amplifier (PGA) Configuration
 * @{
 */
#define ADS1115_PGA_TWOTHIRDS 	(0b000 << 1) ///< 2/3x Gain configuration.
#define ADS1115_PGA_ONE			(0b001 << 1) ///< 1x Gain configuration.
#define ADS1115_PGA_TWO			(0b010 << 1) ///< 2x Gain configuration.
#define ADS1115_PGA_FOUR		(0b011 << 1) ///< 4x Gain configuration.
#define ADS1115_PGA_EIGHT		(0b100 << 1) ///< 8x Gain configuration.
#define ADS1115_PGA_SIXTEEN		(0b111 << 1) ///< 16x Gain configuration.
/** @} */

/**
 * @name ADS1115 Mode Configuration
 * @{
 */
#define ADS1115_MODE (0b0) /**< Default Mode value. */
/** @} */

/**
 * @name ADS1115 Data Rate Configuration
 * @{
 */
#define ADS1115_DATA_RATE_8		(0b000 << 5) 	///< Data Rate: 8 SPS configuration.
#define ADS1115_DATA_RATE_16	(0b001 << 5)	///< Data Rate: 16 SPS configuration.
#define ADS1115_DATA_RATE_32	(0b010 << 5)	///< Data Rate: 32 SPS configuration.
#define ADS1115_DATA_RATE_64	(0b011 << 5)	///< Data Rate: 64 SPS configuration.
#define ADS1115_DATA_RATE_128	(0b100 << 5)	///< Data Rate: 128 SPS configuration.
#define ADS1115_DATA_RATE_250	(0b101 << 5)	///< Data Rate: 250 SPS configuration.
#define ADS1115_DATA_RATE_475	(0b110 << 5)	///< Data Rate: 475 SPS configuration.
#define ADS1115_DATA_RATE_860	(0b111 << 5)	///< Data Rate: 860 SPS configuration.
/** @} */

/**
 * @name ADS1115 Comparator Configuration
 * @{
 */
#define ADS1115_COMP_MODE 	(0b0 << 4) ///< Comparator Mode configuration.
#define ADS1115_COMP_POL 	(0b0 << 3) ///< Comparator Polarity configuration.
#define ADS1115_COMP_LAT 	(0b0 << 2) ///< Latching Comparator configuration.
#define ADS1115_COMP_QUE 	(0b11)     ///< Comparator Queue configuration.
/** @} */

/**
 * @name ADS1115 Register Addresses
 * @{
 */
#define ADS1115_CONVER_REG 0x0 ///< Conversion Register address.
#define ADS1115_CONFIG_REG 0x1 ///< Configuration Register address.
/** @} */

/**
 * @name ADS1115 Timeout Configuration
 * @{
 */
#define ADS1115_TIMEOUT 1 ///< Timeout value in milliseconds.
/** @} */

/**
 * I2C Handlers for ADS1115
 */
I2C_HandleTypeDef ADS1115_1_I2C_Handler, ADS1115_2_I2C_Handler,ADS1115_3_I2C_Handler;

/**
 * @brief Initialize the ADS1115 ADC.
 *
 * This function initializes the ADS1115 ADC with the specified I2C handler.
 *
 * @param handler Pointer to the I2C handler structure.
 *
 * @return HAL_StatusTypeDef indicating success or failure.
 */
HAL_StatusTypeDef ADS1115_Init(uint8_t ads_reference, I2C_HandleTypeDef *handler);


/**
 * @brief Read analog voltage from the ADS1115.
 *
 * This function reads the analog voltage from the specified multiplexer (mux) port
 * of the ADS1115 ADC and returns the result as a floating-point voltage value.
 *
 * @param muxPort The multiplexer (mux) port to read from.
 * @param voltage Pointer to store the read voltage value.
 *
 * @return HAL_StatusTypeDef indicating success or failure.
 */
HAL_StatusTypeDef ADS1115_Read(uint16_t muxPort, float *voltage, I2C_HandleTypeDef *handler);

#endif /* INC_JEEVANLITE_PERIPHERALS_ANALOG_ADS1115_H_ */
