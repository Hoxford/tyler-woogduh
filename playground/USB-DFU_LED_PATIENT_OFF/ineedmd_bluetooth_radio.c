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
#define INEEDMD_LOW_BATT_VOLTAGE     3300
#define INEEDMD_SHTDWN_BATT_VOLTAGE  3100
#define INEEDMD_FULL_BATT_VOLTAGE    3400
#define INEEDMED_PIO_MASK            01

#define SET_CONTROL_ECHO  "\r\nSET CONTROL ECHO %d\r\n"
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

//#define SET_BT_NAME       "\r\nSET BT NAME IneedMD-00:07:80:0D:73:E0\r\n"
#define SET_BT_WHAT_NAME  "\r\nSET BT NAME IneedMD-NUTS\r\n"
#define SET_BT_NAME       "\r\nSET BT NAME IneedMD-%x%x\r\n"
#define SET_BT_BDADDR        "\r\nSET BT BDADDR\r\n"  //gets the BT address, yes gets it even though it says SET
#define SET_BT_BDADDR_PARSE  "%s %s %s %x %c %x %c %x %c %x %c %x %c %x"
#define SET_BT_SSP      "\r\nSET BT SSP %d %d\r\n"
  #define SET_BT_SSP_CPBLTES_DISP_ONLY    0  //Display only
  #define SET_BT_SSP_CPBLTES_DISP_AND_YN  1  //Display + yes/no button
  #define SET_BT_SSP_CPBLTES_KEYBRD_ONLY  2  //Keyboard only
  #define SET_BT_SSP_CPBLTES_NONE         3  //None
  #define SET_BT_SSP_MITM_NO_PROT    0  //no man in the middle protection
  #define SET_BT_SSP_MITM_PROT       1  //man in the middle protection required
#define SET_PROFILE_SPP   "\r\nSET PROFILE SPP\r\n"
#define SET_CONTROL_BATT  "\r\nSET CONTROL BATTERY %d %d %d %.2d\r\n"
  #define SET_CONTROL_BATT_LOW     INEEDMD_LOW_BATT_VOLTAGE
  #define SET_CONTROL_BATT_SHTDWN  INEEDMD_SHTDWN_BATT_VOLTAGE
  #define SET_CONTROL_BATT_FULL    INEEDMD_FULL_BATT_VOLTAGE
  #define SET_CONTROL_BATT_MASK    INEEDMED_PIO_MASK
#define SET_SET           "\r\nSET\r\n"

#define BG_SIZE  64

/*
 * Function Section
 *
 */
int iIneedmd_radio_rcv_string(char *cRcv_string);
int iIneedmd_parse_addr(char * cString_buffer, uint8_t * uiAddr);

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
int iIneedmd_radio_rcv_string(char *cRcv_string)
{
  int i;
  for(i = 0; i != BG_SIZE; i++)
  {
    cRcv_string[i] = UARTCharGet(UART1_BASE);
    if(cRcv_string[i] == '\n')
    {
      i++;
      break;
    }
  }
  return i;
}

/*
 * parses a received string for a BT address
 */
int iIneedmd_parse_addr(char * cString_buffer, uint8_t * uiAddr)
{
  uint8_t iAddr_1, iAddr_2, iAddr_3, iAddr_4, iAddr_5, iAddr_6;
  char cThrow_away[10];
  sscanf ( cString_buffer, SET_BT_BDADDR_PARSE, cThrow_away, cThrow_away, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr);
  return 1;
}


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

//  char  *send_string = NULL;
  char cSend_buff[BG_SIZE];
  char cRcv_buff[BG_SIZE];
  uint8_t uiBT_addr[6];
  memset(cSend_buff, 0x00, BG_SIZE);
  memset(cRcv_buff, 0x00, BG_SIZE);

  //turn power on
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
  snprintf(cSend_buff, BG_SIZE, SET_CONTROL_ECHO, SET_CONTROL_ECHO_SETTING);
  ineedmd_radio_send_string(cSend_buff);

  //tell the radio we are using BT SSP pairing
  memset(cSend_buff, 0x00, BG_SIZE);
  snprintf(cSend_buff, BG_SIZE, SET_BT_SSP, SET_BT_SSP_CPBLTES_DISP_AND_YN, SET_BT_SSP_MITM_PROT);
  ineedmd_radio_send_string(cSend_buff);
//  send_string = SET_BT_SSP_1;
//  for (i = 0; i<strlen(send_string); i++)
//  {
//    UARTCharPut(UART1_BASE, send_string[i]);
//    while(UARTBusy(UART1_BASE)){}
//  }

  //tells the radio we are using SPP protocol
  ineedmd_radio_send_string(SET_PROFILE_SPP);

  // sets the battery mode for the radio,  configures the - low bat warning voltage - the low voltage lock out - the charge release voltage - that this signal is radio GPIO 01
  memset(cSend_buff, 0x00, BG_SIZE);
  snprintf(cSend_buff, BG_SIZE, SET_CONTROL_BATT, SET_CONTROL_BATT_LOW, SET_CONTROL_BATT_SHTDWN, SET_CONTROL_BATT_FULL, SET_CONTROL_BATT_MASK);
  ineedmd_radio_send_string(cSend_buff);
//  send_string = SET_CONTROL_BATT;
//  for (i = 0; i<strlen(send_string); i++)
//  {
//    UARTCharPut(UART1_BASE, send_string[i]);
//    while(UARTBusy(UART1_BASE)){}
//  }

  //commits these changes to the radio
//  send_string = SET_SET;
//  for (i = 0; i<strlen(send_string); i++)
//  {
//    UARTCharPut(UART1_BASE, send_string[i]);
//    while(UARTBusy(UART1_BASE)){}
//  }

  //get BT address
  ineedmd_radio_send_string(SET_BT_BDADDR);
//  for (i = 0; i<strlen(send_string); i++)
//  {
//    UARTCharPut(UART1_BASE, send_string[i]);
//    while(UARTBusy(UART1_BASE)){}
//  }

//  i = 0;
//  memset(cSend_buff, 0x00, BG_SIZE);
//  for(i = 0; i != BG_SIZE; i++)
//  {
//    cSend_buff[i] = UARTCharGet(UART1_BASE);
//    if(cSend_buff[i] == '\n')
//    {
//      i++;
//      break;
//    }
//  }
  i = iIneedmd_radio_rcv_string(cRcv_buff);

  //parse string for the BT address
  iIneedmd_parse_addr(cRcv_buff, uiBT_addr);
  memset(cSend_buff, 0x00, BG_SIZE);
  snprintf(cSend_buff, BG_SIZE, SET_BT_NAME, uiBT_addr[4], uiBT_addr[5]);

  ineedmd_radio_send_string(cSend_buff);
//  send_string = SET_BT_NAME;
//  for (i = 0; i<strlen(send_string); i++)
//  {
//    UARTCharPut(UART1_BASE, send_string[i]);
//    while(UARTBusy(UART1_BASE)){}
//  }
}

/*
 * Get status from the radio
 */

/*
 * END
 */
