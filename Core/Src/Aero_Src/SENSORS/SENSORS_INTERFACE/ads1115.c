/**
 *******************************************************************************
 * \file ads1115.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Implementation of ADS1115 Module Configuration.
 *
 * This file contains the Implementation of ADS1115 Module Configuration, including
 * the initialization
 *******************************************************************************
 **/


/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include <SENSORS_INTERFACE/ads1115.h>
#include <SENSORS_INTERFACE/constants.h>
#include "stm32f4xx_hal.h"
#include "string.h"

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/


/**
  * @brief  Initialize the ADS1115 I2C communication.
  * @param  handler: Pointer to the I2C handler structure.
  * @retval HAL status.
  */
HAL_StatusTypeDef ADS1115_Init(uint8_t ads_reference, I2C_HandleTypeDef *handler) {

	if(ads_reference==1){
		// Handler
		memcpy(&ADS1115_1_I2C_Handler, handler, sizeof(*handler));
	}else if(ads_reference==2){
		// Handler
		memcpy(&ADS1115_2_I2C_Handler, handler, sizeof(*handler));
	}else if(ads_reference==3){
		// Handler
		memcpy(&ADS1115_3_I2C_Handler, handler, sizeof(*handler));
	}else{
		return HAL_ERROR;
	}

    HAL_StatusTypeDef ads_status = HAL_I2C_IsDeviceReady(handler, ADS1115_ADDRESS, 5, ADS1115_TIMEOUT);
    // Check if ADS1115 is ready
    if (ads_status== HAL_OK) {
        return HAL_OK;
    } else {
        return HAL_ERROR;
    }
}

/**
  * @brief  Read the ADC value from the ADS1115.
  * @param  muxPort: Multiplexer configuration for the ADC.
  * @param  voltage: Pointer to store the converted voltage.
  * @retval HAL status.
  */
HAL_StatusTypeDef ADS1115_Read(uint16_t muxPort, float *voltage, I2C_HandleTypeDef *handler) {
    uint8_t ADS1115_config[3];
    uint8_t ADS1115_rawValue[2];
    float ADS1115_voltCoef = 6144.0;

    // Configure ADS1115
    ADS1115_config[0] = 0X01;
    ADS1115_config[1] = ADS1115_OS | muxPort | ADS1115_PGA_TWOTHIRDS | ADS1115_MODE;
    ADS1115_config[2] = ADS1115_DATA_RATE_860 | ADS1115_COMP_MODE | ADS1115_COMP_POL | ADS1115_COMP_LAT| ADS1115_COMP_QUE;

    // Write configuration to ADS1115
 	if(HAL_I2C_Master_Transmit(handler, ADS1115_ADDRESS, ADS1115_config, 3, 1)==HAL_OK)
 	{
 		ADS1115_config[0] = 0X00;
  		if(HAL_I2C_Master_Transmit(handler, ADS1115_ADDRESS, ADS1115_config, 1, 1)==HAL_OK)
  		{
  			// Read converted value from ADS1115
  			if(HAL_I2C_Master_Receive(handler, ADS1115_ADDRESS, ADS1115_rawValue, 2, 1)==HAL_OK)
  			{
  				// Convert raw value to voltage
  				uint16_t adc = (ADS1115_rawValue[0] << 8) | ADS1115_rawValue[1];

  				Trigger_Ads1115_Raw_Adc = adc;

  				*voltage = (adc*ADS1115_voltCoef/ADS1115_RESOLUTION);

  				return HAL_OK;
  			}
  		}
  	}
    return HAL_ERROR;
}



