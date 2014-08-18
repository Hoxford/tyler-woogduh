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

#include "utils_inc/error_codes.h"

#include "board.h"
#include "ineedmd_bluetooth_radio.h"
#include "ineedmd_led.h"
#include "app_inc/ineedmd_command_protocol.h"
#include "utils_inc/proj_debug.h"

//*****************************************************************************
// defines
//*****************************************************************************
#define INEEDMD_LOW_BATT_VOLTAGE     3300
#define INEEDMD_SHTDWN_BATT_VOLTAGE  3100
#define INEEDMD_FULL_BATT_VOLTAGE    3400
#define INEEDMD_PIO_MASK             01

#define INEEDMD_PLATFORM_RADIO_TYPE  INEEDMD_BT_RADIO_PLATFORM
  #define INEEDMD_BT_RADIO_PLATFORM  0x01

#define INEEDMD_RADIO_CMND_ECHO
#ifdef INEEDMD_RADIO_CMND_ECHO
  #define vRADIO_ECHO  vDEBUG
#else
  #define vRADIO_ECHO(a)
#endif //INEEDMD_RADIO_CMND_ECHO

#define INEEDMD_CMND_SOF    0x9C
#define INEEDMD_CMND_EOF    0xC9

#define PIO               "\r\nPIO %s %x %x\r\n"
//todo: completly define these
  #define PIO_CMND_GET
  #define PIO_CMND_GETDIR
  #define PIO_CMND_GETBIAS
  #define PIO_CMND_SET
  #define PIO_CMND_SETDIR
  #define PIO_CMND_SETBIAS
  #define PIO_CMND_RESET
    #define PIO_CMND
  #define PIO_MASK_PIO0   0x0001
  #define PIO_MASK_PIO1   0x0002
  #define PIO_MASK_PIO2   0x0004
  #define PIO_MASK_PIO3   0x0008
  #define PIO_MASK_PIO4   0x0010
  #define PIO_MASK_PIO5   0x0020
  #define PIO_MASK_PIO6   0x0040
  #define PIO_MASK_PIO7   0x0080
  #define PIO_MASK_PIO8   0x0100
  #define PIO_MASK_PIO9   0x0200
  #define PIO_MASK_PIO10  0x0400
  #define PIO_MASK_PIO11  0x0800
    #define PIO_MASK_PIO
  #define PIO_STATE_PIO0
  #define PIO_STATE_PIO1
  #define PIO_STATE_PIO2
  #define PIO_STATE_PIO3
  #define PIO_STATE_PIO4
  #define PIO_STATE_PIO5
  #define PIO_STATE_PIO6
  #define PIO_STATE_PIO7
  #define PIO_STATE_PIO8
  #define PIO_STATE_PIO9
  #define PIO_STATE_PIO10
  #define PIO_STATE_PIO11
    #define PIO_STATE
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
#define SET_BT_PAIRCOUNT  "\r\nSET BT PAIRCOUNT %d\r\n"
  #define SET_BT_PAIRCOUNT_MAX_PAIRINGS  16
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
    #define SET_BT_SSP_MITM          SET_BT_SSP_MITM_NO_PROT
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
#define SET_CONTROL_ESCAPE "\r\nSET CONTROL ESCAPE %d %.4x %d\r\n"
  #define SET_CONTROL_ESCAPE_ESC_CHAR_DISABLE  '-'
  #define SET_CONTROL_ESCAPE_ESC_CHAR_DEFAULT  '+'
  #define SET_CONTROL_ESCAPE_ESC_CHAR_NEW      '?'
    #define SET_CONTROL_ESCAPE_ESC_CHAR  SET_CONTROL_ESCAPE_ESC_CHAR_DISABLE
  #define SET_CONTROL_ESCAPE_DTR_MASK_PINS  PIO_MASK_PIO0
    #define SET_CONTROL_ESCAPE_DTR_MASK  SET_CONTROL_ESCAPE_DTR_MASK_PINS
  #define SET_CONTROL_ESCAPE_DTR_MODE_DISABLED    0
  #define SET_CONTROL_ESCAPE_DTR_MODE_CMND_MODE   1
  #define SET_CONTROL_ESCAPE_DTR_MODE_CLOSE_CONN  2
  #define SET_CONTROL_ESCAPE_DTR_MODE_SOFT_RST    3
    #define SET_CONTROL_ESCAPE_DTR_MODE  SET_CONTROL_ESCAPE_DTR_MODE_CMND_MODE
