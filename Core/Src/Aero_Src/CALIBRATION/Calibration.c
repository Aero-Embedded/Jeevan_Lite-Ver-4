/*
 * Calibration.c
 *
 *  Created on: Mar 16, 2023
 *      Author: asus
 */


#include "Calibration.h"

int Count=0;
float Temp_Pressure_Val_1;
float Temp_Pressure_Val_2;
float Total_temp_Pressure_Val;

extern int Blower_RPM;
int O2_calib_Count=0;
uint8_t Loop;

float Minimum_Oxygen_Value,Maximum_Oxygen_Value,deviation,Deviation_Decimal;
extern SENSOR MPX5010;
extern SENSOR MPX7002;

uint16_t Calibration_Blower_Dac=400;
uint16_t Calibration_Proportional_valve_Dac=1900;
uint8_t Step=1;
uint16_t Old_offset;



void Split_Calibration_Packets(CALIBRATION_PACKET * Receive_Graph_Packet)
{
	CALIBRARTION.BLOWER                  =  (Receive_Graph_Packet->BLOWER_CALIBRATION);
	CALIBRARTION.PRESSURE_SENSOR         =  (Receive_Graph_Packet->PRESSURE_SENSOR_CALIBRATION);
	CALIBRARTION.FLOW_SENSOR_7002        =  (Receive_Graph_Packet->FLOW_SENSOR_7002_CALIBRATION);
	CALIBRARTION.VALVE_LEAK_TEST         =  (Receive_Graph_Packet->VALVE_LEAK_TEST_CALIBRATION);
	CALIBRARTION.O2_CHECK                =  (Receive_Graph_Packet->O2_CHECK_CALIBRATION);
	CALIBRARTION.ALARAM_TEST             =  (Receive_Graph_Packet->LED_ALARAM_CALIBRATION);
	CALIBRARTION.BATTERY_TEST            =  (Receive_Graph_Packet->BATTERY_CALIBRATION);
	CALIBRARTION.SENSOR_OFFSET_TEST      =  (Receive_Graph_Packet->SENSOR_OFFSET_CALIBRATION);
	CALIBRARTION.O2_SENSOR_100_PER_TEST  =  (Receive_Graph_Packet->O2_SENSOR_100_PER_CALIBRATE);
    Device_Monitoring                    =  YES;

	Max_Flow = 0;
	Min_Flow = 0;

	 if(CALIBRARTION.O2_CHECK == 1)
	 {
		 Step=1;

	 }
	vTaskResume(Calibration_Handler);
}



uint8_t chksum8_cal(const unsigned char *buff, size_t len)
{
    unsigned int sum;
    for ( sum = 0 ; len != 0 ; len-- )
        sum += *(buff++);
    return (uint8_t)sum;
}


void Send_Calibrated_Packet()
{

	SEND_CALIBRATION_PACKET._header = Send_BlueTooth_Calibration_Header;
	SEND_CALIBRATION_PACKET._length = 7;
	SEND_CALIBRATION_PACKET._CRC8   = chksum8_cal((unsigned char*)&SEND_CALIBRATION_PACKET.PASS_FAIL,(SEND_CALIBRATION_PACKET._length-1));
	HAL_UART_Transmit_IT(&huart6,(uint8_t*)&SEND_CALIBRATION_PACKET,sizeof(SEND_CALIBRATION_PACKET));
	Send_Bluetooth_Data_Farme = SEND_GRAPH_FRAME;

}

