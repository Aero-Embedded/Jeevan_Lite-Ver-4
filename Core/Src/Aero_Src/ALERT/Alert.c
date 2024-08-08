/**
 *******************************************************************************
 * @file Alert.c
 * @author Sarath Kumar
 * @date 27/03/24
 * @brief Implementation of an indicator for abnormal status of the ventilator.
 *
 * This file contains the Implementation of an indicator for abnormal status of the ventilator,
 * including the initialization.
 *******************************************************************************
 * */


#include "Alert.h"


uint8_t Achieved_Respiratory_Count=0;               ///< stores Achieved_Respiratory_Count
uint8_t Achieved_One_RR=0;                          ///< stores Achieved_One_RR
int Total_Tidal_volume,Maximum_Tidal_Volume_Achieved,Achieved_Minite_Volume=0;      ///< stores volume related data
int RR_E_TIME_ACHIEVED=0;                          ///< trace expiration time
int RR_I_TIME_ACHIEVED=0;                          ///< trace inspiration time
int Insp_Volume,Exp_Volume;                        ///< stores volume related data
int Leak;                                          ///< stores leak information
uint8_t Oxygen_Supply_Failed = NO;                 ///< stores oxygen supply status
uint8_t Patient_Ckt_Discon = 0;                    ///< stores patient circuit disconnection
uint8_t Proximal_Flow_Sensor_Reverse = 0;          ///< stores Proximal_Flow_Sensor_Reverse
uint8_t Toggle=0,old_State=1,Newstate=0;           ///< battery related variables



float Battery_Raw_Value;                           ///< battery related variables
extern uint8_t Toggle;                             ///< battery related variables
uint8_t Battery_Voltage_old=100,Battery_Voltage_new=0;  ///< battery related variables

uint8_t Alert_Check_Time_vc = 50;


/**
 * @brief Convert_Battery_Percentage.
 * This function calculate the battery percentage in device.
 * */
void Convert_Battery_Percentage();


/**
 * @brief SEND_ALERT_PACKET.
 * This function store the alert range data using alert_Range structure.
 * */

void Split_Alert_Parameter_Packets(ALERT_RANGE_PACKET *Receive_Graph_Packet)
{
	Alert_Range._RANGE_PIP_MIN_Val      =	(Receive_Graph_Packet->_RANGE_PIP_MIN);
	Alert_Range._RANGE_PIP_MAX_Val	    =	(Receive_Graph_Packet->_RANGE_PIP_MAX);
	Alert_Range._RANGE_VT_MIN_Val	    = 	(Receive_Graph_Packet->_RANGE_VT_MIN);
	Alert_Range._RANGE_VT_MAX_Val	    = 	(Receive_Graph_Packet->_RANGE_VT_MAX);
	Alert_Range._RANGE_RR_MIN_Val       =	(Receive_Graph_Packet->_RANGE_RR_MIN);
	Alert_Range._RANGE_RR_MAX_Val       =	(Receive_Graph_Packet->_RANGE_RR_MAX);
	Alert_Range._RANGE_MINT_VOL_MIN_Val =	(Receive_Graph_Packet->_RANGE_MINT_VOL_MIN);
	Alert_Range._RANGE_MINT_VOL_MAX_Val =	(Receive_Graph_Packet->_RANGE_MINT_VOL_MAX);
	Alert_Range._RANGE_SPO2_MIN_Val     =	(Receive_Graph_Packet->_RANGE_SPO2_MIN);
	Alert_Range._RANGE_PULSE_MAX_Val    =	(Receive_Graph_Packet->_RANGE_PULSE_MAX);
	Alert_Range._RANGE_PULSE_MIN_Val    =	(Receive_Graph_Packet->_RANGE_PULSE_MIN);
	Alert_Range._RANGE_Leak             =	(Receive_Graph_Packet->_RANGE_LEAK);
}

/**
 * @brief SEND_ALERT_PACKET.
 * This function send the alert frame from the Bluetooth_Data_Send() function.
 * */
