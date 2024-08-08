/**
 *******************************************************************************
 * @file Bluetooth_Graph.c
 * @author Sarath Kumar
 * @date 27/03/24
 * @brief Implementation of Blue-tooth data reception and transmission.
 *
 * This file contains the implementation Blue-tooth data reception and transmission using
 * interrupts,task and function
 *******************************************************************************
 * */


#include "Bluetooth_Graph.h"



/**
 * @brief Bluetooth_Task.
 * This task control whole uart control function.
 * @param None.
 * */

uint8_t Check_Mode;

void Bluetooth_Task(void *argument)
{
	while(1)
	{
		 switch(Bluetooth_Status)
		 {
		      case BlueTooth_Wait_Time:
		    	  UART6_Init();
		    	  Bluetooth_Status=Bluetooth_Status_Init;
		    	  Bluetooth_Parameter.Bluetooth_Transmit_Delay=Four_Second_Delay;

		      break;
		      case Bluetooth_Status_Init:
		    	  HAL_UART_Receive_IT(&huart6,(uint8_t *) Bluetooth_Parameter.Bluetooth_RX_BUF, sizeof( Bluetooth_Parameter.Bluetooth_RX_BUF));
		    	  Bluetooth_Status                             = Bluetooth_Status_Send_Data;
		    	  Bluetooth_Parameter.Bluetooth_Transmit_Delay = Twenty_MilliSecond_Delay;

			  break;
		      case Bluetooth_Status_Send_Data:
		    	  Bluetooth_Data_Send();
			  break;
		      default:
		      break;
		 }

		 vTaskDelay(Bluetooth_Parameter.Bluetooth_Transmit_Delay);

	}
}

/**
 * @brief UART6_Init.
 * This Function initialise the uart 6 peripheral.
 * @param None.
 * */

void UART6_Init()
{
	huart6.Instance             = USART6;
	huart6.Init.BaudRate        = 57600;
	huart6.Init.WordLength      = UART_WORDLENGTH_8B;
	huart6.Init.StopBits        = UART_STOPBITS_1;
	huart6.Init.Parity          = UART_PARITY_NONE;
	huart6.Init.Mode            = UART_MODE_TX_RX;
	huart6.Init.HwFlowCtl       = UART_HWCONTROL_NONE;
	huart6.Init.OverSampling    = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart6) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief chksum8.
 * This function is used to identify a data error.
 * */


uint8_t chksum8(const unsigned char *buff, size_t len)
{
    unsigned int sum;
    for ( sum = 0 ; len != 0 ; len-- )
        sum += *(buff++);
    return (uint8_t)sum;
}


/**
 * @brief Bluetooth_Graph_Data_Send.
 * This Function send the mode graph data.
 * @param None.
 * */

void Bluetooth_Graph_Data_Send()
{
	if( ( !(Device_Control == RESUME))   )
	{
		GRAPH_VALUES.volume       = RESET;
		Volume_max                = RESET;
	}
	if(LAST_FLOW_TRIGGER != 0)
	{
		GRAPH_VALUES.flow=(int)LAST_FLOW_TRIGGER;
	}
	else
	{
		GRAPH_VALUES.flow=(int)GRAPH_VALUES.flow;
	}
	Graph_Data._header          = Send_BlueTooth_Graph_Header;
	Graph_Data._length          = sizeof(Graph_Data)-3 ;
	Graph_Data._Pressure_Val    = GRAPH_VALUES.pressure;
	Graph_Data._Flow_Val        = GRAPH_VALUES.flow;
	Graph_Data._Volume_Val      = GRAPH_VALUES.volume;

	Check_Mode                  = Mode_Val<<4;
	Graph_Data._Status          = Check_Mode|(Device_Control<<3)|BREATH_STATE;
	//Graph_Data._Status          = BREATH_STATE;

	Graph_Data._CRC8            = chksum8(&Graph_Data._Pressure_Val,(Graph_Data._length - 1));
	LAST_FLOW_TRIGGER           = 0;

    HAL_UART_Transmit_IT(&huart6,(uint8_t*)&Graph_Data,sizeof(Graph_Data));

}



