/**
 *******************************************************************************
 * \file Bluetooth_Data_Split.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of split Blue-tooth data.
 *
 * This file contains the implementation split Blue-tooth data.
 * and pause and resume a device.
 *******************************************************************************
 **/

#ifndef INC_BLUETOOTH_DATA_SPLIT_H_
#define INC_BLUETOOTH_DATA_SPLIT_H_

#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "main.h"

/**
 *\brief Enumeration for modes in number format for identification
 */

typedef enum
{
	Idle=0,                ///< Idle.
	Pccmv=1,               ///< PC CMV mode.
	Vccmv=2,               ///< VC CMV mode.
	Aprv=4,                ///< APRV mode.
	Pcsimv=5,              ///< PC SIMV mode.
	Vcsimv=6,              ///< VC SIMV mode.
	Psv=7,                 ///< PSV mode.
	Spont=10,              ///< SPONTANEOUS mode.
	Cpap=11,               ///< CPAP mode.
	BiPap=12,              ///< BIPAP mode.
	Hfnc=13,               ///< HFNC mode.
	Suction=15,            ///< SUCTION mode.
	Backup_Pccmv=21,       ///< BACKUP PCCMV mode.
}Modes;


/**
 *\brief Enumeration for Backup mode
 */
typedef enum
{
	No_Backup_Mode,       ///< NO BACKUP PCCMV mode.
	Backup_PCCMV_Mode,    ///< BACKUP PCCMV mode.
}Backup_Modes;


/**
 *\brief Enumeration to run a next Phase of breathcycle
 */
typedef enum
{
	Generate_Inspiration_Cycle = 0,     ///< inspiration phase
	Generate_Expiration_Cycle,          ///< expiration phase
	NoWaveFormState                     ///< idle phase
}Next_half_Breath_Cycle;


/**
 *\brief Enumeration to check current Phase of breathcycle
 */
typedef enum
{
	No_Run_State=0,                ///< idle phase
	Run_Inspiration_Cycle,         ///< inspiration phase
	Run_Expiration_Cycle           ///< expiration phase
}Run_current_Breathe_State;

uint8_t Mode_Val;
Modes Mode,Running_Mode;                              ///< enumeration variable for Modes.
Backup_Modes Current_Backup_Mode;                     ///< enumeration variable for Backup_Modes.
Next_half_Breath_Cycle Next_Half_Breath_Cycle;        ///< enumeration variable for Next_half_Breath_Cycle.
Run_current_Breathe_State Run_Current_Breathe_State;  ///< enumeration variable for Run_current_Breathe_State.

/**
 * @brief chksum8.
 * This function is used to identify a data error.
 * */
uint8_t chksum8_2(const unsigned char *buff, size_t len);




#endif /* INC_BLUETOOTH_DATA_SPLIT_H_ */