void SEND_ALERT_PACKET()
{

	DEVICE_ALERT_DATA_SEND._header  = Send_BlueTooth_Alert_Header;
	DEVICE_ALERT_DATA_SEND._length  = 7;
	DEVICE_ALERT_DATA_SEND._CRC8    = chksum8((unsigned char*)&DEVICE_ALERT_DATA_SEND.FIRST_FRAME_UN.FIRST_BYTES,(DEVICE_ALERT_DATA_SEND_LENGTH -1));
	HAL_UART_Transmit(&huart6,(uint8_t*)&DEVICE_ALERT_DATA_SEND,sizeof(DEVICE_ALERT_DATA_SEND),300);




}

/**
 * @brief Clear_All_Alert_Bits.
 * This function resets all alert bits when the alert frame is sent
 * */

void Clear_All_Alert_Bits()
{
	CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_PLUGGED_IN);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_BATTERY_MODE);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_HIGH_PIP);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_LOW_PIP);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_HIGH_PEEP);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_LOW_PEEP);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_HIGH_MINUTE_VOLUME);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_LOW_MINUTE_VOLUME);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_HIGH_TIDAL_VOLUME);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_LOW_TIDAL_VOLUME);
	CLEAR_ALERT_BIT(FOURTH_FRAME_UN,_ALERT_HIGH_RR);
	CLEAR_ALERT_BIT(FOURTH_FRAME_UN,_ALERT_LOW_RR);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN,_ALERT_OXYGEN_SUPPLY_FAILED);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN,_ALERT_PATIENT_CIRCUIT_DISCONNECTED);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN,_ALERT_FLOW_SENSOR_WRONG_DIRECTION);
	CLEAR_ALERT_BIT(FOURTH_FRAME_UN, _ALERT_HIGH_FIO2);
	CLEAR_ALERT_BIT(FOURTH_FRAME_UN, _ALERT_LOW_FIO2);
	CLEAR_ALERT_BIT(SECOND_FRAME_UN, _ALERT_LEAK);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_20_PER_BATTERY_DRAIN);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_NEBULISER_ON);
    CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_NEBULISER_OFF);
}


/**
 * @brief Alert_Task.
 * This alert task runs only during inspiration time to check for
 * patient circuit disconnection alerts and proximal sensor reverse direction.
 * */
void Alert_Task (void *argument)
{
	while(1)
	{
		if( (Run_Current_Breathe_State == Run_Inspiration_Cycle)  )
		{
			if(Alert_Check == 1 && (! ((Assist_Control == ASSIST_OFF) && (Patient_Trigger == PATIENT_TRIGGER_HAPPEN) )) )
			{
				Patient_Circuit_Disconnected_Alert();
			}
			Proximal_Flow_Sensor_Reverse_Direction();
		}
		vTaskDelay(Two_Millisecond_Delay);
	}
}


/**
 * @brief Alert_Flags_Reset.
 * This function resets all the alerts that occur once when the mode starts
 * */
void Alert_Flags_Reset()
{
	Alert_Count.PIP_Alert_Count          = 0;
	Alert_Count.PEEP_Alert_Count         = 0;
	Alert_Count.Tidal_Volume_Alert_Count = 0;
	Achieved_Respiratory_Count           = 0;
	Total_Tidal_volume                   = 0;
	Alert_Count.FIO2_Supply_Alert_Count  = 0;
	Alert_Count.FIO2_Alert_Count         = 0;
	Alert_Count.Leak_Alert_Count         = 0;
	Alert_Count.Patient_Circuit_disconnected_Alert_Count=0;
	Alert_Check=5;
	O2_Check   = 5;
}

/**
 * @brief Check_Alert.
 * This function calls all alert functions from here
 * */
