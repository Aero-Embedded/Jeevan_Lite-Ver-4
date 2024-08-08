/**
 *******************************************************************************
 * \file database.c
 * \author Sarath Kumar
 * \date 27/03/24
 * \brief Initialization of graph values.
 *
 *******************************************************************************
 **/


/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/

#include <SENSORS_INTERFACE/database.h>

/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/


/**
  * @brief  Initialize the graph data.
  */


void init_database(){
	GRAPH_VALUES.pressure = 0;
	GRAPH_VALUES.flow = 0;
	GRAPH_VALUES.oxygen_flow = 0;
	GRAPH_VALUES.volume = 0;
	GRAPH_VALUES.oxygen = 20;

	ADC1_DATA[0] = 0;
	ADC1_DATA[1] = 0;
}
