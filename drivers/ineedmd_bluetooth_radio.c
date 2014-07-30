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
#define INEEDMD_PIO_MASK            01

#define INEEDMD_PLATFORM_RADIO_TYPE  INEEDMD_BT_RADIO_PLATFORM
  #define INEEDMD_BT_RADIO_PLATFORM  0x01

#define INEEDMD_CMND_SOF    0x9C
#define INEEDMD_CMND_EOF    0xC9

#define RESET             "\r\nRESET\r\n"
#define SDP               "\r\nSDP %s %s %s\r\nSDP\r\n"
  #define SDP_REMOTE_DEV_ADDR    cRemore_dev_addr_string
  #define SDP_SERVICE_NAME       "< I SERVICENAME S “%s” >"
    #define SDP_SERVICE_NAME_BLUETOOTH_SPP  "Bluetooth SPP"
    #define SDP_SERVICE_NAME_TBD            "?????????????"
      #define SDP_SERVICE_NAME_ID    SDP_SERVICE_NAME_BLUETOOTH_SPP
  #define  SDP_PROTOCOL_DESCRIPTOR_LIST  "< I PROTOCOLDESCRIPTORLIST < < U L2CAP> < U RFCOMM I %s > > >"
    #define  SDP_PROTOCOL_DESCRIPTOR_RFCOMM_CHANNEL_01    "01"
      #define  SDP_PROTOCOL_DESCRIPTOR_RFCOMM_CHANNEL  SDP_PROTOCOL_DESCRIPTOR_RFCOMM_CHANNEL_01
#define SET_SET           "\r\nSET\r\n"  //retruns the settings of the BT radio *warning: large return string*
  #define ENDOF_SET_SETTINGS         "SET\r\n"
  #define ENDOF_SET_SETTINGS_STRLEN  5

//#define SET_BT_NAME       "\r\nSET BT NAME IneedMD-00:07:80:0D:73:E0\r\n"
#define SET_BT_AUTH       "\r\nSET BT AUTH %c %d\r\n"
  #define SET_BT_AUTH_MODE_DISP     '*'
  #define SET_BT_AUTH_MODE_NO_DISP  '-'
    #define SET_BT_AUTH_MODE        SET_BT_AUTH_MODE_DISP
  #define SET_BT_AUTH_PIN_CODE      2124 //TODO: read out of flash maybe?

#define SET_BT_WHAT_NAME  "\r\nSET BT NAME IneedMD-NUTS\r\n"
#define SET_BT_NAME       "\r\nSET BT NAME IneedMD-%x%x\r\n"
#define SET_BT_BDADDR        "SET BT BDADDR\r\n"  //gets the BT address, yes gets it even though it says SET
#define SET_BT_BDADDR_PARSE  "%s %s %s %x %c %x %c %x %c %x %c %x %c %x"
#define SET_BT_SSP      "\r\nSET BT SSP %d %d\r\n"
  #define SET_BT_SSP_CPBLTES_DISP_ONLY    0  //Display only
  #define SET_BT_SSP_CPBLTES_DISP_AND_YN  1  //Display + yes/no button
  #define SET_BT_SSP_CPBLTES_KEYBRD_ONLY  2  //Keyboard only
  #define SET_BT_SSP_CPBLTES_NONE         3  //None
    #define SET_BT_SSP_CPLTES             SET_BT_SSP_CPBLTES_NONE
  #define SET_BT_SSP_MITM_NO_PROT    0  //no man in the middle protection
  #define SET_BT_SSP_MITM_PROT       1  //man in the middle protection required
    #define SET_BT_SSP_MITM          SET_BT_SSP_MITM_PROT
#define SET_PROFILE_SPP   "\r\nSET PROFILE SPP %s\r\n"
  #define SET_PROFILE_SPP_ON   "ON"
  #define SET_PROFILE_SPP_OFF  "OFF"
    #define SET_PROFILE_SPP_PARAM  SET_PROFILE_SPP_ON