#define SET_CONTROL_MUX  "\r\nSET CONTROL MUX %d\r\n"
  #define SET_CONTROL_MUX_MODE_DISABLE  0
  #define SET_CONTROL_MUX_MODE_ENABLE   1
    #define SET_CONTROL_MUX_MODE  SET_CONTROL_MUX_MODE_ENABLE
#define SET_CONTROL_MUX_HEX_DISABLE
#define SSP_CONFIRM  "\r\nSSP CONFIRM %x:%x:%x:%x:%x:%x OK\r\n"
#define SSP_PASSKEY  "\r\nSSP PASSKEY %x:%x:%x:%x:%x:%x OK\r\n"
#define SSP_PASSKEY_PARSE  "%s %s %x %c %x %c %x %c %x %c %x %c %x %d"
#define SSP_PASSKEY_PARSE_NUM_ELEMENTS  15
#define SET_RESET         "\r\nSET RESET\r\n"  //sets and returns the factory settings of the module.
#define READY             "READY.\r\n"
#define READY_STRLEN      8
#define RING              "\r\nRING 0 %x:%x:%x:%x:%x:%x 1 RFCOMM\r\n"

#define BT_MACADDR_NUM_BYTES  6
#define BG_SIZE              1024
#define BG_SEND_SIZE         128
#define BG_SEND_SIZE_SMALL   32

#define ONESEC_DELAY         100

/******************************************************************************
* variables
******************************************************************************/
volatile bool bIs_data;
volatile bool bIs_connection = false;
volatile bool bIs_frame_to_send = false;

uint8_t   uiBT_addr[6];        //BT module mac address
char      cBT_addr_string[18]; //BT module mac address in string format
uint8_t   uiRemote_dev_addr[6];        //remote BT module mac address
char      cRemore_dev_addr_string[18]; //remote BT module mac address in string format
uint8_t   uiSend_frame[BG_SEND_SIZE_SMALL];
uint16_t  uiSend_frame_len = 0;

uint32_t  uiRemote_dev_key = 0;
uint8_t   uiSet_control_mux_hex_disable[] = {0xBF, 0xFF, 0x00, 0x11, 0x53, 0x45, 0x54, 0x20, 0x43, 0x4f, 0x4e, 0x54, 0x52, 0x4f, 0x4c, 0x20, 0x4d, 0x55, 0x58, 0x20, 0x30, 0x00};
uint8_t   uiIneedmd_radio_type = INEEDMD_PLATFORM_RADIO_TYPE;

//*****************************************************************************
// external functions
//*****************************************************************************

//*****************************************************************************
// private function declarations
//*****************************************************************************
int iIneedmd_radio_cmnd_mode(bool bMode_Set);
ERROR_CODE eSend_Radio_CMND(char * cCmnd_buff, uint16_t uiCmnd_Buff_size);
int iIneed_md_parse_ssp (char * cBuffer, uint8_t * uiDev_addr, uint32_t * uiDev_key);
int iIneedmd_parse_addr  (char * cString_buffer , uint8_t * uiAddr);
ERROR_CODE iIneedmd_radio_rcv_boot_msg(char *cRcv_string, uint16_t uiBuff_size);
ERROR_CODE iIneedmd_radio_rcv_settings(char *cRcv_string, uint16_t uiBuff_size);
#if defined(DEBUG) && defined(INEEDMD_RADIO_CMND_ECHO)
    void vRADIO_ECHO_FRAME(uint8_t * uiFrame, uint16_t uiFrame_len);
#else
  vRADIO_ECHO_FRAME(a, i)
#endif //INEEDMD_RADIO_CMND_ECHO