void CALIBRATION_Task(void *argument)
{

	while(1)
	{

		if(CALIBRARTION.BLOWER  == START )
		{
			Blower_Calibration();
		}
		else if(CALIBRARTION.PRESSURE_SENSOR == START)
		{
			Pressure_Sensor_Calibration();
		}
		else if(CALIBRARTION.FLOW_SENSOR_7002 == START)
		{
			Flow_Sensor_7002_Calibration();
		}
		else if(CALIBRARTION.VALVE_LEAK_TEST == START)
		{
			Expiratory_Valve_Leak_Calibration();
		}
		else if(CALIBRARTION.O2_CHECK == START)
		{
			Oxygen_blending_Calibration();
		}
		else if(CALIBRARTION.ALARAM_TEST == START)
		{
			Led_Buzzer_Calibration();
		}
		else if(CALIBRARTION.BATTERY_TEST == START)
		{
			Battery_Calibration();
		}
		else if(CALIBRARTION.SENSOR_OFFSET_TEST == START)
		{
			Sensor_Voltage_Transmit();
		}
		else if(CALIBRARTION.O2_SENSOR_100_PER_TEST == START)
		{
			 vTaskResume(O2_Calibration_Handler);
		}


		vTaskDelay(2);
	}
}



void Blower_Calibration()
{

	Count++;
    Blower_ON();
    ExpValve_CLOSE();
	Blower_Signal(4095);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);             // start blower Rpm
	SEND_CALIBRATION_PACKET.RESULT              = GRAPH_VALUES.pressure;
	 if(Count > 1500)
	 {
		 /*if(Blower_RPM <55000)
		 {
				SEND_CALIBRATION_PACKET.RESULT              = GRAPH_VALUES.pressure ;
				Blower_Signal(0);
				ExpValve_OPEN();
				vTaskDelay(2000);
				SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_FAIL ;
				Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
				vTaskDelay(Twenty_MilliSecond_Delay);
				Count                                       = RESET;
				CALIBRARTION.BLOWER                         = STOP;
				vTaskSuspend(Calibration_Handler);
		 }*/
		 if ((GRAPH_VALUES.pressure >=60)  )
		 {
				SEND_CALIBRATION_PACKET.RESULT              = GRAPH_VALUES.pressure ;
				Blower_Signal(0);
				ExpValve_OPEN();
				vTaskDelay(2000);
				SEND_CALIBRATION_PACKET.PASS_FAIL              = CALIBRATION_PASS ;
				Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
				vTaskDelay(Twenty_MilliSecond_Delay);
				Count                                       = RESET;
				CALIBRARTION.BLOWER                         = STOP;
				Device_Monitoring                           =  NO;
				vTaskSuspend(Calibration_Handler);

		 }
		 /*else if ((Max_Flow >=60) || ( Blower_RPM > 55000) )
		 {
				SEND_CALIBRATION_PACKET.RESULT              = GRAPH_VALUES.pressure ;
				Blower_Signal(0);
				ExpValve_OPEN();
				vTaskDelay(2000);
				SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_PASS ;
				Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
				vTaskDelay(Twenty_MilliSecond_Delay);
				Count                                       = RESET;
				CALIBRARTION.BLOWER                         = STOP;
				vTaskSuspend(Calibration_Handler);

		 }*/
		 else
		 {
				SEND_CALIBRATION_PACKET.RESULT              = GRAPH_VALUES.pressure ;
				Blower_Signal(0);
				ExpValve_OPEN();
				vTaskDelay(2000);
				SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_FAIL ;
				Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
				vTaskDelay(Twenty_MilliSecond_Delay);
				Count                                       = RESET;
				CALIBRARTION.BLOWER                         = STOP;
				Device_Monitoring                           =  NO;
				vTaskSuspend(Calibration_Handler);
		 }

	  }
}



