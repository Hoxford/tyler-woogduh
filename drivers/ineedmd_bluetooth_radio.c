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
#include <stdlib.h>
#include "utils_inc/string_add.h"
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
#include "ineedmd_bluetooth_radio.h"

//*****************************************************************************
// defines
//*****************************************************************************
#define SET_CONTROL_ECHO  "\r\nSET CONTROL ECHO x\r\n"
  #define SET_CONTROL_ECHO_BANNER_ON      0x01  //Bit 0 If this bit is set, the start-up banner is visible.
  #define SET_CONTROL_ECHO_BANNER_OFF     0x00
  #define SET_CONTROL_ECHO_COMMAND_ON     0x02  //Bit 1 If this bit is set, characters are echoed back to client in command mode.
  #define SET_CONTROL_ECHO_COMMAND_OFF    0x00
  #define SET_CONTROL_ECHO_SET_EVENT_ON   0x04  //Bit 2 This bit indicates if set events are displayed in command mode.
  #define SET_CONTROL_ECHO_SET_EVENT_OFF  0x00
  #define SET_CONTROL_ECHO_SYNTAX_ERR_ON  0x00  //Bit 3 If this bit is set, SYNTAX ERROR messages are disabled.
  #define SET_CONTROL_ECHO_SYNTAX_ERR_OFF 0x08
    #define SET_CONTROL_ECHO_SETTING  ( SET_CONTROL_ECHO_BANNER_OFF    \
                                      | SET_CONTROL_ECHO_COMMAND_OFF   \
                                      | SET_CONTROL_ECHO_SET_EVENT_OFF \
                                      | SET_CONTROL_ECHO_SYNTAX_ERR_ON)

#define SET_BT_NAME       "\r\nSET BT NAME IneedMD-00:07:80:0D:73:E0\r\n"
#define SET_BT_BDADDR     "\r\nSET BT BDADDR\r\n"
#define SET_BT_SSP_1      "\r\nSET BT SSP 1 1\r\n"
#define SET_PROFILE_SPP   "\r\nSET PROFILE SPP\r\n"
#define SET_CONTROL_BATT  "\r\nSET CONTOL BATTERY 3300 3100 3400 01\r\n"
#define SET_SET           "\r\nSET\r\n"

#define BG_SIZE  1024

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
//		GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_ENABLE, INEEDMD_PORTE_RADIO_ENABLE);
	  //power on the radio
    iRadio_Power_On();
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
void ineedmd_radio_reset(void)
{
  //exerts the processor rest pin
  GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_REST,  INEEDMD_PORTE_RADIO_REST );


	//as we dont know the state of the processor or the timers we will do the delay in cpu clock cycles
	//about a 10th of a second
	//it would be nicer to have this as a proces sleep..
	MAP_SysCtlDelay( MAP_SysCtlClockGet() / 30  );

	//de-exert,sets it low, reset pin
	GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_REST, 0x00);
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void ineedmd_radio_soft_reset(void)
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
void bluetooth_setup(void)
{
  uint32_t i;
  char cSet_Control;
  char  *send_string;
  char cRcv_buff[BG_SIZE];

  memset(cRcv_buff, 0x00, BG_SIZE);

  ineedmd_radio_power(true);

  // Enables the communication FIFO
//    UARTFIFOEnable(INEEDMD_RADIO_UART);
//    //And the Radio UART
//    UARTEnable(INEEDMD_RADIO_UART);
  //enable radio interface
  iRadio_interface_enable();

  //it would be nicer to have this as a proces sleep..
  MAP_SysCtlDelay( MAP_SysCtlClockGet() / 30  );

  ineedmd_radio_reset();

  //
  // Configure the UART for 115,200, 8-N-1 operation.
  // This function uses SysCtlClockGet() to get the system clock
  // frequency.  This could be also be a variable or hard coded value
  // instead of a function call.
  //
  UARTConfigSetExpClk( UART1_BASE, MAP_SysCtlClockGet(), 115200, ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE ));

  UARTEnable(UART1_BASE);

  //set the radio echo
  cSet_Control = itoa(SET_CONTROL_ECHO_SETTING, 16);

  ineedmd_radio_send_string(SET_CONTROL_ECHO);

  //tell the radio we are using BT SSP pairing
//  send_string = "SET BT SSP 1 1\n\r";
  send_string = SET_BT_SSP_1;
  for (i = 0; i<strlen(send_string); i++)
  {
    UARTCharPut(UART1_BASE, send_string[i]);
    while(UARTBusy(UART1_BASE)){}
  }

  //tells the radio we are using SPP protocol
  send_string = SET_PROFILE_SPP;
  for (i = 0; i<strlen(send_string); i++)
  {
    UARTCharPut(UART1_BASE, send_string[i]);
    while(UARTBusy(UART1_BASE)){}
  }

  // sets the battery mode for the radio,  configures the - low bat warning voltage - the low voltage lock out - the charge release voltage - that this signal is radio GPIO 01
  send_string = SET_CONTROL_BATT;
  for (i = 0; i<strlen(send_string); i++)
  {
    UARTCharPut(UART1_BASE, send_string[i]);
    while(UARTBusy(UART1_BASE)){}
  }

  //commits these changes to the radio
//  send_string = SET_SET;
//  for (i = 0; i<strlen(send_string); i++)
//  {
//    UARTCharPut(UART1_BASE, send_string[i]);
//    while(UARTBusy(UART1_BASE)){}
//  }

  //get BT address
  send_string = SET_BT_BDADDR;
  for (i = 0; i<strlen(send_string); i++)
  {
    UARTCharPut(UART1_BASE, send_string[i]);
    while(UARTBusy(UART1_BASE)){}
  }

  i = 0;
  for(i = 0; i != BG_SIZE; i++)
  {
    cRcv_buff[i] = UARTCharGet(UART1_BASE);
    if(cRcv_buff[i] == '\n')
    {
      i++;
      break;
    }
  }

  send_string = SET_BT_NAME;
  for (i = 0; i<strlen(send_string); i++)
  {
    UARTCharPut(UART1_BASE, send_string[i]);
    while(UARTBusy(UART1_BASE)){}
  }
}

/*
 * Get status from the radio
 */

/*
 * END
 */