void Check_Alert()
{
	Convert_Battery_Percentage();
	if(Patient_Ckt_Discon == 1)
	{
		Hide_Alert_1();
		Patient_Ckt_Discon = 0;
	}
    else if(Proximal_Flow_Sensor_Reverse == 1)
	{
		Hide_Alert_2();
		Proximal_Flow_Sensor_Reverse = 0;
	}
	else
	{
		if(Alert_Check == 1)
		{
				Pip_Alert(PIP_Average_Parameter.Maximum_PIP_Acheived );

				Peep_Alert(Common_Mode_Parameter._PEEP_Val);

				if(Vent_Type == ET_TUBE_MODE)
				{
					Tidal_Volume_Alert(Volume_max);


					if(Achieved_Respiratory_Count == Common_Mode_Parameter._RESPIRATORY_RATE_Val)
					{
						Achieved_Minite_Volume         = ((float)Total_Tidal_volume / 1000);
						Total_Tidal_volume             = 0;
						Achieved_Respiratory_Count     = 0;
						Minite_Volume_Alert(Achieved_Minite_Volume);
					}
				}




				Respiratory_Rate_Alert();

				if(Common_Mode_Parameter._FIO2_Val>21)
				{
					Oxygen_Supply_Alert(Common_Mode_Parameter._FIO2_Val,GRAPH_VALUES.Maximum_oxygen_flow);
					Oxygen_Blending_Alert(Common_Mode_Parameter._FIO2_Val,Oxygen_Parameter.Achieved_Oxygen);
				}

				if(Vent_Type == ET_TUBE_MODE)
				{
					Leak_Alert();
				}
		}
	}
	AC_Supply_or_Battery_Supply_Indication();
	if(old_State != Newstate)
	{
		if(Toggle == 1)
		{
		    SET_ALERT_BIT(FIRST_FRAME_UN, _ALERT_PLUGGED_IN);
			CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_BATTERY_MODE);
		}
		else
		{
			CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_PLUGGED_IN);
			SET_ALERT_BIT(FIRST_FRAME_UN, _ALERT_BATTERY_MODE);
		}
		Newstate = old_State;
	}


}

/**
 * @brief Alert_Inspiration_Time_Parameter.
 * This function stores the data from the previous breath.
 * */

void Alert_Inspiration_Time_Parameter()
{

	Achieved_Respiratory_Count++;
	Total_Tidal_volume += Volume_max;
	RR_E_TIME_ACHIEVED = TIME_Base_parameter.EXPIRATION_TIME_ACHEIVED ;
	if(Alert_Check>1)
		Alert_Check--;
	if(O2_Check>1)
		O2_Check--;
	Exp_Volume=GRAPH_VALUES.volume;
    Exp_Volume=Insp_Volume-Exp_Volume;


	Leak=Exp_Volume-Insp_Volume;
}


/**
 * @brief AC_Supply_or_Battery_Supply_Indication.
 * This This function checks whether the device is running on battery mode or AC supply mode
 * */
void AC_Supply_or_Battery_Supply_Indication()
{
/*	if((HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2))== 0)
	{
		Toggle = 1;
		old_State=Toggle;
	}
	else
	{
		Toggle = 0;
		old_State = Toggle;
	}*/        //This one is for pin number has been changed PE2


		if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7))== 0)
		{
			Toggle = 1;
			old_State=Toggle;
		}
		else
		{
			Toggle = 0;
			old_State = Toggle;
		}


}

/**
 * @brief Alert_Expiration_Time_Parameter.
 * This function reset the alert flags during expiration time.
 * */

void Alert_Expiration_Time_Parameter()
{
	RR_I_TIME_ACHIEVED = TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED ;
	Insp_Volume=GRAPH_VALUES.volume;
	Alert_Count.Patient_Circuit_disconnected_Alert_Count=0;
	Alert_Count.Proximal_Flow_Sensor_reversed_Alert_Count=0;
	PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = CHECK;
	PROXIMAL_SENSOR_REVERSE_ALERT_CHECK = CHECK;

}



/**
 * @brief Pip_Alert.
 * This function checks whether the PIP value exceeds the alert limit.
 * @param PIP Maximum PIP value achieved in the last breath cycle from the mode
 * */
