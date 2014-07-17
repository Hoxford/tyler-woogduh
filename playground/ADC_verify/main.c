
#ifndef PART_TM4C1233H6PM
 #define PART_TM4C1233H6PM
#endif

#ifndef TARGET_IS_TM4C1233H6PM
 #define TARGET_IS_TM4C1233H6PM
#endif

// Set us to debug mode
#define DEBUG_CODE
//


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <driverlib/rom.h>

//Pull the processor
#include <inc/tm4c1233h6pm.h>


//processor pin config files
#include "board.h"
#include <driverlib/i2c.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
//!!new
#include "INEEDMD_ADC.h"

//
//a processor loop wait timer.  The number of cycles are calculated from the frequency of the main clock.
//
void wait_time (unsigned int tenths_of_seconds)
{
	MAP_SysCtlDelay(( MAP_SysCtlClockGet() / 30  )*tenths_of_seconds );
}


/*
 * main.c
 */
int main(void) {

    volatile uint32_t ui32Loop;

    uint32_t id = 3;

	PortFunctionInit();
	INEEDMD_ADC_Start_Low();
	//power on ADC, disable continuous conversions
	INEEDMD_ADC_Power_On();
	//turn off continuous conversion for register read/writes
	INEEDMD_ADC_Stop_Continuous_Conv();

	id = INEEDMD_ADC_Get_ID();

	INEEDMD_ADC_Start_Internal_Reference();
	INEEDMD_ADC_Enable_Lead_Detect();
	uint32_t val2 = INEEDMD_ADC_Check_RLD_Lead();
	wait_time(1);

	//TODO: setup a continuous data read using interrupt
	//use INEEDMD_ADC_Get_Conversion to store reading into global variable
	INEEDMD_ADC_Register_Write(CH8SET, PD1);
	//start conversions
	INEEDMD_ADC_Start_High();
	char result[INEEDMD_ADC_DATA_SIZE];

	while(1)
	{
		//single shot data read

		//wait for conversion to be ready
		while(GPIOPinRead(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_INTERUPT_PIN))
		{
		}

		//request data
		INEEDMD_ADC_Request_Data();

		//read result

		INEEDMD_ADC_Receive_Data(result);
		uint32_t lead;

		lead = INEEDMD_ADC_Check_Lead_Off();
	}
	/*
	INEEDMD_ADC_Start_Continuous_Conv();

	while(1)
	{
		//single shot data read

		//wait for conversion to be ready
		while(GPIOPinRead(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_INTERUPT_PIN))
		{
		}
		//read result

		INEEDMD_ADC_Receive_Data(result);
		uint32_t lead;

		lead = INEEDMD_ADC_Check_Lead_Off();
	}
	 */
	return(0);
}
