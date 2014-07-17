
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
#include "INEEDMD_LED.h"
#include <driverlib/i2c.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.H"

//
//a processor loop wait timer.  The number of cycles are calculated from the frequency of the main clock.
//
void wait_time (unsigned int tenths_of_seconds)
{
	MAP_SysCtlDelay(( MAP_SysCtlClockGet() / 30  )*tenths_of_seconds );
}


//
//a 2 BYTE i2C WRITE ROUTINE.  No error checking is implemented
//
void write_2_byte_i2c (unsigned char device_id, unsigned char first_byte, unsigned char second_byte)

{

	I2CMasterSlaveAddrSet(I2C0_BASE, device_id, false);
    I2CMasterDataPut(I2C0_BASE, first_byte);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C0_BASE));
    I2CMasterDataPut(I2C0_BASE, second_byte);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(I2CMasterBusy(I2C0_BASE));

}



void led_test(void)
{

    //ints to use in switch stmt

    ineedmd_led_pattern(POWER_ON_BATGOOD);
    ineedmd_led_pattern(POWER_ON_BATLOW);
    ineedmd_led_pattern(BAT_CHARGING);
    ineedmd_led_pattern(BT_CONNECTED);
    ineedmd_led_pattern(BT_ATTEMPTING);
    ineedmd_led_pattern(BT_FAILED);
    ineedmd_led_pattern(USB_CONNECTED);
    ineedmd_led_pattern(USB_FAILED);
    ineedmd_led_pattern(DATA_TRANSFER);
    ineedmd_led_pattern(TRANSFER_DONE);
    ineedmd_led_pattern(STORAGE_WARNING);
    ineedmd_led_pattern(LEAD_LOOSE);
    ineedmd_led_pattern(ACQUIRE_UPLOAD_DATA);
    ineedmd_led_pattern(PATIENT_ALERT);
    ineedmd_led_pattern(LED_OFF);

}
void bluetooth_test(void)
{
    uint32_t i;
	char  *send_string;
	char *recieve_string;
	

	//
	// Set radio to power up and raise the reset pin
	//
	GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_ENABLE, INEEDMD_PORTE_RADIO_ENABLE);
	GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_REST, 0x00);
	wait_time(2);
	GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_REST,  INEEDMD_PORTE_RADIO_REST );

	write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_RED_ENABLE | INEEDMD_LED2_BLUE_ENABLE));

	//
    // Configure the UART for 115,200, 8-N-1 operation.
    // This function uses SysCtlClockGet() to get the system clock
    // frequency.  This could be also be a variable or hard coded value
    // instead of a function call.
    //
    UARTConfigSetExpClk( UART1_BASE, SysCtlClockGet(), 115200, ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE ));
    //UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX7_8, UART_FIFO_RX7_8);
    //UARTFIFOEnable(UART1_BASE);
    //UARTFlowControlSet(UART1_BASE, UART_FLOWCONTROL_TX | UART_FLOWCONTROL_RX);
    UARTEnable(UART1_BASE);

	write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));

	while(1){
		send_string = "Hello world";
				for (i = 0; i<strlen(send_string); i++)
			{
				UARTCharPut(UART1_BASE, send_string[i]);
				while(UARTBusy(UART1_BASE)){}
			}
			wait_time(5);
	
	}
	send_string = "SET BT SSP 1 1\n";
		for (i = 0; i<strlen(send_string); i++)
	{
		UARTCharPut(UART1_BASE, send_string[i]);
		while(UARTBusy(UART1_BASE)){}
	}
	wait_time(5);

	write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE ));

	
	send_string = "SET PROFILE SPP\n";
		for (i = 0; i<strlen(send_string); i++)
	{
		UARTCharPut(UART1_BASE, send_string[i]);
		while(UARTBusy(UART1_BASE)){}
	}
	wait_time(5);
/*
	send_string = "SET",
		for (i = 0; i<strlen(send_string); i++)
	{
		UARTCharPut(UART1_BASE, send_string[i]);
		while(UARTBusy(UART1_BASE)){}
	}
	wait_time(5);
*/

	write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), ( INEEDMD_LED2_BLUE_ENABLE));


	while(1){}
	
}




/*
 * main.c
 */
int main(void) {

    volatile uint32_t ui32Loop;

	PortFunctionInit();
	write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_ALL_ILEVEL, 0x05);
	write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_ALL_PWM, 0x20);
	write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED2_BLUE_ENABLE | INEEDMD_LED2_GREEN_ENABLE) );

    while(1)

    {
// 		led_test();

//    	dfd();

    	bluetooth_test();
    }


	return(0);
}