void Pip_Alert(uint8_t PIP)
{

	if ( (PIP >= Alert_Range._RANGE_PIP_MIN_Val) && (PIP <= Alert_Range._RANGE_PIP_MAX_Val))
	{
		Alert_Count.PIP_Alert_Count = 0;
	}
	else
	{
		Alert_Count.PIP_Alert_Count++;
		if (Alert_Count.PIP_Alert_Count > 3)
		{
			Alert_Status = ALERT_HAPPEN;

				if (Alert_Range._RANGE_PIP_MIN_Val > PIP)
				{
					SET_ALERT_BIT(THIRD_FRAME_UN, _ALERT_LOW_PIP);
					CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_HIGH_PIP);

					Alert_Count.PIP_Alert_Count = 0;
				}
				else if (Alert_Range._RANGE_PIP_MAX_Val < PIP)
				{
					SET_ALERT_BIT(THIRD_FRAME_UN, _ALERT_HIGH_PIP);
					CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_LOW_PIP);
					Alert_Count.PIP_Alert_Count = 0;
				}
		}
	}
}


/**
 * @brief Peep_Alert.
 * This function checks whether the PEEP value exceeds the alert limit.
 * @param PEEP minimum PEEP value achieved in the last breath cycle from the mode
 * */
void Peep_Alert(uint8_t PEEP)
{
	uint8_t PEEP_HIGH = (PEEP+3);
	uint8_t PEEP_LOW = (PEEP-3);
	if ((PEEP_Maintain_Parameter.PEEP_AVG_VAL <= PEEP_HIGH)&& (PEEP_Maintain_Parameter.PEEP_AVG_VAL >= PEEP_LOW))
	{
		Alert_Count.PEEP_Alert_Count = 0;
	}
	else
	{
		Alert_Count.PEEP_Alert_Count++;
			if(Alert_Count.PEEP_Alert_Count>3)
			{
				Alert_Status = ALERT_HAPPEN;

				if (PEEP_HIGH > PEEP_Maintain_Parameter.PEEP_AVG_VAL)
				{
					SET_ALERT_BIT(THIRD_FRAME_UN, _ALERT_LOW_PEEP);
					CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_HIGH_PEEP);

					Alert_Count.PEEP_Alert_Count = 0;
				}
				else if (PEEP_LOW< PEEP_Maintain_Parameter.PEEP_AVG_VAL)
				{
					SET_ALERT_BIT(THIRD_FRAME_UN, _ALERT_HIGH_PEEP);
					CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_LOW_PEEP);
					Alert_Count.PEEP_Alert_Count = 0;
				}
			}
	}
}


/**
 * @brief Tidal_Volume_Alert.
 * This function checks whether the Tidal volume value exceeds the alert limit.
 * @param Maximum_Volume_Achieved  achieved tidal volume in the last breath cycle from the mode
 * */

void Tidal_Volume_Alert(int Maximum_Volume_Achieved)
{
	if( (Maximum_Volume_Achieved >= Alert_Range._RANGE_VT_MIN_Val)  &&  (Maximum_Volume_Achieved <= Alert_Range._RANGE_VT_MAX_Val))
	{
		Alert_Count.Tidal_Volume_Alert_Count=0;
	}
	else
	{
		Alert_Count.Tidal_Volume_Alert_Count++;
		if(Alert_Count.Tidal_Volume_Alert_Count>3)
		{
			Alert_Status = ALERT_HAPPEN;
			if(Alert_Range._RANGE_VT_MIN_Val>Maximum_Volume_Achieved)
			{
				SET_ALERT_BIT(THIRD_FRAME_UN,_ALERT_LOW_TIDAL_VOLUME);
				CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_HIGH_TIDAL_VOLUME);

				Alert_Count.Tidal_Volume_Alert_Count=0;
			}
			else if(Alert_Range._RANGE_VT_MAX_Val<Maximum_Volume_Achieved)
			{
				SET_ALERT_BIT(THIRD_FRAME_UN,_ALERT_HIGH_TIDAL_VOLUME);
				CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_LOW_TIDAL_VOLUME);
				Alert_Count.Tidal_Volume_Alert_Count=0;
			}
		}

	  }
}



/**
 * @brief Minite_Volume_Alert.
 * This function checks whether the minite volume value exceeds the alert limit.
 * @param Achieved_Minite_Volume  achieved minite volume in the last one minute.
 * */