void Pressure_Sensor_Calibration()
{
	if(GRAPH_VALUES.pressure<60)
	{
		Count++;
		Blower_ON();
		ExpValve_CLOSE();
		Blower_Signal(4095);
		SEND_CALIBRATION_PACKET.RESULT              = GRAPH_VALUES.pressure;
	}
	else
	{
		if ((GRAPH_VALUES.pressure >=60) )
		{
		       HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_1);             // stop blower Rpm
				SEND_CALIBRATION_PACKET.RESULT              = GRAPH_VALUES.pressure ;
				Blower_Signal(0);
				ExpValve_OPEN();
				vTaskDelay(2000);
				SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_PASS ;
				Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
				vTaskDelay(Twenty_MilliSecond_Delay);
				Count                                       = RESET;
				CALIBRARTION.PRESSURE_SENSOR                = STOP;
				Device_Monitoring                           =  NO;
				vTaskSuspend(Calibration_Handler);
		}
		/*else
		{
		    HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_1);             // stop blower Rpm
			SEND_CALIBRATION_PACKET.RESULT             = GRAPH_VALUES.pressure ;
			Blower_Signal(0);
			ExpValve_OPEN();
			vTaskDelay(2000);
			SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_FAIL ;
			Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
			vTaskDelay(Twenty_MilliSecond_Delay);
			Count                                       = RESET;
			CALIBRARTION.PRESSURE_SENSOR                = STOP;
			vTaskSuspend(Calibration_Handler);
		}*/
	 }
	 if(Count > 1500)
	 {
		HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_1);             // stop blower Rpm
		SEND_CALIBRATION_PACKET.RESULT              = GRAPH_VALUES.pressure ;
		Blower_Signal(0);
		ExpValve_OPEN();
		vTaskDelay(2000);
		SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_FAIL ;
		Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
		vTaskDelay(Twenty_MilliSecond_Delay);
		Count                                       = RESET;
		CALIBRARTION.PRESSURE_SENSOR                = STOP;
		Device_Monitoring                           =  NO;
		vTaskSuspend(Calibration_Handler);
	  }
}


void Flow_Sensor_7002_Calibration()
{
	  if (GRAPH_VALUES.flow < 20  )
	  {
		  Count++;
		  Blower_ON();
		  ExpValve_OPEN();
		  Blower_Signal(2000);
		  SEND_CALIBRATION_PACKET.RESULT              = GRAPH_VALUES.flow ;
	  }
	  else
	  {
		   SEND_CALIBRATION_PACKET.RESULT              = GRAPH_VALUES.flow ;
		   Blower_Signal(0);
		   ExpValve_OPEN();
		   vTaskDelay(2000);
		   SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_PASS ;
		   Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
		   vTaskDelay(Twenty_MilliSecond_Delay);
		   Count                                       = RESET;
		   CALIBRARTION.FLOW_SENSOR_7002               = STOP;
		   Device_Monitoring                           =  NO;
		   vTaskSuspend(Calibration_Handler);
	   }
	   if (Count > 4000)
	   {
			SEND_CALIBRATION_PACKET.RESULT                = GRAPH_VALUES.flow ;
			Blower_Signal(0);
			ExpValve_OPEN();
			vTaskDelay(2000);
			SEND_CALIBRATION_PACKET.PASS_FAIL  = CALIBRATION_FAIL ;
			Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
			vTaskDelay(Twenty_MilliSecond_Delay);
			Count                                       = RESET;
			CALIBRARTION.FLOW_SENSOR_7002               = STOP;
			Device_Monitoring                           =  NO;
			vTaskSuspend(Calibration_Handler);
		}
}



void Expiratory_Valve_Leak_Calibration()
{

	Blower_ON();
	ExpValve_CLOSE();
	Blower_Signal(DAC_SIGNALS.DAC_VALUE_20 );

	vTaskDelay(4000);
	Blower_Signal(0);
	vTaskDelay(2000);
	Temp_Pressure_Val_1 =  GRAPH_VALUES.pressure;

	Blower_Signal(0);
	vTaskDelay(8000);
	Temp_Pressure_Val_2 =GRAPH_VALUES.pressure;
	ExpValve_OPEN();
	vTaskDelay(2000);

	if(Temp_Pressure_Val_1 < Temp_Pressure_Val_2)
	{
		Temp_Pressure_Val_2=Temp_Pressure_Val_1;

	}
	else
	{
		Total_temp_Pressure_Val = (Temp_Pressure_Val_1-Temp_Pressure_Val_2);
	}


	if(Total_temp_Pressure_Val<=5)
	{
		if(Temp_Pressure_Val_1 > 5)
		{
			SEND_CALIBRATION_PACKET.PASS_FAIL          = CALIBRATION_PASS ;
			SEND_CALIBRATION_PACKET.RESULT             = Total_temp_Pressure_Val ;
			Send_Bluetooth_Data_Farme                  = SEND_CALIBRATION_FRAME;
			vTaskDelay(Twenty_MilliSecond_Delay);
			CALIBRARTION.VALVE_LEAK_TEST               = STOP;
			ExpValve_OPEN();
			Device_Monitoring                           =  NO;
			vTaskSuspend(Calibration_Handler);
		}
		else
		{
			SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_FAIL ;
			SEND_CALIBRATION_PACKET.RESULT              = Total_temp_Pressure_Val ;
			Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
			vTaskDelay(Twenty_MilliSecond_Delay);
			CALIBRARTION.VALVE_LEAK_TEST                = STOP;
			ExpValve_OPEN();
			Device_Monitoring                           =  NO;
			vTaskSuspend(Calibration_Handler);
		}
	}
	else
	{
		SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_FAIL ;
		SEND_CALIBRATION_PACKET.RESULT              = Total_temp_Pressure_Val ;
		Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
		vTaskDelay(Twenty_MilliSecond_Delay);
		CALIBRARTION.VALVE_LEAK_TEST                = STOP;
		ExpValve_OPEN();
		Device_Monitoring                           =  NO;
		vTaskSuspend(Calibration_Handler);
	}
}









