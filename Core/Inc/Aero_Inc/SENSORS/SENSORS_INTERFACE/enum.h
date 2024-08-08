/******************************************************************************/
/*																			  */
/* Project	    : JeevanLite			     		                          */
/*																			  */
/*----------------------------------------------------------------------------*/
/*                                                                 			  */
/* @file		: enum.h                                 	        	      */
/* @author		: GOWTHAMAAN, ABS2020E007                                 	  */
/* @date		: 20-11-2023, 15:31											  */
/*                                                           				  */
/*----------------------------------------------------------------------------*/
/* @brief       : ENUM Declaration            		  						  */
/******************************************************************************/

#ifndef INC_JEEVANLITE_SHARED_ENUM_H_
#define INC_JEEVANLITE_SHARED_ENUM_H_

/******************************************************************************/
/*                                INCLUDE FILES		                          */
/******************************************************************************/


/******************************************************************************/
/*                                FUNCTION BODY		                          */
/******************************************************************************/


/**
 * @brief Enumeration for Controller Types for the Actuators
 *
 * This enumeration represents different types of controllers.
 * The controller is method of controlling the actuators.
 */
typedef enum {
    CONTROL_PRESSURE, /**< Pressure control mode */
    CONTROL_FLOW,     /**< Flow control mode */
    CONTROL_SPEED     /**< Speed control mode */
} CONTROLLER_TYPE;


#endif /* INC_JEEVANLITE_SHARED_ENUM_H_ */