//*****************************************************************************
// private functions
//*****************************************************************************
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iIneedmd_radio_cmnd_mode(bool bMode_Set)
{
  //todo: ABSTRACT!
  if(bMode_Set == true)
  {
    eSet_radio_to_cmnd_mode();
  }
  else if(bMode_Set == false)
  {
//    int32_t iPin_Set;
//    iPin_Set = MAP_GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3);
//    if(GPIO_PIN_3 == iPin_Set)
//    {
//      iPin_Set = MAP_GPIOPinRead(GPIO_PORTE_BASE, 0xff); //todo: define magic number to all pins or something
//      iPin_Set =iPin_Set & 0xf7;
      MAP_GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 0);
//    }

  }
  else
  {
#ifdef DEBUG
    while(1){}; //unknown mode set value
#endif
    return -1;
  }
  return 1;
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
ERROR_CODE eSend_Radio_CMND(char * cCmnd_buff, uint16_t uiCmnd_Buff_size)
{
#define SAME 0
  uint32_t i = 0;
  int iStr_cmp = 0;
  ERROR_CODE eEC = ER_FAIL;

  eEC = eIs_UART_using_DMA();
  if(eEC == ER_TRUE)
  {
    iStr_cmp = strcmp(cCmnd_buff, SET_SET);
    if(iStr_cmp == SAME)
    {
      //do special DMA setup
//      eEC = ePrep_large_DMA_rcv_buff();
    }
    eEC = eRadio_DMA_send_string(cCmnd_buff, uiCmnd_Buff_size);
  }
  else
  {
    i = iRadio_send_string(cCmnd_buff, uiCmnd_Buff_size);

    if(i < strlen(cCmnd_buff))
    {
      eEC = ER_FAIL;
#ifdef DEBUG
      while(1){};
#endif //DEBUG
    }
    else
    {
      eEC = ER_OK;
    }
  }

  //todo: if error code is not ok try sending it directly?

  if(eEC != ER_OK)
  {
    eEC = ER_TRANSMIT;
#ifdef DEBUG
    while(1){};
#endif //DEBUG
  }
  else
  {
    eEC = ER_OK;
  }

  return eEC;
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
ERROR_CODE eRcv_Radio_CMND_Resp(char * cCmnd_buff, uint16_t uiMax_buff_size)
{
  ERROR_CODE eEC = ER_FAIL;



  return eEC;
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
                                          //%s           %s           %x            %c           %x            %c           %x            %c           %x            %c           %x            %c           %x          %d
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
 * parses a received string for a BT address
 */
int iIneedmd_parse_addr(char * cString_buffer, uint8_t * uiAddr)
{
  char cThrow_away[10];
  memset(cThrow_away, 0x00, 10);
  int iError;

  //                                                     %s           %s           %s           %x        %c           %x        %c           %x        %c           %x        %c           %x        %c           %x
  iError = sscanf ( cString_buffer, SET_BT_BDADDR_PARSE, cThrow_away, cThrow_away, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr);

  iError = sscanf ( cString_buffer, "%s %s %s %x", cThrow_away, cThrow_away, cThrow_away, uiAddr++);

  //check if the raw BT address string from the modem is in the correct format
  if(iError != 14)  //todo: remove this magic number
  {
    return 0;
  }
  return 1;
}

/*
 * get boot message from the radio
 */
ERROR_CODE iIneedmd_radio_rcv_boot_msg(char *cRcv_string, uint16_t uiBuff_size)
{
  uint32_t i = 0;
  ERROR_CODE eEC = ER_OK;
  bool bWas_any_data_rcved = false;
  bool bIs_frame_complete = false;
  bool bDid_rcv_boot_msg = false;
  char * cBoot_msg = NULL;

  eEC = eUsing_radio_uart_dma();
  if(eEC == ER_TRUE)
  {
    for(i = 0; i < 5; i++)  //Check all available buffers to review buffers  //todo: magic number alert
    {
      eEC = eRcv_dma_radio_boot_frame(cRcv_string, uiBuff_size);
      if(eEC == ER_OK)
      {
          //Find the boot message
          cBoot_msg = strstr(cRcv_string, READY);
          if(cBoot_msg == NULL)
          {
            eEC = ER_INVALID_RESPONSE;
            continue;
          }
          else
          {
            eEC = ER_VALID;
            break;
          }
          //set a data received control variable to mark some data was received from radio
          bWas_any_data_rcved = true;
      }
      else if(eEC == ER_NODATA)
      {
        //check if the radio was completly silent
        if(bWas_any_data_rcved == false)
        {
          eEC = ER_FAIL;
        }
        else{/*do nothing*/}
        continue;
      }
      else
      {
        /*nothing*/
  #ifdef DEBUG
        while(1){};//catch a fail to receive
  #endif
      }
      //todo: perform a delay == time between cmnd send and cmnd resp
    }

    //check if all receive attempts were invalid
    if(eEC == ER_INVALID_RESPONSE)
    {
      vDEBUG("Invalid boot message from radio");
    }
    //check if all received frames were invalid and additional receive attempts were empty frames
    else if(eEC == ER_NODATA)
    {
      vDEBUG("Invalid frames rcvd from radio");
    }
    //check if radio was completly silent to the reset command
    else if(eEC == ER_FAIL)
    {
      if(i == 5)//todo: magic number alert
      {
        //todo, check if buffers are completly empty
        eEC = ER_TIMEOUT;
        vDEBUG("No boot msg rcvd, timeout");
      }
      else
      {
        vDEBUG("No boot msg rcvd, radio silent");
      }
    }
    //check if radio response was as expected
    else if(eEC == ER_VALID)
    {
      vDEBUG("Boot msg rcvd!");
      //todo notify HAL we are done receiving commands and to clear out buffers and control variables
      eEC = ER_OK;
    }
    else{/*do nothing*/}
#ifdef DEBUG  //catch errors
    if(eEC != ER_OK){while(1){};}
    else{/*do nothing*/}
#endif
  }
  else
  {

    while(bDid_rcv_boot_msg == false)
    {

      bIs_frame_complete = bGet_CTS_Status();
      while(bIs_frame_complete == false)
      {
        if(i == uiBuff_size)
        {
          break;
        }

        eEC = iRadio_rcv_char(&cRcv_string[i]);

        if(eEC == ER_TIMEOUT)
        {
          break;
        }

  //      if(i >= (READY_STRLEN - 1))
  //      {
  //        //check if the most recent char is an end of line
  //        if(cRcv_string[i] == '\n')
  //        {
  //          //check if the last part of the boot message is the end of the boot message
  //          if (strcmp(&cRcv_string[i - (READY_STRLEN - 1)], READY) == 0)
  //          {
  //            i = 0xFFFF;
  //            break;
  //          }
  //        }
  //      }

        ++i;
      }

      //Find the boot message
      cBoot_msg = strstr(cRcv_string, READY);
      if(cBoot_msg == NULL)
      {
        bDid_rcv_boot_msg = false;
      }
      else
      {
        bDid_rcv_boot_msg = true;
        i = 0xFFFF;
      }
    }

    if(i == 0xFFFF)
    {
      eEC = ER_OK;
    }
    else
    {
      eEC = ER_FAIL;
    }
  }

  return eEC;
}

/*
 * get the settings from the radio
 */
ERROR_CODE iIneedmd_radio_rcv_settings(char *cRcv_string, uint16_t uiBuff_size)
{
  int i = 0;
  ERROR_CODE eEC = ER_OK;
  eEC = eUsing_radio_uart_dma();
  char * cSettings_msg = NULL;
  bool bWas_any_data_rcvd = false;

  eEC = eUsing_radio_uart_dma();
  if(eEC == ER_TRUE)
  {
    for(i = 0; i < 5; i++)  //Check all available buffers to review buffers
    {
      eEC = eRcv_dma_radio_cmnd_frame(cRcv_string, uiBuff_size);
      if(eEC == ER_OK)
      {
        //determine if settings message received
        cSettings_msg = strstr(cRcv_string, ENDOF_SET_SETTINGS);
        if(cSettings_msg == NULL)
        {
          eEC = ER_INVALID_RESPONSE;
        }
        else
        {
          eEC = ER_VALID;
          break;
        }
        //set a data received control variable to mark some data was received from radio
        bWas_any_data_rcvd = true;
      }
      else if(eEC == ER_NODATA)
      {
        //check if the radio was completly silent
        if(bWas_any_data_rcvd == false)
        {
          eEC = ER_FAIL;
        }
        else{/*do nothing*/}
        continue;
      }
      else
      {
        /*nothing*/
  #ifdef DEBUG
        while(1){};//catch a fail to receive
  #endif
      }
    }
  }
  else //using blocking type receive
  {
    while(true)
    {
      if(i == uiBuff_size)
      {
        break;
      }

      eEC = iRadio_rcv_char(&cRcv_string[i]);

      if(eEC == ER_TIMEOUT)
      {
        break;
      }

      if(i >= (ENDOF_SET_SETTINGS_STRLEN - 1))
      {
        if (strcmp(&cRcv_string[i - (ENDOF_SET_SETTINGS_STRLEN - 1)], ENDOF_SET_SETTINGS) == 0)
        {
          break;
        }
      }

      ++i;
    }
  }


  return eEC;
}

ERROR_CODE eIneedmd_radio_parse_default_settings(char * cSettings_buff)
{
  ERROR_CODE eEC = ER_FAIL;

  return eEC;
}

/******************************************************************************
* name: vRADIO_ECHO_FRAME
* description: echo's the received frame from the uart out the debug port. Only
*   enabled if both debuging and radio echo are enabled.
* param description:
* return value description:
******************************************************************************/
#if defined(DEBUG) && defined(INEEDMD_RADIO_CMND_ECHO)
void vRADIO_ECHO_FRAME(uint8_t * uiFrame, uint16_t uiFrame_len)
{
  uint16_t uiIndex = 0, uiSend_index = 0;

  uint8_t uiSend_Frame[512]; //todo: magic number warning!
  char cHex_format[] = "%.2x ";
  if((uiFrame_len * 4) > 512)
  {
    return;
  }

  memset(uiSend_Frame, 0x00, 512);

  for(uiIndex = 0; uiIndex <= (uiFrame_len - 1); uiIndex++)
  {
    uiSend_index += snprintf((char *)&uiSend_Frame[uiSend_index], 512, cHex_format, uiFrame[uiIndex]);
  }

  vRADIO_ECHO((char *)uiSend_Frame);
}
#endif

//*****************************************************************************
// public functions
//*****************************************************************************

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
#ifdef DEBUG
  if(i < strlen(send_string)){while(1){};}
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
#ifdef DEBUG
  if(i < uiFrame_size){while(1){};}
#endif //DEBUG

  return i;
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
int  iIneedmd_radio_que_frame(uint8_t *send_frame, uint16_t uiFrame_size)
{
  int iEC = 0;
  uint8_t * uiFrame_cpy = NULL;

  if(uiFrame_size > BG_SEND_SIZE_SMALL)
  {
    uiSend_frame_len = 0;
    bIs_frame_to_send = false;
    iEC = -1;
  }
  else
  {
    memset(uiSend_frame, 0x00, BG_SEND_SIZE_SMALL);
    uiFrame_cpy = memcpy(uiSend_frame, send_frame, uiFrame_size);
    if(uiFrame_cpy == NULL)
    {
      uiSend_frame_len = 0;
      bIs_frame_to_send = false;
      iEC = -2;
    }
    else
    {
      uiSend_frame_len = uiFrame_size;
      bIs_frame_to_send = true;
      iEC = 0;
    }
  }

  return iEC;
}

/*
 * get message from the radio
 */
int iIneedmd_radio_rcv_string(char *cRcv_string, uint16_t uiBuff_size)
{
  int i;
  ERROR_CODE eEC = ER_FAIL;
  eEC = eUsing_radio_uart_dma();
  if(eEC == ER_TRUE)
  {
    eEC = eRcv_dma_radio_cmnd_frame(cRcv_string, uiBuff_size);
    if(eEC == ER_OK)
    {

    }
  }
  else
  {
    i = iRadio_rcv_string(cRcv_string, uiBuff_size);
  }

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
  //bool bIs_data = false;

  for(i = 0; i < uiBuff_size; i++)
  {
    iRadio_rcv_byte(&uiRcv_frame[i]);
//    MAP_UARTIntClear(INEEDMD_RADIO_UART, MAP_UARTIntStatus(INEEDMD_RADIO_UART, true));

    if(uiRcv_frame[i] == INEEDMD_CMND_EOF)
    {
      //increment i to compensate for the last byte
      i++;
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
 * get message from the radio via interrupt
 */
ERROR_CODE iIneedmd_radio_int_rcv_string(char *cRcv_string, uint16_t uiBuff_size)
{
  int i = 0;
  ERROR_CODE eEC = ER_OK;
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

    eEC = iRadio_rcv_char(&cRcv_string[i]);

    if(eEC == ER_TIMEOUT)
    {
      break;
    }

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

  return eEC;
}

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
#define DEBUG_iIneedMD_radio_setup
#ifdef DEBUG_iIneedMD_radio_setup
  #define  vDEBUG_RDIO_SETUP  vDEBUG
#else
  #define vDEBUG_RDIO_SETUP(a)
#endif
  uint32_t iEC;
  ERROR_CODE eEC = ER_OK;
  char cSend_buff[BG_SIZE];
  char cRcv_buff[BG_SIZE];
  uint32_t ui32SysClock = MAP_SysCtlClockGet();

  if(uiIneedmd_radio_type == INEEDMD_BT_RADIO_PLATFORM)
  {
    memset(uiBT_addr,0x00, BT_MACADDR_NUM_BYTES);
    memset(uiRemote_dev_addr,0x00, BT_MACADDR_NUM_BYTES);
    //turn power on to the radio
    ineedmd_radio_power(true);

    //enable radio interface
    iRadio_interface_enable();

    //todo: perform a DTR pin set and check if radio responsive to commands

    //SET CONTROL MUX disable in hex format incase radio was in mux mode
//    ineedmd_radio_send_frame(uiSet_control_mux_hex_disable, 22);
    //todo: perform a syntax error check
    //set the radio in command mode
    iIneedmd_radio_cmnd_mode(true);
//    MAP_SysCtlDelay(ui32SysClock);
//    iRadio_send_char(&cEsc_Char);
//    iRadio_send_char(&cEsc_Char);
//    iRadio_send_char(&cEsc_Char);
//    MAP_SysCtlDelay(ui32SysClock);

//    ineedmd_radio_send_string("\r\nLIST\r\n", strlen("\r\nLIST\r\n"));
//    ineedmd_radio_send_string("\r\nCLOSE 0\r\n", strlen("\r\nCLOSE 0\r\n"));
//    memset(cRcv_buff, 0x00, BG_SIZE);
//    iEC = iIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE);

    //SET RESET, reset to factory defaults
    eEC = eSend_Radio_CMND(SET_RESET, strlen(SET_RESET));
    if(eEC == ER_OK)
    {
      vDEBUG_RDIO_SETUP("SET RESET, RFD");
    }else{/*do nothing*/}
    memset(cRcv_buff, 0x00, BG_SIZE);
    eEC = iIneedmd_radio_rcv_boot_msg(cRcv_buff, BG_SIZE);
    if(eEC == ER_OK)
    {
      vRADIO_ECHO(cRcv_buff);
    }else{/*do nothing*/}

    //SET, get the settings after perfroming the RFD, this is performed to alert when the RFD was completed
//todo: received settings > 1024. DMA Buffers >1024 causing problems
//    eEC = eSend_Radio_CMND(SET_SET, strlen(SET_SET));
//    if(eEC == ER_OK)
//    {
//      vDEBUG_RDIO_SETUP("SET, get settings");
//    }
//    else{/*do nothing*/}
////    ineedmd_radio_send_string(SET_SET, strlen(SET_SET));
//    memset(cRcv_buff, 0x00, BG_SIZE);
//    eEC = iIneedmd_radio_rcv_settings(cRcv_buff, BG_SIZE);
//    if(eEC == ER_OK)
//    {
//      vRADIO_ECHO(cRcv_buff);
//    }
//    else{/*do nothing*/}

    //parse the received settings and verify device is in RFD
//todo: parse settings needs to be implemented for RFD and post set
//    eEC = eIneedmd_radio_parse_default_settings(cRcv_buff);

    //RESET, reset the radio software
////todo: a reset seems to need a delay as well like the RFD
//    eEC = eSend_Radio_CMND(RESET, strlen(RESET));
//    if(eEC == ER_OK)
//    {
//      vDEBUG_RDIO_SETUP("RESET, perform software reset");
//    }
//    //get the boot output from the radio software reset, this is performed to alert when the reset was completed
//    memset(cRcv_buff, 0x00, BG_SIZE);
//    iEC = iIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE);
//    vRADIO_ECHO(cRcv_buff);

    //todo: is this power cycle even necessary?
//    //hardware power reset the radio
//    vDEBUG_RDIO_SETUP("Power cycle");
//    ineedmd_radio_reset();
//    //get the boot output from radio power up
//    memset(cRcv_buff, 0x00, BG_SIZE);
//    iIneedmd_radio_rcv_boot_msg(cRcv_buff, BG_SIZE);
//    vRADIO_ECHO(cRcv_buff);

    //SET CONTROL ECHO, set the radio echo
    memset(cSend_buff, 0x00, BG_SIZE);
    snprintf(cSend_buff, BG_SIZE, SET_CONTROL_ECHO, SET_CONTROL_ECHO_SETTING);
//    ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));
    eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
    if(eEC == ER_OK)
    {
      vDEBUG_RDIO_SETUP("SET CONTROL ECHO, set the echo");
    }
    memset(cRcv_buff, 0x00, BG_SIZE);
    iEC = iIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE);
    vRADIO_ECHO(cRcv_buff);

    //SET BT SSP, tell the radio we are using BT SSP pairing
    memset(cSend_buff, 0x00, BG_SIZE);
    snprintf(cSend_buff, BG_SIZE, SET_BT_SSP, SET_BT_SSP_CPLTES, SET_BT_SSP_MITM);
    eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
    if(eEC == ER_OK)
    {
      vDEBUG_RDIO_SETUP("SET BT SSP, set the SSP pairing");
    }
    memset(cRcv_buff, 0x00, BG_SIZE);
    iEC = iIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE);
    vRADIO_ECHO(cRcv_buff);

    //SET BT AUTH, tell the radio what auth method we are using
    vDEBUG_RDIO_SETUP("SET BT AUTH, set the BT auth method");
    memset(cSend_buff, 0x00, BG_SIZE);
    snprintf(cSend_buff, BG_SIZE, SET_BT_AUTH, SET_BT_AUTH_MODE, SET_BT_AUTH_PIN_CODE);
    ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));

    //SET PROFILE SPP, tells the radio we are using SPP protocol
    vDEBUG_RDIO_SETUP("SET PROFILE SPP, set the SPP protocol");
    memset(cSend_buff, 0x00, BG_SIZE);
    iEC = snprintf(cSend_buff, BG_SIZE, SET_PROFILE_SPP, SET_PROFILE_SPP_PARAM);
    ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));

    // sets the battery mode for the radio,  configures the - low bat warning voltage - the low voltage lock out - the charge release voltage - that this signal is radio GPIO 01
    memset(cSend_buff, 0x00, BG_SIZE);
    iEC = snprintf(cSend_buff, BG_SIZE, SET_CONTROL_BATT, SET_CONTROL_BATT_LOW, SET_CONTROL_BATT_SHTDWN, SET_CONTROL_BATT_FULL, SET_CONTROL_BATT_MASK);
    ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));
    vDEBUG_RDIO_SETUP("SET_CONTROL_BATT");

    //get BT address
    vDEBUG_RDIO_SETUP("SET BT BDADDR, get the BT mac addr");
    ineedmd_radio_send_string(SET_BT_BDADDR, strlen(SET_BT_BDADDR));
    memset(cRcv_buff, 0x00, BG_SIZE);
    iEC = iIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE);
    vRADIO_ECHO(cRcv_buff);

    //parse string for the BT address
    iEC = iIneedmd_parse_addr(cRcv_buff, uiBT_addr);
  #ifdef DEBUG
    if(iEC != 1){ while(1){}};//todo: proper error code, no magic number
  #endif

    //format the send string with the BT name
    vDEBUG_RDIO_SETUP("SET BT NAME, set the new bt name");
    memset(cSend_buff, 0x00, BG_SIZE);
    snprintf(cSend_buff, BG_SIZE, SET_BT_NAME, uiBT_addr[4], uiBT_addr[5]);
    //send the new name
    ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));

    //Set the max number of pairings
    vDEBUG_RDIO_SETUP("SET BT PAIRCOUNT, set max number of pairings");
    memset(cSend_buff, 0x00, BG_SIZE);
    snprintf(cSend_buff, BG_SIZE, SET_BT_PAIRCOUNT, SET_BT_PAIRCOUNT_MAX_PAIRINGS);
    ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));

    //set the escape mode
    vDEBUG_RDIO_SETUP("SET CONTROL ESCAPE, set the escape mode");
    memset(cSend_buff, 0x00, BG_SIZE);
    snprintf(cSend_buff, BG_SIZE, SET_CONTROL_ESCAPE, SET_CONTROL_ESCAPE_ESC_CHAR, SET_CONTROL_ESCAPE_DTR_MASK, SET_CONTROL_ESCAPE_DTR_MODE);
    ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));