/**
 * @brief Bluetooth_Data_Send.
 * This function controls the type of Bluetooth data that needs to be sent at a time.
 * @param None.
 * */

void Bluetooth_Data_Send()
{


	switch (Send_Bluetooth_Data_Farme)
	{
        case SEND_SAMPLED_PARAMETER_FRAME:
        		switch (Send_Frame)
			    {
        		    case First_Frame:
        		    	Sampled_Parameter_List_1_Transmit();
        		    	if(Running_Mode != Hfnc)
        		    		Send_Bluetooth_Data_Farme = SEND_ALERT_FRAME;
        		    	else
        		    		Send_Bluetooth_Data_Farme = SEND_GRAPH_FRAME;
        		    break;
        		    default:
        		    break;

			    }

        break;
		case SEND_CALIBRATION_FRAME:
			Send_Calibrated_Packet();
		break;
		case SEND_SERVICE_FRAME:
			Send_Serviced_Packet();
		break;
		case SEND_ALERT_FRAME:
			switch (Apnea_Alert)
			{

				  case APNEA_ALERT_HAPPEN:
					  SEND_ALERT_PACKET();
					  CLEAR_ALERT_BIT(SECOND_FRAME_UN,_ALERT_APNEA);
					  CLEAR_ALERT_BIT(FIRST_FRAME_UN,_ALERT_PATIENT_CIRCUIT_DISCONNECTED);
					  Apnea_Alert               = NO_APNEA_ALERT_HAPPEN;
					  Send_Bluetooth_Data_Farme = SEND_GRAPH_FRAME;
				  break;
				  case NO_APNEA_ALERT_HAPPEN:
					switch (Mode)
					{
						case Idle:
							SEND_ALERT_PACKET();
							Send_Bluetooth_Data_Farme = SEND_GRAPH_FRAME;
						break;
						default:
							SEND_ALERT_PACKET();
							Clear_All_Alert_Bits();
							//Send_Bluetooth_Data_Farme = SEND_SAMPLED_PARAMETER_FRAME;       //new
							Send_Bluetooth_Data_Farme = SEND_GRAPH_FRAME;
						break;

					}
				  break;
			 }


		break;
		case SEND_GRAPH_FRAME:
			Bluetooth_Graph_Data_Send();
		break;
	}
}



/**
 * @brief BlueTooth_Receiver_Task.
 * This function controls the type of Bluetooth data that needs to be send at a time.
 * @post this function resume the Bluetooth_data_split_Task.
 * */


void BlueTooth_Receiver_Task(void *argument)
{
	while (1)
	{
		if (xQueueReceive(Bluetooth_Receive, &(Bluetooth_Parameter.Bluetooth_RX_BUF), portMAX_DELAY) == pdTRUE)
		{
			Uart_Receive_Debug_Toggle_Led();
			vTaskResume(Bluetooth_data_Split_Handler);
		}
   }
}

/**
 * @brief Bluetooth_data_split_Task.
 * This function calls from the BlueTooth_Receiver_Task.
 * @post it split the data after the function call.
 * */

void Bluetooth_data_split_Task (void *argument)
{
	while (1)
	{
		Bluetooth_Packet_Data_Split((RECEIVE_GRAPH_PACKET*) (Bluetooth_Parameter.Bluetooth_RX_BUF));
		vTaskSuspend(Bluetooth_data_Split_Handler);
	}
}



/**
 * @brief HAL_UART_RxCpltCallback.
 * This is an interrupt function call from UART6..
 * @param uart6.
 * */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	HAL_UART_Receive_IT(&huart6,(uint8_t *) (Bluetooth_Parameter.Bluetooth_RX_BUF), sizeof( Bluetooth_Parameter.Bluetooth_RX_BUF));
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendToFrontFromISR(Bluetooth_Receive, &(Bluetooth_Parameter.Bluetooth_RX_BUF), &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


