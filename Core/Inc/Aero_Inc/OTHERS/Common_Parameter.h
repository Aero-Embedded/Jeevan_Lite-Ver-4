/**
 *******************************************************************************
 * \file Common_Parameter.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of Finding average of PIP,peep.
 *
 * This file contains the Declaration of Finding average of PIP,peep.
 * and also initializes DAC values.
 *******************************************************************************
 **/

#ifndef INC_COMMON_PARAMETER_H_
#define INC_COMMON_PARAMETER_H_

#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"

/**
 * \brief Structure contains common variables to check the alert.
 */

typedef struct
{
	uint8_t  _PIP_Val;
	uint8_t  _PEEP_Val;
	uint8_t  _RESPIRATORY_RATE_Val;
	uint8_t  _FIO2_Val;
	uint16_t _VOLUME_Val;
	uint8_t  _FLOW_RATE;
	uint16_t  TOLERANCE_EWAVE;
	uint16_t  TRIG_WINDOW;
	uint16_t  TRIG_LMT;
	uint16_t  TRIG_TIME;
}Common_Mode_parameter;


/**
 * \brief Structure contains peep related variables.
 */
typedef struct
{
	float    PEEP_Temp;
	int16_t  Expiratory_Valve_Open_Time;
	uint8_t  Maximum_PEEP_Acheived;
	uint32_t PEEP_Cumulative_Val;
	uint32_t PEEP_Event_Count;
	uint8_t  PEEP_AVG_VAL;
	uint32_t PEEP_Cumulative_Val_Trigger;
	uint32_t PEEP_Event_Count_Trigger;
	int      CHECK_TRIGGER;

}PEEP_maintain_parameter;

/**
 * \brief Structure contains pip related variables.
 */
typedef struct
{
	uint8_t  Maximum_PIP_Acheived;
	uint32_t PIP_Cumulative_Val;
	uint32_t PIP_Event_Count;
	uint8_t  PIP_AVG_VAL;

}PIP_average_parameter;

/**
 * \brief Structure contains time based variables.
 */
typedef struct
{
	int _INSPIRATION_TIME;
	int _EXPIRATION_TIME;
	int INSPIRATION_TIME_ACHEIVED;
	int EXPIRATION_TIME_ACHEIVED;
	int FLOW_ACHIEVED_TIMER;

}TIME_base_parameter;

/**
 *\brief Enumeration for Peep Achieved status.
 */
typedef enum
{
	PEEP_NOT_ACHEIVED,
	PEEP_ACHEIVED
}Peep_status;


/**
 *\brief Enumeration for Expiratory valve status.
 */
typedef enum
{
	OPENED,
	CLOSED
}Expiratory_Valve_Lock_delay;


/**
 *\brief Enumeration to check which type of breath cycle is running.
 */
typedef enum
{
	INSPIRATION_CYCLE = 0,
	EXPIRATION_CYCLE  = 1,
	PATIENT_TRIGGER_INSPIRATION   = 2,
	PATIENT_TRIGGER_EXPIRATION    = 3,
	NO_CYCLE
}Breathe_state;


/**
 *\brief Enumeration to check the device status.
 */
typedef enum
{
	RESUME,
	PAUSE
}Device_control;


Peep_status Peep_Status;                                    ///< Enumeration variable
Expiratory_Valve_Lock_delay Expiratory_Valve_Lock_Delay;    ///< Enumeration variable
Common_Mode_parameter Common_Mode_Parameter;                ///< Structure variable
PIP_average_parameter PIP_Average_Parameter;                ///< Structure variable
TIME_base_parameter TIME_Base_parameter;                    ///< Structure variable

PEEP_maintain_parameter PEEP_Maintain_Parameter;
Breathe_state BREATH_STATE;                                 ///< Enumeration variable


/**
 * \brief Find_Max_PIP.
 * */
void Find_Max_PIP();

/**
 * \brief Find_Maximum_PEEP.
 * */
void Find_Maximum_PEEP();

/**
 * \brief PIP_AVERAGE.
 * */
void PIP_AVERAGE();

/**
 * \brief PEEP_AVERAGE.
 * */
void PEEP_AVERAGE();
/**
 * \brief INITIAL_DAC_VALUE.
 * */
void INITIAL_DAC_VALUE();
/**
 * \brief DAC_VAL.
 * */
uint16_t DAC_VAL(uint8_t);
/**
 * \brief Change_DAC_Values.
 * */
void Change_DAC_Values(uint8_t,uint16_t);
/**
 * \brief INITIAL_DAC_VALUE.
 * */
void INITIAL_DAC_VALUE();
/**
 * \brief Change_CPAP_DAC_Values_INVASIVE.
 * */
void Change_CPAP_DAC_Values_INVASIVE(uint8_t,uint16_t);
/**
 * \brief APNEA_DAC_VAL.
 * */
uint16_t APNEA_DAC_VAL(uint8_t Set_VAL);
/**
 * \brief INITIAL_CPAP_DAC_VALUE_INVASIVE.
 * */
void INITIAL_CPAP_DAC_VALUE_INVASIVE();

#endif /* INC_COMMON_PARAMETER_H_ */
