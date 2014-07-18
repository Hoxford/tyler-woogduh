/*
 * ineedmd_bluetooth_radio.c
 *
 *  Created on: Jul 7, 2014
 *      Author: BrianS
 */



/*
 * Include Section
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/uart.h"
#include "driverlib/debug.h"

//Pull the processor
#include <inc/tm4c1233h6pm.h>

#include "board.h"


/*
 * Function Section
 *
 */


/* ineedmd_radio_power
 * Power up and down the radio
 *
 * This function simply writes the GPIO pin that controls the power state of the radio.
 * It does not toggle the state - just in case the power state becomed confused
 * The funcion is passed a boolina that controls the power state.
 * Alos shuts down the UART.. there is no point in having the UART powered if there is no radio to talk to.
 */
void ineedmd_radio_power(bool power_up)
{

	if (power_up == true)
	{
		// sets the pin high if the passed value is true
		GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_ENABLE, INEEDMD_PORTE_RADIO_ENABLE);
		// Enables the communication FIFO
		UARTFIFOEnable(INEEDMD_RADIO_UART);
		//And the Radio UART
		UARTEnable(INEEDMD_RADIO_UART);
	}
	else
	{
		//otherwise sets it to low
		GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_ENABLE, 0x00);
		// Disables the UART
		UARTDisable(INEEDMD_RADIO_UART);
		// then get rid of the FIFO
		UARTFIFODisable(INEEDMD_RADIO_UART);

	}
}

/* ineedmd_radio_reset
 * Reset the radio
 *
 * toggles the reset pin for the radio
 */
void ineedmd_radio_rest(void)
{
	//sets it low
	GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_REST, 0x00);
	//as we dont know the state of the processor or the timers we will do the delay in cpu clock cycles
	//about a 10th of a second
	//it would be nicer to have this as a proces sleep..
	MAP_SysCtlDelay( MAP_SysCtlClockGet() / 30  );
	//de-exerts the processor rest pin
	GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_REST,  INEEDMD_PORTE_RADIO_REST );
}


void ineedmd_radio_soft_rest(void)
{
	//assert a uart break
	UARTBreakCtl (INEEDMD_RADIO_UART, true);
	//as we dont know the state of the processor or the timers we will do the delay in cpu clock cycles
	//about a 10th of a second
	//it would be nicer to have this as a proces sleep..
	MAP_SysCtlDelay( MAP_SysCtlClockGet() / 30  );
	//de-exerts the processor rest pin
	UARTBreakCtl (INEEDMD_RADIO_UART, false);
}


/*
 * send message to the radio
 */
void ineedmd_radio_send_string(char *send_string)
{
	uint32_t i;
	for (i = 0; i<strlen(send_string); i++)
	{
		UARTCharPut(INEEDMD_RADIO_UART, send_string[i]);
	}
}
/*
 * get message from the radio
 */


/*
 * Check the battery voltage
 */



/*
 * Is the USB plugged in
 */

/*
 * Setup the radio
 */


/*
 * Get status from the radio
 */

/*
 * END
 */
