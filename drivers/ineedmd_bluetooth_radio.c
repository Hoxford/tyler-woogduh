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
#define SET_BT_BDADDR        "SET BT BDADDR\r\n"  //gets the BT address, yes gets it even though it says SET
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
#define SET_SET           "\r\nSET\r\n"  //retruns the settings of the BT radio *warning: large return string*
#define SET_RESET         "\r\nSET RESET\r\n"  //resets the radio to factory default settings
#define SSP PASSKEY       "\r\nSSP PASSKEY %x : %x : %x : %x : %x : %x "

#define BG_SIZE  256

/******************************************************************************
* variables
******************************************************************************/
volatile bool bIs_data;
volatile bool bIs_connection = false;

/*
 * Function Section
 *
 */
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
		GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_RADIO_ENABLE_PIN, 0x00);

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
  GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_RADIO_RESET_PIN,  INEEDMD_RADIO_RESET_PIN );


	//as we dont know the state of the processor or the timers we will do the delay in cpu clock cycles
	//about a 10th of a second
	//it would be nicer to have this as a proces sleep..
	MAP_SysCtlDelay( MAP_SysCtlClockGet() / 30  );

	//de-exert,sets it low, reset pin
	GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_RADIO_RESET_PIN, 0x00);
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
 * This function in it's current state just passes the parameters to the HAL.
 * It is meant to be here for future api, rtos, parsing use.
 */
void ineedmd_radio_send_string(char *send_string, uint16_t uiBuff_size)
{
  int i;

  i = iRadio_send_string(send_string, uiBuff_size);
#if DEBUG
  if(i < strlen(send_string))
  {
    while(1);
  }
#endif //DEBUG
}

/*
 * get message from the radio
 */
int iIneedmd_radio_rcv_string(char *cRcv_string, uint16_t uiBuff_size)
{
  int i;

  i = iRadio_rcv_string(cRcv_string, uiBuff_size);

  return i;
}

/*
 * parses a received string for a BT address
 */
int iIneedmd_parse_addr(char * cString_buffer, uint8_t * uiAddr)
{
  char cThrow_away[10];
  memset(cThrow_away, 0x00, 10);
  int iError;
  iError = sscanf ( cString_buffer, SET_BT_BDADDR_PARSE, cThrow_away, cThrow_away, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr);

  //check if the raw BT address string from the modem is in the correct format
  if(iError != 14)  //todo: remove this magic number
  {
    return 0;
  }
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
int  iIneedMD_radio_setup(void)
{
  uint32_t iEC;

//  char  *send_string = NULL;
  char cSend_buff[BG_SIZE];
  char cRcv_buff[BG_SIZE];
  uint8_t uiBT_addr[6];
  memset(cSend_buff, 0x00, BG_SIZE);
  memset(cRcv_buff, 0x00, BG_SIZE);

  //turn power on to the radio
  ineedmd_radio_power(true);

  //enable radio interface
  iRadio_interface_enable();

  //hardware power reset the radio
  ineedmd_radio_reset();

  //reset the radio to factory defaults
  ineedmd_radio_send_string(SET_RESET, strlen(SET_RESET));
  iHW_delay(10);
  //hardware power reset the radio
  ineedmd_radio_reset();

  //set the radio echo
  snprintf(cSend_buff, BG_SIZE, SET_CONTROL_ECHO, SET_CONTROL_ECHO_SETTING);
  ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));

  //tell the radio we are using BT SSP pairing
  memset(cSend_buff, 0x00, BG_SIZE);
  snprintf(cSend_buff, BG_SIZE, SET_BT_SSP, SET_BT_SSP_CPBLTES_DISP_AND_YN, SET_BT_SSP_MITM_PROT);
  ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));

  //tells the radio we are using SPP protocol
  ineedmd_radio_send_string(SET_PROFILE_SPP, strlen(SET_PROFILE_SPP));

  // sets the battery mode for the radio,  configures the - low bat warning voltage - the low voltage lock out - the charge release voltage - that this signal is radio GPIO 01
  memset(cSend_buff, 0x00, BG_SIZE);
  iEC = snprintf(cSend_buff, BG_SIZE, SET_CONTROL_BATT, SET_CONTROL_BATT_LOW, SET_CONTROL_BATT_SHTDWN, SET_CONTROL_BATT_FULL, SET_CONTROL_BATT_MASK);

  ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));

  //get BT address
  ineedmd_radio_send_string(SET_BT_BDADDR, strlen(SET_BT_BDADDR));
  memset(cRcv_buff, 0x00, BG_SIZE);
  iEC = iIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE);

  //parse string for the BT address
  iEC = iIneedmd_parse_addr(cRcv_buff, uiBT_addr);
  if(iEC != 1)  //todo: proper error code, no magic number
  {
#if DEBUG
    while(1);
#endif
  }
  memset(cSend_buff, 0x00, BG_SIZE);

  //format the send string with the BT address
  snprintf(cSend_buff, BG_SIZE, SET_BT_NAME, uiBT_addr[4], uiBT_addr[5]);
  //send the new address
  ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));

  //set the connection status to false while waiting for an outside connection
  bIs_connection = false;

  //enable the interrupt to the radio
  iRadio_interface_int_enable();

  return 1;
}

/*
 * Check for BT connection
 */
int  iIneedMD_radio_check_for_connection(void)
{

  return 1;
}

/*
 * Main radio process
 */
int iIneedMD_radio_process(void)
{
  bool bIs_connection = false;
  bool bIs_data = false;

  if(bIs_connection == true)
  {

    if(bIs_data == true)
    {

    }
  }
  return 1;
}
/*
 * Get status from the radio
 */

/*
 * END
 */