void Minite_Volume_Alert(int Achieved_Minite_Volume)
{
	if ((Achieved_Minite_Volume >= Alert_Range._RANGE_MINT_VOL_MIN_Val) && (Achieved_Minite_Volume <= Alert_Range._RANGE_MINT_VOL_MAX_Val))
	{

	}
	else
	{

		Alert_Status = ALERT_HAPPEN;

			if (Alert_Range._RANGE_MINT_VOL_MIN_Val > Achieved_Minite_Volume)
			{
				SET_ALERT_BIT(THIRD_FRAME_UN,_ALERT_LOW_MINUTE_VOLUME);
				CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_HIGH_MINUTE_VOLUME);


			}
			else if (Alert_Range._RANGE_MINT_VOL_MAX_Val < Achieved_Minite_Volume)
			{
				SET_ALERT_BIT(THIRD_FRAME_UN,_ALERT_HIGH_MINUTE_VOLUME);
				CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_LOW_MINUTE_VOLUME);
			}
	}

}

/**
 * @brief Respiratory_Rate_Alert.
 * This function checks whether the Respiratory rate value exceeds the alert limit.
 * @param None.
 * */

void Respiratory_Rate_Alert()
{
	Achieved_One_RR=  (60000/((float)(RR_E_TIME_ACHIEVED + RR_I_TIME_ACHIEVED)));
	if(( Achieved_One_RR >= Alert_Range._RANGE_RR_MIN_Val) && (Achieved_One_RR <=Alert_Range._RANGE_RR_MAX_Val))
	{

	}
	else
	{
		Alert_Status = ALERT_HAPPEN;
		if(Alert_Range._RANGE_RR_MIN_Val>Achieved_One_RR)
		{
			SET_ALERT_BIT(FOURTH_FRAME_UN,_ALERT_LOW_RR);
			CLEAR_ALERT_BIT(FOURTH_FRAME_UN,_ALERT_HIGH_RR);

		}
		else if(Alert_Range._RANGE_RR_MAX_Val<Achieved_One_RR)
		{
			SET_ALERT_BIT(FOURTH_FRAME_UN,_ALERT_HIGH_RR);
			CLEAR_ALERT_BIT(FOURTH_FRAME_UN,_ALERT_LOW_RR);
		}
	}
}


/**
 * @brief Oxygen_Supply_Alert.
 * This function checks whether any abnormal condition has occurred in the oxygen inlet section
 * @param FIO2_Val mode set FIO2_Val.
 * @param Maximum_oxygen_flow maximum oxygen flow achieved in last breath.
 * */


void Oxygen_Supply_Alert(uint8_t FIO2_Val,uint8_t Maximum_oxygen_flow)
{
	if(Maximum_oxygen_flow<=2)
	{
		Alert_Count.FIO2_Supply_Alert_Count++;
		if(Alert_Count.FIO2_Supply_Alert_Count>3)
		{
			Alert_Status = ALERT_HAPPEN;
			  SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_OXYGEN_SUPPLY_FAILED);
			  Alert_Count.FIO2_Supply_Alert_Count=0;
			  Oxygen_Supply_Failed = YES;
		}
	 }
	else
	{
		Alert_Count.FIO2_Supply_Alert_Count=0;
		Oxygen_Supply_Failed = NO;
	}

}

/**
 * @brief Oxygen_Blending_Alert.
 * his function Checks whether Achieved_Oxygen value exceeds the alert limit
 * @param FIO2_Val mode set FIO2_Val.
 * @param Maximum_oxygen_flow maximum oxygen flow achieved in last breath.
 * */