//
//    memset(cRcv_buff, 0x00, BG_SIZE);
//    iEC = iIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE);
//    vRADIO_ECHO(cRcv_buff);

    //reset the radio to make the settings take hold
    vDEBUG_RDIO_SETUP("RESET, perform software reset");
    ineedmd_radio_send_string(RESET, strlen(RESET));
//    iHW_delay(ONESEC_DELAY);
    MAP_SysCtlDelay(ui32SysClock);

//    vDEBUG_RDIO_SETUP("SET, get settings");
//    ineedmd_radio_send_string(SET_SET, strlen(SET_SET));
//    memset(cRcv_buff, 0x00, BG_SIZE);
//    iIneedmd_radio_rcv_settings(cRcv_buff, BG_SIZE);
//    vRADIO_ECHO(cRcv_buff);
    //todo: verify settings properly set

    //set the connection status to false while waiting for an outside connection
    bIs_connection = false;
    iIneedmd_radio_cmnd_mode(false);

    //enable the interrupt to the radio
    iRadio_interface_int_enable();
//    iHW_delay(ONESEC_DELAY);
    MAP_SysCtlDelay(ui32SysClock);
    //todo: check the ready pin
    vDEBUG_RDIO_SETUP("Radio ready");
  }
  else
  {
#ifdef DEBUG
    vDEBUG_RDIO_SETUP("additional platforms are not defined");
    while(1){};
#endif
  }
  return 1;