void Led_Buzzer_Calibration()
{

	Oxygen_Parameter.Achieved_Oxygen = 21;
		Red_Led_ON();
		Blue_Led_OFF();
		Green_Led_OFF();
	vTaskDelay(2000);
	    Red_Led_OFF();
		Blue_Led_ON();
		Green_Led_OFF();
	vTaskDelay(2000);
	    Red_Led_OFF();
	 	Blue_Led_OFF();
	 	Green_Led_ON();
	vTaskDelay(2000);
	    Buzzer1_ON();
	vTaskDelay(2000);
	    Buzzer1_ON();
	vTaskDelay(2000);
	    Red_Led_OFF();
		Blue_Led_OFF();
		Green_Led_OFF();
		Buzzer1_OFF();
		Buzzer2_OFF();
	vTaskDelay(400);

    	SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_PASS ;
		Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
		vTaskDelay(Twenty_MilliSecond_Delay);
		CALIBRARTION.ALARAM_TEST                    = STOP;
		Device_Monitoring                           =  NO;
	    vTaskSuspend(Calibration_Handler);

}



void Battery_Calibration()
{
	if(Battery_Voltage > 50)
	{
		vTaskDelay(2000);
		SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_PASS ;
		SEND_CALIBRATION_PACKET.RESULT              = Battery_Voltage;
		Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
		vTaskDelay(Twenty_MilliSecond_Delay);
		CALIBRARTION.BATTERY_TEST                   = STOP;
		Device_Monitoring                           =  NO;
		vTaskSuspend(Calibration_Handler);
	}
	else
	{
		Count++;
		if(Count>2500)
		{
			Count=0;
			SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_FAIL ;
			SEND_CALIBRATION_PACKET.RESULT              = Battery_Voltage;
			Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
			vTaskDelay(Twenty_MilliSecond_Delay);
			CALIBRARTION.BATTERY_TEST                   = STOP;
			Device_Monitoring                           =  NO;
			vTaskSuspend(Calibration_Handler);

		}
	}
}


void Sensor_Voltage_Transmit()
{
	MPX7002.offset                               = MPX7002.current_reading;
	MPX5010.offset                               = MPX5010.current_reading;


	SEND_CALIBRATION_PACKET.ERROR_CODE           = MPX5010.current_reading;
	if(MPX5010.current_reading >=255)
		SEND_CALIBRATION_PACKET.ERROR_CODE =255;
	SEND_CALIBRATION_PACKET.RESULT               = MPX7002.current_reading;
	SEND_CALIBRATION_PACKET.PASS_FAIL            = CALIBRATION_PASS ;
	Send_Bluetooth_Data_Farme                    = SEND_CALIBRATION_FRAME;
	vTaskDelay(Twenty_MilliSecond_Delay);
	CALIBRARTION.SENSOR_OFFSET_TEST              = STOP;
	Device_Monitoring                           =  NO;
	vTaskSuspend(Calibration_Handler);
}