void Oxygen_Blending_Alert(uint8_t FIO2_Val,uint8_t Achieved_Oxygen)
{
	if( Oxygen_Supply_Failed == NO)
	{
			uint8_t FIO2_HIGH = (FIO2_Val+5);
			uint8_t FIO2_LOW = (FIO2_Val-5);
			 if( (Achieved_Oxygen <= FIO2_HIGH) && (Achieved_Oxygen >= FIO2_LOW))
			 {

				Alert_Count.FIO2_Alert_Count=0;
			 }
			 else
			 {

				Alert_Count.FIO2_Alert_Count++;
					if(Alert_Count.FIO2_Alert_Count>3)
					{
						Alert_Status = ALERT_HAPPEN;

						if (Achieved_Oxygen > FIO2_HIGH)
						{
							SET_ALERT_BIT(FOURTH_FRAME_UN, _ALERT_HIGH_FIO2);
							CLEAR_ALERT_BIT(FOURTH_FRAME_UN, _ALERT_LOW_FIO2);
							Alert_Count.FIO2_Alert_Count=0;
						}
						else if (Achieved_Oxygen < FIO2_LOW)
						{
							SET_ALERT_BIT(FOURTH_FRAME_UN, _ALERT_LOW_FIO2);
							CLEAR_ALERT_BIT(FOURTH_FRAME_UN, _ALERT_HIGH_FIO2);
							Alert_Count.FIO2_Alert_Count=0;
						}
					}
			 }
	}

}

/**
 * @brief Leak_Alert.
 * This function Checks for any leaks in the breathing circuit.
 * @param None.
 * */
void Leak_Alert()
{
	if(Alert_Range._RANGE_Leak  < Leak)
	{
		Alert_Count.Leak_Alert_Count++;
		 if(Alert_Count.Leak_Alert_Count>3)
		 {
			Alert_Status = ALERT_HAPPEN;
		    SET_ALERT_BIT(SECOND_FRAME_UN, _ALERT_LEAK);
		    Alert_Count.Leak_Alert_Count=0;
		  }
	}
	else
	{
		Alert_Count.Leak_Alert_Count=0;
	}
}



/**
 * @brief Patient_Circuit_Disconnected_Alert.
 * This function checks if the breathing circuit is disconnected from the device.
 * @param None.
 * */
void Patient_Circuit_Disconnected_Alert()
{
	if(Vent_Type == MASK_MODE)
	{
		if(  (GRAPH_VALUES.pressure<=3) && ( (GRAPH_VALUES.flow==0 || GRAPH_VALUES.flow==-1 || GRAPH_VALUES.flow==-2) || ( (GRAPH_VALUES.flow>= 80) && (GRAPH_VALUES.pressure<=3) ) ) )
		{
				if((DAC1->DHR12R1)>=400)
				{
					if(PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK == CHECK)
					{
						Alert_Count.Patient_Circuit_disconnected_Alert_Count++;
							if(Alert_Count.Patient_Circuit_disconnected_Alert_Count >= Alert_Check_Time)
							{
								Alert_Status = ALERT_HAPPEN;
								SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_PATIENT_CIRCUIT_DISCONNECTED);
								PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = DO_NOT_CHECK;
								Patient_Ckt_Discon = 1;
							}
					}
				}
		}
	}
	else if(Vent_Type == ET_TUBE_MODE)
	{
		if(Fio2_Blending_Method == PRESSURE_CONTROLLED)
		{
				if(  ( (GRAPH_VALUES.volume<=3) && ( (GRAPH_VALUES.flow==0 || GRAPH_VALUES.flow==-1 || GRAPH_VALUES.flow==-2))) || ( (GRAPH_VALUES.flow>= 80) && (GRAPH_VALUES.volume>=100)))
				{
						if((DAC1->DHR12R1)>=400)
						{
							if(PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK == CHECK)
							{
								Alert_Count.Patient_Circuit_disconnected_Alert_Count++;
									if(Alert_Count.Patient_Circuit_disconnected_Alert_Count >= Alert_Check_Time)
									{
										Alert_Status = ALERT_HAPPEN;
										SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_PATIENT_CIRCUIT_DISCONNECTED);
										PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = DO_NOT_CHECK;
										Patient_Ckt_Discon = 1;
									}
							}
						}
				}
		}
		else
		{
			if(  ( (GRAPH_VALUES.volume<=3) && ( (GRAPH_VALUES.flow==0 || GRAPH_VALUES.flow==-1 || GRAPH_VALUES.flow==-2))) )
			{

				if(PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK == CHECK)
				{
					Alert_Count.Patient_Circuit_disconnected_Alert_Count++;
						if(Alert_Count.Patient_Circuit_disconnected_Alert_Count >= Alert_Check_Time)
						{
							Alert_Status = ALERT_HAPPEN;
							SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_PATIENT_CIRCUIT_DISCONNECTED);
							PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = DO_NOT_CHECK;
							Patient_Ckt_Discon = 1;
						}
				}

			}

			else if(   ((GRAPH_VALUES.pressure<=3))  )
			{

				if(PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK == CHECK)
				{
					Alert_Count.Patient_Circuit_disconnected_Alert_Count++;
						if(Alert_Count.Patient_Circuit_disconnected_Alert_Count >= Alert_Check_Time)
						{
							Alert_Status = ALERT_HAPPEN;
							SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_PATIENT_CIRCUIT_DISCONNECTED);
							PATIENT_CIRCUIT_DISCONNECTED_ALERT_CHECK = DO_NOT_CHECK;
							Patient_Ckt_Discon = 1;
						}
				}

			}
		}

	}
}