#define SET_CONTROL_BATT  "\r\nSET CONTROL BATTERY %d %d %d %.2d\r\n"
  #define SET_CONTROL_BATT_LOW     INEEDMD_LOW_BATT_VOLTAGE
  #define SET_CONTROL_BATT_SHTDWN  INEEDMD_SHTDWN_BATT_VOLTAGE
  #define SET_CONTROL_BATT_FULL    INEEDMD_FULL_BATT_VOLTAGE
  #define SET_CONTROL_BATT_MASK    INEEDMD_PIO_MASK
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
#define SET_CONTROL_MUX  "\r\nSET CONTROL MUX %d\r\n"
  #define SET_CONTROL_MUX_MODE_DISABLE  0
  #define SET_CONTROL_MUX_MODE_ENABLE   1
    #define SET_CONTROL_MUX_MODE  SET_CONTROL_MUX_MODE_ENABLE
#define SET_CONTROL_MUX_HEX_DISABLE
#define SSP_CONFIRM  "\r\nSSP CONFIRM %x:%x:%x:%x:%x:%x OK\r\n"
#define SSP_PASSKEY  "\r\nSSP PASSKEY %x:%x:%x:%x:%x:%x OK\r\n"
#define SSP_PASSKEY_PARSE  "%s %s %x %c %x %c %x %c %x %c %x %c %x %d %c"
#define SSP_PASSKEY_PARSE_NUM_ELEMENTS  15
#define SET_RESET         "\r\nSET RESET\r\n"  //sets and returns the factory settings of the module.
#define READY             "READY.\r\n"
#define READY_STRLEN      8
#define RING              "\r\nRING 0 %x:%x:%x:%x:%x:%x 1 RFCOMM\r\n"

#define BT_MACADDR_NUM_BYTES  6
#define BG_SIZE              1024
#define BG_SEND_SIZE         256

/******************************************************************************
* variables
******************************************************************************/
volatile bool bIs_data;
volatile bool bIs_connection = false;

uint8_t   uiBT_addr[6];        //BT module mac address
char      cBT_addr_string[18]; //BT module mac address in string format
uint8_t   uiRemote_dev_addr[6];        //remote BT module mac address
char      cRemore_dev_addr_string[18]; //remote BT module mac address in string format
uint32_t  uiRemote_dev_key = 0;
uint8_t   uiSet_control_mux_hex_disable[] = {0xBF, 0xFF, 0x00, 0x11, 0x53, 0x45, 0x54, 0x20, 0x43, 0x4f, 0x4e, 0x54, 0x52, 0x4f, 0x4c, 0x20, 0x4d, 0x55, 0x58, 0x20, 0x30, 0x00};
uint8_t   uiIneedmd_radio_type = INEEDMD_PLATFORM_RADIO_TYPE;

/*
 * Function Section
 *
 */
int iIneed_md_parse_ssp (char * cBuffer, uint8_t * uiDev_addr, uint32_t * uiDev_key);
int iIneedmd_parse_addr  (char * cString_buffer , uint8_t * uiAddr);
int iIneedmd_radio_rcv_boot_msg(char *cRcv_string, uint16_t uiBuff_size);
int iIneedmd_radio_rcv_settings(char *cRcv_string, uint16_t uiBuff_size);

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
 * send a raw hex message to the radio
 * This function in it's current state just passes the parameters to the HAL.
 * It is meant to be here for future api, rtos, parsing use.
 */
