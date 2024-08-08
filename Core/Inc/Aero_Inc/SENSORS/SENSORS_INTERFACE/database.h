/**
 *******************************************************************************
 * \file database.h
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Declaration of Graph variables.
 *
 * This file contains the Declaration of Graph variables.
 *******************************************************************************
 **/

#ifndef INC_JEEVANLITE_SHARED_DATABASE_H_
#define INC_JEEVANLITE_SHARED_DATABASE_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include "stdint.h"

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/



/**
 * \brief Structure main  patient Graph values.
 */

typedef struct{
	uint8_t pressure;               ///<  pressure graph value.
	float flow;                     ///<  Flow graph value.
	float volume;                   ///<  volume graph value.
	int8_t oxygen;                  ///<  Achieved oxygen value.
	float oxygen_flow;              ///<  oxygen flow value.
	int16_t Maximum_oxygen_flow;    ///<  maximum oxygen flow value.
}INSTANTANEOUS_VALUE;

INSTANTANEOUS_VALUE GRAPH_VALUES;     ///<  graph data parameter

uint16_t ADC1_DATA[2];                ///<  Adc value


/**
  * \brief  Initialize the graph data.
  */
void init_database();

#endif /* INC_JEEVANLITE_SHARED_DATABASE_H_ */