/**
 * @brief Proximal_Flow_Sensor_Reverse_Direction.
 * This function checks if the proximal flow sensor is connected in the reverse direction.
 * @param None.
 * */

void Proximal_Flow_Sensor_Reverse_Direction()
{
	if(TIME_Base_parameter.INSPIRATION_TIME_ACHEIVED < 500)
	{
		if((GRAPH_VALUES.volume<(-10)))
		{
			if(PROXIMAL_SENSOR_REVERSE_ALERT_CHECK == CHECK)
			{
				Alert_Count.Proximal_Flow_Sensor_reversed_Alert_Count++;
					 if( Alert_Count.Proximal_Flow_Sensor_reversed_Alert_Count>=150)
					 {
						  Alert_Status = ALERT_HAPPEN;
						  SET_ALERT_BIT(FIRST_FRAME_UN,_ALERT_FLOW_SENSOR_WRONG_DIRECTION);
						  PROXIMAL_SENSOR_REVERSE_ALERT_CHECK =  DO_NOT_CHECK;
						  Proximal_Flow_Sensor_Reverse = 1;

					 }
			}

		}
	}
}


/**
 * \brief LED_Alert.
 * This function controls the alert indication LED
 * @param None.
 * */

void LED_Alert()
{
	if(Alert_Status == ALERT_HAPPEN)
	{
		Red_Led_ON();
		Blue_Led_OFF();
		Green_Led_OFF();

	}
	else if(Alert_Status == NO_ALERT)
	{
		Green_Led_ON();
		Blue_Led_OFF();
		Red_Led_OFF();
	}
}

/**
 * @brief Hide_Alert_1.
 * This function resets the alert bits when an alert occurs due to the
 * patient circuit being disconnected from the device.
 * */
void Hide_Alert_1()
{

	CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_PLUGGED_IN);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_BATTERY_MODE);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_HIGH_PIP);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_LOW_PIP);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_HIGH_PEEP);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_LOW_PEEP);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_HIGH_MINUTE_VOLUME);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_LOW_MINUTE_VOLUME);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_HIGH_TIDAL_VOLUME);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_LOW_TIDAL_VOLUME);
	CLEAR_ALERT_BIT(FOURTH_FRAME_UN,_ALERT_HIGH_RR);
	CLEAR_ALERT_BIT(FOURTH_FRAME_UN,_ALERT_LOW_RR);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN,_ALERT_OXYGEN_SUPPLY_FAILED);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN,_ALERT_FLOW_SENSOR_WRONG_DIRECTION);
	CLEAR_ALERT_BIT(FOURTH_FRAME_UN, _ALERT_HIGH_FIO2);
	CLEAR_ALERT_BIT(FOURTH_FRAME_UN, _ALERT_LOW_FIO2);
	CLEAR_ALERT_BIT(SECOND_FRAME_UN, _ALERT_LEAK);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_20_PER_BATTERY_DRAIN);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_NEBULISER_ON);
    CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_NEBULISER_OFF);

}

/**
 * @brief Hide_Alert_2.
 * This function resets the alert bits when an alert occurs due to the
 * proximal flow sensor is connected in the reverse direction.
 * */