void O2_Calibration_Task(void *argument)
{

	while(1)
	{

		//Oxygen_blending_Calibration();
	}
}



void Oxygen_blending_Calibration()
{
	if(Step == 1)
	{

		Old_offset = OOM202.offset_maximum;
		GRAPH_VALUES.Maximum_oxygen_flow  =  0;
		ExpValve_CLOSE();
		Count++;
		Blower_Signal(Calibration_Blower_Dac);
		Parkar_valve_Signal(0);
		OOM202.offset_minimum = OOM202.current_reading;
		if(Count > 10000)
		{
			OOM202.offset_minimum = OOM202.current_reading;
			controlsys_Init(&Parker_valve_tune);          //parker
			Count = 0;
			Blower_Signal(Calibration_Blower_Dac);
			Step = 2;
			vTaskDelay(Two_Millisecond_Delay);

		}
	}
	else if(Step == 2)
	{
		ExpValve_CLOSE();
		Count++;
		Blower_Signal(Calibration_Blower_Dac);

		if(  (GRAPH_VALUES.oxygen_flow < (GRAPH_VALUES.flow + 20)) )
		{
			Calibration_Proportional_valve_Dac=controlsys_Update(&Parker_valve_tune, (GRAPH_VALUES.flow + 20),GRAPH_VALUES.oxygen_flow);
			Parkar_valve_Signal(Calibration_Proportional_valve_Dac);
		}
		else
		{
			Calibration_Proportional_valve_Dac=controlsys_Update(&Parker_valve_tune, (GRAPH_VALUES.flow + 20),GRAPH_VALUES.oxygen_flow);
			Parkar_valve_Signal(Calibration_Proportional_valve_Dac);
		}

		Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;
		OOM202.offset_maximum = OOM202.current_reading;
		if(Count > 10000)
		{

			OOM202.offset_maximum = OOM202.current_reading;

			if(GRAPH_VALUES.Maximum_oxygen_flow  <10)
			{
				OOM202.offset_maximum = Old_offset;
			}

			Oxygen_Parameter.Achieved_Oxygen= GRAPH_VALUES.oxygen;
			if(Oxygen_Parameter.Achieved_Oxygen >=100)
			    Oxygen_Parameter.Achieved_Oxygen = 100;
			if(Oxygen_Parameter.Achieved_Oxygen<=20)
				Oxygen_Parameter.Achieved_Oxygen = 20;


			if(Oxygen_Parameter.Achieved_Oxygen <90)
			{
			    SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_FAIL ;
			}
			else
			{
				SEND_CALIBRATION_PACKET.PASS_FAIL           = CALIBRATION_PASS ;
			}
			Count = 0;
			Blower_Signal(0);
			Parkar_valve_Signal(0);
			Step = 3;
			vTaskDelay(Two_Millisecond_Delay);


		}

	}
	else if(Step == 3)
	{
		Blower_Signal(Calibration_Blower_Dac);
		ExpValve_CLOSE();
		Count++;
		OOM202.offset_minimum = OOM202.current_reading;
		if(Count > 10000)
		{
			OOM202.offset_minimum = OOM202.current_reading;
			Blower_Signal(0);
			Parkar_valve_Signal(0);
			ExpValve_OPEN();
			Count=0;
			Step=4;

			SEND_CALIBRATION_PACKET.RESULT              = Oxygen_Parameter.Achieved_Oxygen ;
			SEND_CALIBRATION_PACKET.ERROR_CODE          = OOM202.offset_minimum;
			SEND_CALIBRATION_PACKET.STAGE               = OOM202.offset_maximum;

			Send_Bluetooth_Data_Farme                   = SEND_CALIBRATION_FRAME;
			vTaskDelay(Twenty_MilliSecond_Delay);
			CALIBRARTION.O2_CHECK                       = STOP;
			GRAPH_VALUES.Maximum_oxygen_flow            = 0;
			Device_Monitoring                           =  NO;
			vTaskSuspend(Calibration_Handler);
		}
	}

	vTaskDelay(Two_Millisecond_Delay);
}