int ineedmd_radio_send_frame(uint8_t *send_frame, uint16_t uiFrame_size)
{
  int i = 0;

  i = iRadio_send_frame(send_frame, uiFrame_size);
#if DEBUG
  if(i < uiFrame_size)
  {
    while(1);
  }
#endif //DEBUG

  return i;
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
 * get a byte from the radio
 */
int iIneedmd_radio_rcv_byte(uint8_t *uiRcv_byte)
{
  int i;

  i = iRadio_rcv_byte(uiRcv_byte);

  return i;
}

/*
 * get message from the radio
 */
int iIneedmd_radio_rcv_frame(uint8_t *uiRcv_frame, uint16_t uiBuff_size)
{
  int i;

  i = iRadio_rcv_string((char *)uiRcv_frame, uiBuff_size);

  return i;
}

/*
 * gets a data frame from the radio that was proc'ed by an interrupt
 */
int iIneedmd_radio_int_rcv_frame(uint8_t *uiRcv_frame, uint16_t uiBuff_size)
{
  int i;
  bool bIs_data = false;

  for(i = 0; i < uiBuff_size; i++)
  {
    iRadio_rcv_byte(&uiRcv_frame[i]);
//    MAP_UARTIntClear(INEEDMD_RADIO_UART, MAP_UARTIntStatus(INEEDMD_RADIO_UART, true));

    if(uiRcv_frame[i] == INEEDMD_CMND_EOF)
    {
      break;
    }
//    else
//    {
//      //wait for the next byte
//      while(bRadio_is_data() == false){};
//      continue;
//    }
  }

  return i;
}

/*
 * get message from the radio
 */
int iIneedmd_radio_int_rcv_string(char *cRcv_string, uint16_t uiBuff_size)
{
  int i = 0;
  bool bSetup_is_data;
  uint32_t ui32Status;
  iRadio_interface_int_enable();
  MAP_UARTIntClear(INEEDMD_RADIO_UART, MAP_UARTIntStatus(INEEDMD_RADIO_UART, true));
  bSetup_is_data = bRadio_is_data();

  while(bSetup_is_data == true)
  {
    if(i == uiBuff_size)
    {
      break;
    }

    iRadio_rcv_char(&cRcv_string[i]);

    if(i >= (READY_STRLEN - 1))
    {
      if (strcmp(&cRcv_string[i - (READY_STRLEN - 1)], READY) == 0)
      {
        break;
      }
    }
//    char *cEnd_of_frame;
//    if(cRcv_string[i] == '\n') keep this until the above READY check is solid
//    {
//      cEnd_of_frame = &cRcv_string[i - 7];
//      if((cEnd_of_frame[0] == 'R') &&
//         (cEnd_of_frame[1] == 'E') &&
//         (cEnd_of_frame[2] == 'A') &&
//         (cEnd_of_frame[3] == 'D') &&
//         (cEnd_of_frame[4] == 'Y'))
//      {
//        break;
//      }
//    }

    ++i;

    iRadio_interface_int_disable();
    iRadio_interface_int_enable();
    ui32Status = MAP_UARTIntStatus(INEEDMD_RADIO_UART, true);
    MAP_UARTIntClear(INEEDMD_RADIO_UART, ui32Status);
    bSetup_is_data = bRadio_is_data();
  }

  iRadio_interface_int_disable();
  return i;
}

/*
 * get boot message from the radio
 */
int iIneedmd_radio_rcv_boot_msg(char *cRcv_string, uint16_t uiBuff_size)
{
  int i = 0;
//  bool bSetup_is_data;
//  uint32_t ui32Status;
//  iRadio_interface_int_enable();
//  MAP_UARTIntClear(INEEDMD_RADIO_UART, MAP_UARTIntStatus(INEEDMD_RADIO_UART, true));
//  bSetup_is_data = bRadio_is_data();

//  while(bSetup_is_data == true)
  while(true)
  {
    if(i == uiBuff_size)
    {
      break;
    }

    iRadio_rcv_char(&cRcv_string[i]);

    if(i >= (READY_STRLEN - 1))
    {
      if (strcmp(&cRcv_string[i - (READY_STRLEN - 1)], READY) == 0)
      {
        break;
      }
    }
//    char *cEnd_of_frame;
//    if(cRcv_string[i] == '\n') keep this until the above READY check is solid
//    {
//      cEnd_of_frame = &cRcv_string[i - 7];
//      if((cEnd_of_frame[0] == 'R') &&
//         (cEnd_of_frame[1] == 'E') &&
//         (cEnd_of_frame[2] == 'A') &&
//         (cEnd_of_frame[3] == 'D') &&
//         (cEnd_of_frame[4] == 'Y'))
//      {
//        break;
//      }
//    }

    ++i;

//    iRadio_interface_int_disable();
//    iRadio_interface_int_enable();
//    ui32Status = MAP_UARTIntStatus(INEEDMD_RADIO_UART, true);
//    MAP_UARTIntClear(INEEDMD_RADIO_UART, ui32Status);
//    bSetup_is_data = bRadio_is_data();
  }

//  iRadio_interface_int_disable();
  return i;
}

int iIneedmd_radio_rcv_settings(char *cRcv_string, uint16_t uiBuff_size)
{
  int i = 0;

  while(true)
  {
    if(i == uiBuff_size)
    {
      break;
    }

    iRadio_rcv_char(&cRcv_string[i]);

    if(i >= (ENDOF_SET_SETTINGS_STRLEN - 1))
    {
      if (strcmp(&cRcv_string[i - (ENDOF_SET_SETTINGS_STRLEN - 1)], ENDOF_SET_SETTINGS) == 0)
      {
        break;
      }
    }

    ++i;
  }

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


/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iIneed_md_parse_ssp(char * cBuffer, uint8_t * uiDev_addr, uint32_t * uiDev_key)
{
  int iEC = 0;
  char cThrow_away[20];

  iEC = sscanf (cBuffer, SSP_PASSKEY_PARSE, cThrow_away, cThrow_away, uiDev_addr++, cThrow_away, uiDev_addr++, cThrow_away, uiDev_addr++, cThrow_away, uiDev_addr++, cThrow_away, uiDev_addr++, cThrow_away, uiDev_addr, uiDev_key);

  if(iEC == SSP_PASSKEY_PARSE_NUM_ELEMENTS)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/*
 * Check the battery voltage
 */



/*
 * Is the USB plugged in
 */

/******************************************************************************
* name:iIneedMD_radio_setup
* description: this function sets the radio interface for the corresponding
*   platform.
*   Bluetooth type platform
*   tbd type platform
* param description: none
* return value description: returns 1 if successful
******************************************************************************/
int  iIneedMD_radio_setup(void)
{
  uint32_t iEC;
//  char  *send_string = NULL;
  char cSend_buff[BG_SIZE];
  char cRcv_buff[BG_SIZE];

  if(uiIneedmd_radio_type == INEEDMD_BT_RADIO_PLATFORM)
  {
    memset(uiBT_addr,0x00, BT_MACADDR_NUM_BYTES);
    memset(uiRemote_dev_addr,0x00, BT_MACADDR_NUM_BYTES);

    //turn power on to the radio
    ineedmd_radio_power(true);

    //enable radio interface
    iRadio_interface_enable();

    //SET CONTROL MUX disable in hex format incase radrio was in mux mode
    ineedmd_radio_send_frame(uiSet_control_mux_hex_disable, 22);
    //todo: perform a syntax error check

    //SET RESET, reset to factory defaults
    ineedmd_radio_send_string(SET_RESET, strlen(SET_RESET));

    //SET, get the settings after perfroming the RFD, this is performed to alert when the RFD was completed
    ineedmd_radio_send_string(SET_SET, strlen(SET_SET));
    memset(cRcv_buff, 0x00, BG_SIZE);
    iIneedmd_radio_rcv_settings(cRcv_buff, BG_SIZE);

  //  iIneedmd_radio_rcv_boot_msg(cRcv_buff, BG_SIZE);
    //RESET, reset the radio software
    ineedmd_radio_send_string(RESET, strlen(RESET));

    //get the boot output from the radio software reset, this is performed to alert when the reset was completed
    memset(cRcv_buff, 0x00, BG_SIZE);
    iEC = iIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE);

    //hardware power reset the radio
    ineedmd_radio_reset();
    //get the boot output from radio power up
    memset(cRcv_buff, 0x00, BG_SIZE);
    iIneedmd_radio_rcv_boot_msg(cRcv_buff, BG_SIZE);

    //SET CONTROL ECHO, set the radio echo
    memset(cSend_buff, 0x00, BG_SIZE);
    snprintf(cSend_buff, BG_SIZE, SET_CONTROL_ECHO, SET_CONTROL_ECHO_SETTING);
    ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));
    memset(cRcv_buff, 0x00, BG_SIZE);
    iEC = iIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE);

    //SET BT SSP, tell the radio we are using BT SSP pairing
    memset(cSend_buff, 0x00, BG_SIZE);
    snprintf(cSend_buff, BG_SIZE, SET_BT_SSP, SET_BT_SSP_CPLTES, SET_BT_SSP_MITM);
    ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));

    //SET BT AUTH, tell the radio what auth method we are using
    memset(cSend_buff, 0x00, BG_SIZE);
    snprintf(cSend_buff, BG_SIZE, SET_BT_AUTH, SET_BT_AUTH_MODE, SET_BT_AUTH_PIN_CODE);
    ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));

    //SET PROFILE SPP, tells the radio we are using SPP protocol
    memset(cSend_buff, 0x00, BG_SIZE);
    iEC = snprintf(cSend_buff, BG_SIZE, SET_PROFILE_SPP, SET_PROFILE_SPP_PARAM);
    ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));

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
  #if DEBUG
    if(iEC != 1){ while(1){}};//todo: proper error code, no magic number
  #endif

    //format the send string with the BT name
    memset(cSend_buff, 0x00, BG_SIZE);
    snprintf(cSend_buff, BG_SIZE, SET_BT_NAME, uiBT_addr[4], uiBT_addr[5]);
    //send the new name
    ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));

    //reset the radio to make the settings take hold
    ineedmd_radio_send_string(RESET, strlen(RESET));
    iHW_delay(1000);

    ineedmd_radio_send_string(SET_SET, strlen(SET_SET));
    memset(cRcv_buff, 0x00, BG_SIZE);
    iIneedmd_radio_rcv_settings(cRcv_buff, BG_SIZE);

    //set the connection status to false while waiting for an outside connection
    bIs_connection = false;

    //enable the interrupt to the radio
    iRadio_interface_int_enable();

  }
  else
  {
#if DEBUG
    while(1){};//todo: additional platforms are not defined
#endif
  }
  return 1;
}