void Hide_Alert_2()
{
	CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_PLUGGED_IN);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_BATTERY_MODE);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_HIGH_PIP);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_LOW_PIP);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_HIGH_PEEP);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN, _ALERT_LOW_PEEP);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_HIGH_MINUTE_VOLUME);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_LOW_MINUTE_VOLUME);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_HIGH_TIDAL_VOLUME);
	CLEAR_ALERT_BIT(THIRD_FRAME_UN,_ALERT_LOW_TIDAL_VOLUME);
	CLEAR_ALERT_BIT(FOURTH_FRAME_UN,_ALERT_HIGH_RR);
	CLEAR_ALERT_BIT(FOURTH_FRAME_UN,_ALERT_LOW_RR);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN,_ALERT_OXYGEN_SUPPLY_FAILED);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN,_ALERT_PATIENT_CIRCUIT_DISCONNECTED);
	CLEAR_ALERT_BIT(FOURTH_FRAME_UN, _ALERT_HIGH_FIO2);
	CLEAR_ALERT_BIT(FOURTH_FRAME_UN, _ALERT_LOW_FIO2);
	CLEAR_ALERT_BIT(SECOND_FRAME_UN, _ALERT_LEAK);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_20_PER_BATTERY_DRAIN);
	CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_NEBULISER_ON);
    CLEAR_ALERT_BIT(FIRST_FRAME_UN, _ALERT_NEBULISER_OFF);
}



/**
 * @brief Convert_Battery_Percentage.
 * This function calculate the battery percentage in device.
 * */
void Convert_Battery_Percentage()
{
	if(Toggle == 0)
	{
		Battery_Voltage =  (uint8_t) (Bat_Avg / Bat_Avg_count);
		Battery_Voltage_old = Battery_Voltage;


		if(Battery_Voltage_new > Battery_Voltage)
		{
			Battery_Voltage = Battery_Voltage_new;
		}

		Battery_Voltage_new = Battery_Voltage;

	}
	else if(Toggle == 1)
	{
		Battery_Voltage =  (uint8_t) (Bat_Avg / Bat_Avg_count);
		if(Battery_Voltage_old < Battery_Voltage)
		{
			Battery_Voltage = Battery_Voltage_old;
		}
		Battery_Voltage_old = Battery_Voltage;
		Battery_Voltage_new = 0;
	}
	Bat_Avg         = 0;
	Bat_Avg_count   = 0;
}


/**
 * @brief Convert_Battery_Percentage.
 * This function calculate the analog value of battery percentage in device.
 * */
void Read_Battery_Voltage()
{
    if( (Running_Mode == Aprv)  )
    {
         if((BREATH_STATE == EXPIRATION_CYCLE))
         {
			Battery_Raw_Value  = ((Adc.AdcData[2]-3050)*100)/(750.0);           //pin number has changed for board pin changed
			if(Battery_Raw_Value>100)
			{
				Battery_Raw_Value=100;
			}
			Bat_Avg += Battery_Raw_Value;
			Bat_Avg_count++;
         }
    }
    else if(Run_Current_Breathe_State == Run_Expiration_Cycle)
    {

		Battery_Raw_Value  = ((Adc.AdcData[2]-3050)*100)/(750.0);                //pin number has changed for board pin changed
		if(Battery_Raw_Value>100)
		{
			Battery_Raw_Value=100;
		}
		Bat_Avg += Battery_Raw_Value;
		Bat_Avg_count++;
    }
    else if(Run_Current_Breathe_State == No_Run_State)
    {

		Battery_Raw_Value  = ((Adc.AdcData[2]-3050)*100)/(750.0);                //pin number has changed for board pin changed
		if(Battery_Raw_Value>100)
		{
			Battery_Raw_Value=100;
		}
		Bat_Avg += Battery_Raw_Value;
		Bat_Avg_count++;

		if(Bat_Avg_count > 5000)
		{
			AC_Supply_or_Battery_Supply_Indication();
			Convert_Battery_Percentage();
			if(Device_Monitoring  ==  NO)
			{
				Send_Bluetooth_Data_Farme = SEND_SAMPLED_PARAMETER_FRAME;
				Send_Frame                = First_Frame;
			}
		}
    }
}