#undef vDEBUG_RDIO_SETUP
}

/*
 * Check for BT connection
 */
int  iIneedMD_radio_check_for_connection(void)
{
#define DEBUG_radio_check_for_connection
#ifdef DEBUG_radio_check_for_connection
  #define  vDEBUG_RDIO_CHKCONN  vDEBUG
#else
  #define vDEBUG_RDIO_CHKCONN(a)
#endif
#define rc_chkbuff_size 128
  int iEC = 0;
  uint8_t uiRcv_buff[rc_chkbuff_size];
  bool bIs_radio_data = false;

  //check if connection with a remote device has been established
  if(bIs_connection == false)
  {
//    ineedmd_led_pattern(BT_ATTEMPTING);

    //begin establishing a connection with a remote device
    bIs_radio_data = bRadio_is_data();

    if(bIs_radio_data == true)
    {
      memset(uiRcv_buff, 0x00, rc_chkbuff_size);
      //receive the ssp confirm from the remote device
      bIs_radio_data = UARTCharsAvail(INEEDMD_RADIO_UART);
      iEC = iIneedmd_radio_int_rcv_frame(uiRcv_buff, rc_chkbuff_size);

      //determine origin of the data frame
      if(iEC > 0)
      {
        vRADIO_ECHO_FRAME(uiRcv_buff, iEC);

        //check if the frame is a ineedMD command protocol frame
        if(uiRcv_buff[0] == 0x9C)
        {
          iIneedmd_Rcv_cmnd_frame(uiRcv_buff, iEC);

        }
      }
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
#undef vDEBUG_RDIO_CHKCONN
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