/*
 * Check for BT connection
 */
int  iIneedMD_radio_check_for_connection(void)
{
#define rc_chkbuff_size 128
  int iEC;
  uint8_t uiRcv_buff[rc_chkbuff_size];
  bool bIs_radio_data = false;

  //check if connection with a remote device has been established
  if(bIs_connection == false)
  {
    //begin establishing a connection with a remote device
    bIs_radio_data = bRadio_is_data();

    if(bIs_radio_data == true)
    {
      memset(uiRcv_buff, 0x00, rc_chkbuff_size);
      //receive the ssp confirm from the remote device
      bIs_radio_data = UARTCharsAvail(INEEDMD_RADIO_UART);
      iEC = iIneedmd_radio_int_rcv_frame(uiRcv_buff, rc_chkbuff_size);


      if(iEC == 1)
      {
        return 1;
      }
//      //check if the received string is a SSP request
//      iEC = iIneed_md_parse_ssp(cRcv_buff, uiRemote_dev_addr, &uiRemote_dev_key);

//      if(iEC == 1)
//      {
//        //build the SSP response frame
//        memset(cSend_buff, 0x00, rc_chkbuff_size);
//        snprintf(cSend_buff, rc_chkbuff_size, SSP_CONFIRM, uiRemote_dev_addr[0], uiRemote_dev_addr[1], uiRemote_dev_addr[2], uiRemote_dev_addr[3], uiRemote_dev_addr[4], uiRemote_dev_addr[5]);
//        //send the SSP response
//        ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));
//
//        //receive the ssp confirm from the remote device
//        iEC = iIneedmd_radio_rcv_string(cRcv_buff, rc_chkbuff_size);
//
//        //build the RING frame
//        memset(cSend_buff, 0x00, rc_chkbuff_size);
//        snprintf(cSend_buff, rc_chkbuff_size, RING, uiRemote_dev_addr[0], uiRemote_dev_addr[1], uiRemote_dev_addr[2], uiRemote_dev_addr[3], uiRemote_dev_addr[4], uiRemote_dev_addr[5]);
//        //send the RING frame
//        ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));
//
//        //activate the mux mode
////        memset(cSend_buff, 0x00, rc_chkbuff_size);
////        snprintf(cSend_buff, rc_chkbuff_size, SET_CONTROL_MUX, SET_CONTROL_MUX_MODE);
////        //send the mux frame
////        ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));
//
//        return 1;
//      }
//      else
//      {
//        return 0;
//      }
    }
    else
    {
      if(bIs_connection == true)
      {
        return 1;
      }
    }
  }

  return 0;
}

/*
 * Main radio process
 */
int iIneedMD_radio_process(void)
{
  bool bIs_connection = false;
  bool bIs_data = false;
  int iRadio_conn_status = 0;
  char cRcv_buff[128];

  iRadio_conn_status = iIneedMD_radio_check_for_connection();
  if(iRadio_conn_status == 1)
  {
    bIs_connection = true;
  }
  else
  {
    bIs_connection = false;
  }

  if(bIs_connection == true)
  {
    bIs_data = bRadio_is_data();
    if(bIs_data == true)
    {
      //receive the ssp confirm from the remote device
      iIneedmd_radio_rcv_string(cRcv_buff, 128);
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
