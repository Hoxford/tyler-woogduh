/*
 * ineedmd_bluetooth_radio.c
 *
 *  Created on: Jul 7, 2014
 *      Author: BrianS
 */

#ifndef __INEEDMD_BLUETOOTH_RADIO_C__
#define __INEEDMD_BLUETOOTH_RADIO_C__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
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
#include "drivers_inc/ineedmd_bluetooth_radio.h"
#include "app_inc/ineedmd_command_protocol.h"
#include "utils_inc/proj_debug.h"
#include "utils_inc/osal.h"

//todo: the bios includes should all eventually be moved to the osal
//#include <xdc/std.h>
//#include <xdc/runtime/System.h>
//
//#include <ti/sysbios/knl/Clock.h>


/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
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
#define INEEDMD_CMND_CONNECTION_FRAME_LEN    5

#define CR_NL             "\r\n"
#define PIO_GET           "PIO GET\r\n"
#define PIO_GETDIR        "PIO GETDIR\r\n"
#define PIO_GETBIAS       "PIO GETBIAS\r\n"
#define PIO_SET           "PIO SET %.2x %.2x\r\n"
#define PIO_SETDIR        "PIO SETDIR %.2x %.2x\r\n"
#define PIO_SETBIAS       "PIO SETBIAS %.2x %.2x\r\n"
  #define PIO_MASK_NONE   0x0000
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
  #define PIO_STATE_LOW_PIO0    0x0000
  #define PIO_STATE_HIGH_PIO0   0x0001
  #define PIO_STATE_LOW_PIO1    0x0000
  #define PIO_STATE_HIGH_PIO1   0x0002
  #define PIO_STATE_LOW_PIO2    0x0000
  #define PIO_STATE_HIGH_PIO2   0x0004
  #define PIO_STATE_LOW_PIO3    0x0000
  #define PIO_STATE_HIGH_PIO3   0x0008
  #define PIO_STATE_LOW_PIO4    0x0000
  #define PIO_STATE_HIGH_PIO4   0x0010
  #define PIO_STATE_LOW_PIO5    0x0000
  #define PIO_STATE_HIGH_PIO5   0x0020
  #define PIO_STATE_LOW_PIO6    0x0000
  #define PIO_STATE_HIGH_PIO6   0x0040
  #define PIO_STATE_LOW_PIO7    0x0000
  #define PIO_STATE_HIGH_PIO7   0x0080
  #define PIO_STATE_LOW_PIO8    0x0000
  #define PIO_STATE_HIGH_PIO8   0x0100
  #define PIO_STATE_LOW_PIO9    0x0000
  #define PIO_STATE_HIGH_PIO9   0x0200
  #define PIO_STATE_LOW_PIO10   0x0000
  #define PIO_STATE_HIGH_PIO10  0x0400
  #define PIO_STATE_LOW_PIO11   0x0000
  #define PIO_STATE_HIGH_PIO11  0x0800
    #define PIO_SET_MASK       PIO_MASK_PIO0
    #define PIO_SET_STATE      PIO_STATE_HIGH_PIO0
    #define PIO_SETDIR_MASK    PIO_MASK_PIO0
    #define PIO_SETDIR_STATE   PIO_STATE_LOW_PIO0
    #define PIO_SETBIAS_MASK   PIO_MASK_PIO0
    #define PIO_SETBIAS_STATE  PIO_STATE_HIGH_PIO0
#define PIO_RESET  "PIO RESET\r\n"
#define RESET             "RESET\r\n"
#define SDP               "\r\nSDP %s %s %s\r\nSDP\r\n"
  #define SDP_REMOTE_DEV_ADDR    cRemore_dev_addr_string
  #define SDP_SERVICE_NAME       "< I SERVICENAME S “%s” >"
    #define SDP_SERVICE_NAME_BLUETOOTH_SPP  "Bluetooth SPP"
    #define SDP_SERVICE_NAME_TBD            "?????????????"
      #define SDP_SERVICE_NAME_ID    SDP_SERVICE_NAME_BLUETOOTH_SPP
  #define  SDP_PROTOCOL_DESCRIPTOR_LIST  "< I PROTOCOLDESCRIPTORLIST < < U L2CAP> < U RFCOMM I %s > > >"
    #define  SDP_PROTOCOL_DESCRIPTOR_RFCOMM_CHANNEL_01    "01"
      #define  SDP_PROTOCOL_DESCRIPTOR_RFCOMM_CHANNEL  SDP_PROTOCOL_DESCRIPTOR_RFCOMM_CHANNEL_01
#define SET_SET           "SET\r\n"  //retruns the settings of the BT radio *warning: large return string*
  #define ENDOF_SET_SETTINGS         "SET\r\n"
  #define ENDOF_SET_SETTINGS_STRLEN  5

//#define SET_BT_NAME       "\r\nSET BT NAME IneedMD-00:07:80:0D:73:E0\r\n"
#define SET_BT_AUTH       "SET BT AUTH %c %.4d\r\n"
  #define SET_BT_AUTH_MODE_DISP     '*'
  #define SET_BT_AUTH_MODE_NO_DISP  '-'
    #define SET_BT_AUTH_MODE        SET_BT_AUTH_MODE_DISP
  #define SET_BT_AUTH_PIN_CODE      2124 //TODO: read out of flash maybe?

#define SET_BT_WHAT_NAME  "SET BT NAME IneedMD-NUTS\r\n"
#define SET_BT_NAME       "SET BT NAME IneedMD-%x%x\r\n"
#define SET_BT_PAIRCOUNT  "SET BT PAIRCOUNT %d\r\n"
  #define SET_BT_PAIRCOUNT_MAX_PAIRINGS  16
#define SET_BT_BDADDR        "SET BT BDADDR\r\n"  //gets the BT address, yes gets it even though it says SET
#define SET_BT_BDADDR_PARSE  "%s %s %s %hhx %c %hhx %c %hhx %c %hhx %c %hhx %c %hhx"
#define SET_BT_SSP      "SET BT SSP %d %d\r\n"
  #define SET_BT_SSP_CPBLTES_DISP_ONLY    0  //Display only
  #define SET_BT_SSP_CPBLTES_DISP_AND_YN  1  //Display + yes/no button
  #define SET_BT_SSP_CPBLTES_KEYBRD_ONLY  2  //Keyboard only
  #define SET_BT_SSP_CPBLTES_NONE         3  //None
    #define SET_BT_SSP_CPLTES             SET_BT_SSP_CPBLTES_NONE
  #define SET_BT_SSP_MITM_NO_PROT    0  //no man in the middle protection
  #define SET_BT_SSP_MITM_PROT       1  //man in the middle protection required
    #define SET_BT_SSP_MITM          SET_BT_SSP_MITM_NO_PROT
#define SET_PROFILE_SPP   "SET PROFILE SPP %s\r\n"
  #define SET_PROFILE_SPP_ON   "ON"
  #define SET_PROFILE_SPP_OFF  "OFF"
    #define SET_PROFILE_SPP_PARAM  SET_PROFILE_SPP_ON
#define SET_CONTROL_BATT  "SET CONTROL BATTERY %d %d %d %.2d\r\n"
  #define SET_CONTROL_BATT_LOW     INEEDMD_LOW_BATT_VOLTAGE
  #define SET_CONTROL_BATT_SHTDWN  INEEDMD_SHTDWN_BATT_VOLTAGE
  #define SET_CONTROL_BATT_FULL    INEEDMD_FULL_BATT_VOLTAGE
  #define SET_CONTROL_BATT_MASK    INEEDMD_PIO_MASK
#define SET_CONTROL_BAUD  "SET CONTROL BAUD %d,8%c%d\r\n"
  #define SET_CONTROL_BAUD_PARITY_NONE  'N'
  #define SET_CONTROL_BAUD_PARITY_EVEN  'E'
  #define SET_CONTROL_BAUD_PARITY_ODD   'O'
    #define SET_CONTROL_BAUD_PARITY  SET_CONTROL_BAUD_PARITY_NONE
  #define SET_CONTROL_BAUD_STOP_BITS_1  1
  #define SET_CONTROL_BAUD_STOP_BITS_2  2
      #define SET_CONTROL_BAUD_STOP_BITS  SET_CONTROL_BAUD_STOP_BITS_1
#define SET_CONTROL_BAUD_GET  "SET CONTROL BAUD\r\n"
#define SET_CONTROL_BIND    "SET CONTROL BIND %d %.4x %s %s\r\n"
  #define SET_CONTROL_BIND_PRIORITY0  0
  #define SET_CONTROL_BIND_PRIORITY1  1
  #define SET_CONTROL_BIND_PRIORITY2  2
  #define SET_CONTROL_BIND_PRIORITY3  3
  #define SET_CONTROL_BIND_PRIORITY4  4
  #define SET_CONTROL_BIND_PRIORITY5  5
  #define SET_CONTROL_BIND_PRIORITY6  6
  #define SET_CONTROL_BIND_PRIORITY7  7
    #define SET_CONTROL_BIND_PRIORITY  SET_CONTROL_BIND_PRIORITY0
    #define SET_CONTROL_BIND_IO_MASK   PIO_MASK_PIO0
  #define SET_CONTROL_BIND_DIRECTION_RISE    "RISE"
  #define SET_CONTROL_BIND_DIRECTION_FALL    "FALL"
  #define SET_CONTROL_BIND_DIRECTION_CHANGE  "CHANGE"
    #define SET_CONTROL_BIND_DIRECTION  SET_CONTROL_BIND_DIRECTION_RISE
    #define SET_CONTROL_COMMAND  "CLOSE 0"
#define SET_CONTROL_CONFIG_GET   "SET CONTROL CONFIG\r\n"
#define SET_CONTROL_CONFIG_PARSE "%s %s %s %hx %hx %hx %hx"
#define SET_CONTROL_CONFIG       "SET CONTROL CONFIG %.4x %.4x %.4x %.4x\r\n"
  #define SET_CONTROL_CONFIG_OPT_BITNULL  0x0000
  #define SET_CONTROL_CONFIG_OPT_BIT00    0x0001
  #define SET_CONTROL_CONFIG_OPT_BIT01    0x0002
  #define SET_CONTROL_CONFIG_OPT_BIT02    0x0004
  #define SET_CONTROL_CONFIG_OPT_BIT03    0x0008
  #define SET_CONTROL_CONFIG_OPT_BIT04    0x0010
  #define SET_CONTROL_CONFIG_OPT_BIT05    0x0020
  #define SET_CONTROL_CONFIG_OPT_BIT06    0x0040
  #define SET_CONTROL_CONFIG_OPT_BIT07    0x0080
  #define SET_CONTROL_CONFIG_OPT_BIT08    0x0100
  #define SET_CONTROL_CONFIG_OPT_BIT09    0x0200
  #define SET_CONTROL_CONFIG_OPT_BIT10    0x0400
  #define SET_CONTROL_CONFIG_OPT_BIT11    0x0800
  #define SET_CONTROL_CONFIG_OPT_BIT12    0x1000
  #define SET_CONTROL_CONFIG_OPT_BIT13    0x2000
  #define SET_CONTROL_CONFIG_OPT_BIT14    0x4000
  #define SET_CONTROL_CONFIG_OPT_BIT15    0x8000
    #define SET_CONTROL_CONFIG_OPT_BLK_3   SET_CONTROL_CONFIG_OPT_BITNULL
    #define SET_CONTROL_CONFIG_OPT_BLK_2   SET_CONTROL_CONFIG_OPT_BITNULL
    #define SET_CONTROL_CONFIG_OPT_BLK_1   SET_CONTROL_CONFIG_OPT_BITNULL
    #define SET_CONTROL_CONFIG_CFG_BLK     SET_CONTROL_CONFIG_OPT_BIT05

#define SET_CONTROL_CONFIG_LIST  "SET CONTROL CONFIG LIST\r\n"
#define SET_CONTROL_ECHO  "SET CONTROL ECHO %d\r\n"
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
                                      | SET_CONTROL_ECHO_SET_EVENT_ON  \
                                      | SET_CONTROL_ECHO_SYNTAX_ERR_ON)
#define SET_CONTROL_ESCAPE "SET CONTROL ESCAPE %c %.2x %d\r\n"
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
#define SET_CONTROL_VREGEN  "SET CONTROL VREGEN %d %x\r\n"
  #define SET_CONTROL_VREGEN_MODE_0  0
  #define SET_CONTROL_VREGEN_MODE_1  1
  #define SET_CONTROL_VREGEN_MODE_2  2
    #define SET_CONTROL_VREGE_MODE SET_CONTROL_VREGEN_MODE_1
  #define SET_CONTROL_VREGEN_PIOMASK  PIO_MASK_NONE
#define SSP_CONFIRM  "\r\nSSP CONFIRM %x:%x:%x:%x:%x:%x OK\r\n"
#define SSP_PASSKEY  "\r\nSSP PASSKEY %x:%x:%x:%x:%x:%x OK\r\n"
#define SSP_PASSKEY_PARSE  "%s %s %hhx %c %hhx %c %hhx %c %hhx %c %hhx %c %hhx %d"
#define SSP_PASSKEY_PARSE_NUM_ELEMENTS  15
#define SET_RESET         "SET RESET\r\n"  //sets and returns the factory settings of the module.
#define READY             "READY.\r\n"
#define READY_STRLEN      8
#define RING              "RING "
#define RING_RFCOMM       "RFCOMM \r\n"
#define RING_PARSE        "%s %n %x%c%x%c%x%c%x%c%x%c%x %n %s"
#define RING_CONNECTION_BUFF_SIZE    36
#define RING_CONNECTION_STRING_LEN   36

#define BT_MACADDR_NUM_BYTES  6
#define BG_SIZE              1024
#define BG_SEND_SIZE         128
#define BG_SEND_SIZE_SMALL   32

//Delay times
#define TIMEOUT_RFD_CTS_HL         2000
#define TIMEOUT_CONN_WAIT_FOREVER  0

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//radio setup and init control variables
bool bIs_Radio_setup = false;

//connection and data control variables
volatile bool bIs_data;
volatile bool bIs_connection = false;
volatile bool bIs_frame_to_send = false;

//callback funtion control variables
bool bDid_rcv_read_callback = false;
int  iRcv_read_count = 0;
bool bDid_rcv_write_callback = false;

//radio address and settings
uint8_t   uiBT_addr[6];                //BT module mac address
char      cBT_addr_string[18];         //BT module mac address in string format
uint8_t   uiRemote_dev_addr[6];        //remote BT module mac address
char      cRemore_dev_addr_string[18]; //remote BT module mac address in string format
uint16_t  uiSet_ctrl_config[4];        //The Radio's feature configuration

uint8_t   uiSend_frame[BG_SEND_SIZE_SMALL];
uint16_t  uiSend_frame_len = 0;

char      cRdo_setup_rcv_buff[BG_SIZE];

uint32_t  uiRemote_dev_key = 0;
uint8_t   uiSet_control_mux_hex_disable[] = {0xBF, 0xFF, 0x00, 0x11, 0x53, 0x45, 0x54, 0x20, 0x43, 0x4f, 0x4e, 0x54, 0x52, 0x4f, 0x4c, 0x20, 0x4d, 0x55, 0x58, 0x20, 0x30, 0x00};
uint8_t   uiIneedmd_radio_type = INEEDMD_PLATFORM_RADIO_TYPE;

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
extern Mailbox_Handle tTIRTOS_Radio_mailbox;
/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//eRadio_event radio event tracking
typedef enum eRadio_event
{
    RDIO_NO_EVENT,
    RDIO_EVENT_BATT_LOW,
    RDIO_EVENT_BATT_SHUTDOWN,
    RDIO_EVENT_TBD
}eRadio_event;

eRadio_event       eActive_radio_events       = RDIO_NO_EVENT; //eActive_radio_events, used to keep track of current radio events

eRadio_setup_state eCurrent_radio_setup_state = RDIO_SETUP_NOT_STARTED;/*the current setup state of the radio, it should only be directly written to
                                                                        *via the radio setup funcion. all other reads and writes should be done from
                                                                        *the radio setup state read and write functions.*/

eRadio_connection_state eCurrent_radio_connection_state = RADIO_CONN_UNKNOWN;

typedef enum eRadio_message
{
  RADIO_MSG_UNKNOWN,
  RADIO_MSG_SEND,
  RADIO_MSG_RCV,
  RADIO_MSG_RADIO_RFD,
  RADIO_MSG_RADIO_SETUP,
  RADIO_MSG_RADIO_PWR_DOWN,
  RADIO_MSG_RADIO_PWR_UP,
  RADIO_MSG_WAIT_FOR_CONNECTION
}eRadio_message;

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
typedef struct tRadio_message_package
{
  eRadio_message eMessage;
  tRadio_request tRequest;
}tRadio_message_package;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eIneedmd_radio_cmnd_mode    (bool bMode_Set);
ERROR_CODE eBT_RDIO_mux_mode_disable   (void); //attempts to get radio out of mux mode
ERROR_CODE eSend_Radio_CMND            (char * cCmnd_buff,     uint16_t uiCmnd_Buff_size);
int        iIneed_md_parse_ssp         (char * cBuffer,        uint8_t * uiDev_addr, uint32_t * uiDev_key);
int        iIneedmd_parse_addr         (char * cString_buffer, uint8_t * uiAddr);
int        ineedmd_radio_send_frame    (uint8_t *send_frame, uint16_t uiFrame_size);
ERROR_CODE iIneedmd_radio_rcv_boot_msg (char * cRcv_string,    uint16_t uiBuff_size);
ERROR_CODE eIneedmd_radio_rcv_string(char *cRcv_string, uint16_t uiBuff_size, uint16_t * puiRcvd_str_len);
ERROR_CODE iIneedmd_radio_rcv_settings (char * cRcv_string,    uint16_t uiBuff_size);
ERROR_CODE eIneedmd_radio_rcv_config   (char * cRcv_string,    uint16_t uiBuff_size, uint16_t * uiSetcfg_Param);
ERROR_CODE eIneedmd_radio_wait_for_conn(tRadio_request * ptConn_request);
ERROR_CODE eSet_connection_state(eRadio_connection_state eConn_state);
#ifdef NOT_NOW
ERROR_CODE eIneedmd_radio_parse_default_settings (char * cSettings_buff);
ERROR_CODE eIneedmd_radio_chk_event    (char * cEvent_buff);
#if defined(DEBUG) && defined(INEEDMD_RADIO_CMND_ECHO)
    void vRADIO_ECHO_FRAME(uint8_t * uiFrame, uint16_t uiFrame_len);
#else
  #define vRADIO_ECHO_FRAME(a, i)
#endif //INEEDMD_RADIO_CMND_ECHO
#endif


/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
/******************************************************************************
* name: eIneedmd_radio_cmnd_mode
* description: Sets the radio to receive proprietary configuration commands and
*  to no longer treate received data frames from the radio as data.
* param description: bool - true: sets the radio to command mode
*                    bool - false: sets the radio to data mode
* return value description: ERROR_CODE - ER_OK: radio was successfully set to the requested mode
*                           ERROR_CODE - ER_FAIL
******************************************************************************/
ERROR_CODE eIneedmd_radio_cmnd_mode(bool bMode_Set)
{
//#define DEBUG_iIneedmd_radio_cmnd_mode
#ifdef DEBUG_iIneedmd_radio_cmnd_mode
  #define  vDEBUG_RDIO_CMND_MODE  vDEBUG
#else
  #define vDEBUG_RDIO_CMND_MODE(a)
#endif
  ERROR_CODE eEC = ER_FAIL;

  if(bMode_Set == true)
  {
    eEC = eSet_radio_to_cmnd_mode();
  }
  else if(bMode_Set == false)
  {
    eEC = eSet_radio_to_data_mode();
  }
  else
  {
#ifdef DEBUG
    vDEBUG_RDIO_CMND_MODE("Cmnd mode SYS HALTED, unknown mode set value")
    eEC = ER_FAIL;
    while(1){}; //unknown mode set value
#elif
    eEC = ER_FAIL;
#endif
  }
  return eEC;
#undef  vDEBUG_RDIO_CMND_MODE
}

ERROR_CODE eBT_RDIO_mux_mode_disable(void)
{
  ERROR_CODE eEC = ER_OK;
  char cEsc_char = '+';
  uint16_t i = 0;
  char cMux_rcv_buff[READY_STRLEN];
  memset(cMux_rcv_buff,0x00, READY_STRLEN);

  eBSP_Set_radio_uart_baud(INEEDMD_RADIO_UART_DEFAULT_BAUD);

  //SET CONTROL MUX disable in hex format incase radio was in mux mode
  ineedmd_radio_send_frame(uiSet_control_mux_hex_disable, 23);

  while(true)
  {
    if(i == READY_STRLEN)
    {
      eEC = ER_BUFF_SIZE;
      break;
    }else{/*do nothing*/}

    eEC = iRadio_rcv_char(&cMux_rcv_buff[i]);

    if(eEC == ER_TIMEOUT)
    {
      break;
    }else{/*do nothing*/}

    if(i >= (READY_STRLEN - 1))
    {
      //check if the most recent char is an end of line
      if(cMux_rcv_buff[i] == '\n')
      {
        //check if the last part of the boot message is the end of the boot message
        if (strcmp(&cMux_rcv_buff[i - (READY_STRLEN - 1)], READY) == 0)
        {
          eEC = ER_VALID;
          break;
        }
        else
        {
          eEC = ER_INVALID;
        }
      }else{/*do nothing*/}
    }else{/*do nothing*/}

    ++i;
  }
  eRadio_clear_rcv_buffer();
  memset(cMux_rcv_buff,0x00, READY_STRLEN);
  i = 0;

  if(eEC != ER_VALID)
  {
    //todo magic numbers!
    iHW_delay(1000);
    iRadio_send_char(&cEsc_char);
    iRadio_send_char(&cEsc_char);
    iRadio_send_char(&cEsc_char);
    iHW_delay(1000);

    while(true)
    {
      if(i == READY_STRLEN)
      {
        eEC = ER_BUFF_SIZE;
        break;
      }else{/*do nothing*/}

      eEC = iRadio_rcv_char(&cMux_rcv_buff[i]);

      if(eEC == ER_TIMEOUT)
      {
        break;
      }else{/*do nothing*/}

      if(i >= (READY_STRLEN - 1))
      {
        //check if the most recent char is an end of line
        if(cMux_rcv_buff[i] == '\n')
        {
          //check if the last part of the boot message is the end of the boot message
          if (strcmp(&cMux_rcv_buff[i - (READY_STRLEN - 1)], READY) == 0)
          {
            eEC = ER_VALID;
            break;
          }
          else
          {
            eEC = ER_INVALID;
          }
        }else{/*do nothing*/}
      }else{/*do nothing*/}

      ++i;
    }
  }
  eRadio_clear_rcv_buffer();

  return eEC;
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
ERROR_CODE eSend_Radio_CMND(char * cCmnd_buff, uint16_t uiCmnd_Buff_size)
{
#define DEBUG_eSend_Radio_CMND
#ifdef DEBUG_eSend_Radio_CMND
  #define  vDEBUG_RDIO_SND_CMND  vDEBUG
#else
  #define vDEBUG_RDIO_SND_CMND(a)
#endif
#define SAME 0
  uint32_t i = 0;
  int iStr_cmp = 0;
  ERROR_CODE eEC = ER_FAIL;

  eEC = eUsing_radio_uart_dma();
  if(eEC == ER_TRUE)
  {
//    check if command to send needs special handling
//    iStr_cmp = strcmp(cCmnd_buff, SET_SET);
//    if(iStr_cmp == SAME)
//    {
//      vDEBUG_RDIO_SND_CMND("Send cmnd, SET command special handling");
//      //do special DMA setup
////      eEC = ePrep_large_DMA_rcv_buff();
//    }
//    eEC = eRadio_DMA_send_string(cCmnd_buff, uiCmnd_Buff_size);
//
//    //todo: if error code is not ok try sending it directly?
  }
  else
  {
    i = iRadio_send_string(cCmnd_buff, uiCmnd_Buff_size);

    if(i < strlen(cCmnd_buff))
    {
      eEC = ER_FAIL;
#ifdef DEBUG
      vDEBUG_RDIO_SND_CMND("Send cmnd SYS HALTED, send failed");
      while(1){};
#endif //DEBUG
    }
    else
    {
      eEC = ER_OK;
    }
  }

  if(eEC != ER_OK)
  {
    eEC = ER_TRANSMIT;
#ifdef DEBUG
    vDEBUG_RDIO_SND_CMND("Send cmnd SYS HALTED, Send command failed");
    while(1){};
#endif //DEBUG
  }
  else
  {
    eEC = ER_OK;
  }

  return eEC;
#undef  vDEBUG_RDIO_SND_CMND
}
#ifdef NOT_NOW
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
#endif
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

  //check if the raw BT address string from the modem is in the correct format
  if(iError != 14)  //todo: remove this magic number
  {
    return 0;
  }
  return 1;
}

/*
 * send a raw hex message to the radio
 * This function in it's current state just passes the parameters to the HAL.
 * It is meant to be here for future api, rtos, parsing use.
 */
int ineedmd_radio_send_frame(uint8_t *send_frame, uint16_t uiFrame_size)
{
//#define DEBUG_ineedmd_radio_send_frame
#ifdef DEBUG_ineedmd_radio_send_frame
  #define  vDEBUG_RDIO_SND_FR  vDEBUG
#else
  #define vDEBUG_RDIO_SND_FR(a)
#endif
  int i = 0;

  i = iRadio_send_frame(send_frame, uiFrame_size);
#ifdef DEBUG
  if(i < uiFrame_size)
  {
    vDEBUG_RDIO_SND_FR("Rdio send fr SYS HALT, send frame failed");
    while(1){};
  }else{/*do nothing*/}
#endif //DEBUG

  return i;
#undef vDEBUG_RDIO_SND_FR
}

/******************************************************************************
* name: iIneedmd_radio_rcv_boot_msg
* description: gets boot message from the radio. This funcion should only be
*  used when the radio is set up to post a boot message on either power cycle,
*  soft reset and or factory reset. If the radio is not set to post the boot
*  message on any of the previous defined events this function will fail.
* param description: char * - pointer to a previously allocated buffer to store
*  the boot message.
*                    uint16_t - unsigned int type variable that states the max
*  size of the allocated buffer to fill in bytes.
* return value description: ERROR_CODE - ER_VALID: boot message received
*                           ERROR_CODE - ER_INVALID: boot message was never received
*                           ERROR_CODE - ER_BUFF_SIZE: the max size of the receive buffer was reached
*                           ERROR_CODE - ER_TIMEOUT: the receive interface timed out
******************************************************************************/
ERROR_CODE iIneedmd_radio_rcv_boot_msg(char *cRcv_string, uint16_t uiBuff_size)
{
//#define DEBUG_iIneedmd_radio_rcv_boot_msg
#ifdef DEBUG_iIneedmd_radio_rcv_boot_msg
  #define  vDEBUG_RDIO_RCV_BOOTMSG  vDEBUG
#else
  #define vDEBUG_RDIO_RCV_BOOTMSG(a)
#endif
  uint32_t i = 0;
  ERROR_CODE eEC = ER_FAIL;
  uint32_t uiClock_time_start = 0;
  uint32_t uiClock_time_current = 0;
  uint32_t uiClock_time_waiting_for_boot = 0;
  char * cBoot_string = NULL;
  uint16_t uiBoot_index = 0;
  typedef enum eBoot_Phase
  {
    BOOT_PHASE_START,
    BOOT_PHASE_BOOT_POST,
    BOOT_PHASE_MFG,
    BOOT_PHASE_READY
  }eBoot_Phase;
  eBoot_Phase eBootState = BOOT_PHASE_START;

  uiClock_time_start = Clock_getTicks();
  eEC = eUsing_radio_uart_dma();
  if(eEC == ER_TRUE)
  {
//    //dma is to fast and causes problems when the device re-boots. A hard delay of 1 second is used to properly compensate for this
////    MAP_SysCtlDelay(ui32SysClock);
//    for(i = 0; i < 5; i++)  //Check all available buffers to review buffers  //todo: magic number alert
//    {
//      eEC = eRcv_dma_radio_boot_frame(cRcv_string, uiBuff_size);
//      if(eEC == ER_OK)
//      {
//        //set a data received control variable to mark some data was received from radio
//        uiRcv_len = strlen(cRcv_string);
//        if(uiRcv_len > 0)
//        {
//          bWas_any_data_rcved = true;
//        }else{/*do nothing*/}
//
//        //Check if the boot msg is in the received buff
//        cBoot_msg = strstr(cRcv_string, READY);
//        if(cBoot_msg == NULL)
//        {
//          //This evaluated buffer did not contain the boot message
//          eEC = ER_INVALID_RESPONSE;
//          continue;
//        }
//        else
//        {
//          //boot message is valid
//          eEC = ER_VALID;
//          break;
//        }
//      }
//      else if(eEC == ER_NODATA)
//      {
//        //check if the radio was completly silent
//        if(bWas_any_data_rcved == false)
//        {
//          eEC = ER_FAIL;
//        }else{/*nothing*/}
//        continue;
//      }
//      else
//      {
//        /*nothing*/
//  #ifdef DEBUG
//        //todo: make sure eRcv_dma_radio_cmnd_frame only returns the checked error codes
//        vDEBUG_RDIO_RCV_BOOTMSG("Rcv bootmsg SYS HALTED, unhandled eRcv_dma_radio_cmnd_frame resp");
//        while(1){};//catch a fail to receive
//  #endif
//      }
//    }
//
//    //check the error codes and respond accordingly
//    if(eEC == ER_INVALID_RESPONSE)
//    {
//      //All received frames from the radio were invalid and not what was expected
//      vDEBUG_RDIO_RCV_BOOTMSG("Rcv bootmsg invalid frames");
//    }
//    else if(eEC == ER_NODATA)
//    {
//      //all receive attempts returned no data in the frames
//      vDEBUG_RDIO_RCV_BOOTMSG("Rcv bootmsg, radio silent");
//    }
//    //check if radio was completly silent to the reset command
//    else if(eEC == ER_FAIL)
//    {
//      if(i == 5)//todo: magic number alert
//      {
//        //todo, check if buffers are completly empty
//        //after multiple attemptps the radio returned either invalid frames or was silent
//        if(bWas_any_data_rcved == true)
//        {
//          vDEBUG_RDIO_RCV_BOOTMSG("Rcv bootmsg invalid frames");
//        }
//        else
//        {
//          vDEBUG_RDIO_RCV_BOOTMSG("Rcv bootmsg, radio silent");
//        }
//        //set the error code to timeout since no valid frames were received during the number of attempts
//        eEC = ER_TIMEOUT;
//      }
//      else
//      {
//        /*nothing*/
//  #ifdef DEBUG
//        //this is an undefined error state since the receive attempt loop only breaks with a valid response.
//        vDEBUG_RDIO_RCV_BOOTMSG("Rcv bootmsg SYS HALTED, undefined check error");
//        while(1){};
//  #endif
//      }
//    }
//    //check if radio response was as expected
//    else if(eEC == ER_VALID)
//    {
//      vDEBUG("Rcv bootmsg GOOD");
//      //todo notify HAL we are done receiving commands and to clear out buffers and control variables
//      eEC = ER_OK;
//    }else{/*do nothing*/}
//#ifdef DEBUG  //catch errors
//    if(eEC != ER_OK)
//    {
//      vDEBUG_RDIO_RCV_BOOTMSG("Rcv bootmsg SYS HALTED, iIneedmd_radio_rcv_boot_msg fail");
//      while(1){};
//    }else{/*do nothing*/}
//#endif
  }
  else
  {
    while(true)
    {
      if(i >= uiBuff_size)
      {
        eEC = ER_BUFF_SIZE;
        break;
      }else{/*do nothing*/}

      eEC = iRadio_rcv_char(&cRcv_string[i]);
      if(eEC == ER_OK)
      {
        i++;
      }
      else if(eEC == ER_NODATA)
      {
        uiClock_time_current = Clock_getTicks();
        uiClock_time_waiting_for_boot = (uiClock_time_current - uiClock_time_start);
        if(uiClock_time_waiting_for_boot >= 1000)
        {
          eEC = ER_TIMEOUT;
          break;
        }
        continue;
      }else{/*no data*/}

      if(eBootState == BOOT_PHASE_MFG)
      {
        cBoot_string = strstr((char *)&cRcv_string[uiBoot_index], READY);
        if(cBoot_string != NULL)
        {
          vDEBUG_RDIO_RCV_BOOTMSG("Rdo boot msg, RFD dev rdy");
          eBootState = BOOT_PHASE_READY;
          eEC = ER_VALID;
          break;
        }
      }
      else if(eBootState == BOOT_PHASE_BOOT_POST)
      {
        cBoot_string = strstr((char *)&cRcv_string[uiBoot_index], "Bluegiga Technologies Inc.");
        if(cBoot_string != NULL)
        {
          uiBoot_index = (cBoot_string - cRcv_string) + strlen("Bluegiga Technologies Inc.");
          vDEBUG_RDIO_RCV_BOOTMSG("Rdo boot msg, got mfg");
          eBootState = BOOT_PHASE_MFG;
        }
      }
      else if(eBootState == BOOT_PHASE_START)
      {
        cBoot_string = strstr((char *)cRcv_string, "WRAP THOR AI");
        if(cBoot_string != NULL)
        {
          uiBoot_index = (cBoot_string - cRcv_string) + strlen("WRAP THOR AI");
          vDEBUG_RDIO_RCV_BOOTMSG("Rdo boot msg, got boot post");
          eBootState = BOOT_PHASE_BOOT_POST;
        }
      }
      else
      {
        eEC = ER_INVALID;
      }
      eEC = ER_INVALID;
    }
  }

#ifdef DEBUG
  //error code check
  if((eEC != ER_VALID)     & \
     (eEC != ER_INVALID)   & \
     (eEC != ER_TIMEOUT)   & \
     (eEC != ER_BUFF_SIZE))
  {
    vDEBUG_RDIO_RCV_BOOTMSG("Radio rcv boot msg SYS HALT, invalid error code to return");
    while(1){};
  }
#endif

  return eEC;
#undef vDEBUG_RDIO_RCV_BOOTMSG
}

/*
 * get message from the radio
 */
ERROR_CODE eIneedmd_radio_rcv_string(char *cRcv_string, uint16_t uiBuff_size, uint16_t * puiRcvd_str_len)
{
  int i = 0;
  ERROR_CODE eEC = ER_FAIL;
  eEC = eUsing_radio_uart_dma();

  //Zero out the number or received characters
  if(puiRcvd_str_len != NULL)
  {
    *puiRcvd_str_len = 0;
  }else{/*do nothing*/}

  if(eEC == ER_TRUE)
  {
//    eEC = eRcv_dma_radio_cmnd_frame(cRcv_string, uiBuff_size);
//    if(eEC == ER_OK)
//    {
//      i = 1;
//    }else{/*do nothing*/}
  }
  else
  {
    while(true)
    {
      if(i == uiBuff_size)
      {
        eEC = ER_BUFF_SIZE;
        break;
      }else{/*do nothing*/}

      eEC = iRadio_rcv_char(&cRcv_string[i]);

      if(eEC == ER_TIMEOUT)
      {
        break;
      }else{/*do nothing*/}

      if(cRcv_string[i] == '\n')
      {
        if(puiRcvd_str_len != NULL)
        {
          *puiRcvd_str_len = strlen(cRcv_string);
          eEC = ER_OK;
          break;
        }
      }
      else
      {
        i++;
      }
    }
  }

  return eEC;
}

/*
 * get the settings from the radio
 */
ERROR_CODE iIneedmd_radio_rcv_settings(char *cRcv_string, uint16_t uiBuff_size)
{
#define DEBUG_iIneedmd_radio_rcv_settings
#ifdef DEBUG_iIneedmd_radio_rcv_settings
  #define  vDEBUG_RDIO_RCV_SET  vDEBUG
#else
  #define vDEBUG_RDIO_RCV_SET(a)
#endif
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
        }else{/*do nothing*/}
        continue;
      }
      else
      {
        /*nothing*/
  #ifdef DEBUG
        vDEBUG_RDIO_RCV_SET("Rcv settings SYS HALT, fail to rcv");
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
      }else{/*do nothing*/}

      eEC = iRadio_rcv_char(&cRcv_string[i]);

      if(eEC == ER_TIMEOUT)
      {
        break;
      }else{/*do nothing*/}

      if(i >= (ENDOF_SET_SETTINGS_STRLEN - 1))
      {
        if (strcmp(&cRcv_string[i - (ENDOF_SET_SETTINGS_STRLEN - 1)], ENDOF_SET_SETTINGS) == 0)
        {
          break;
        }else{/*do nothing*/}
      }else{/*do nothing*/}

      ++i;
    }
  }

  return eEC;
#undef vDEBUG_RDIO_RCV_SET
}

/******************************************************************************
* name: eIneedmd_radio_rcv_config
* description: this function parses the config receive frame from the radio.
*   During the parse it will look for the four 4 character hex parameters that
*   follow the SET CONTROL CONFIG frame headder. The 4 paramters are then retuned
*   in the array passed to the function in the parameter.
* param description:
* return value description:
******************************************************************************/
ERROR_CODE eIneedmd_radio_rcv_config(char * cRcv_string, uint16_t uiBuff_size, uint16_t * uiSetcfg_Param)
{
  ERROR_CODE eEC = ER_FAIL;
  uint16_t   iEC = 0;
  char cThrow_away[10];
  int iError = 0;

  eIneedmd_radio_rcv_string(cRcv_string, BG_SIZE, &iEC);

  iError = sscanf (cRcv_string, SET_CONTROL_CONFIG_PARSE, cThrow_away, cThrow_away, cThrow_away, uiSetcfg_Param++, uiSetcfg_Param++, uiSetcfg_Param++, uiSetcfg_Param);

  //check if the correct number of items were returned
  if(iError < 7) //todo: magic number!
  {
    eEC = ER_FAIL;
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
* param description: type - value: value description (in order from left to right)
*                    bool - true: do action when set to true
* return value description: ERROR_CODE - ER_CONNECTED:
*                                        ER_NOT_CONNECTED:
*                                        ER_TIMEOUT:
******************************************************************************/
ERROR_CODE eIneedmd_radio_wait_for_conn(tRadio_request * ptConn_request)
{
  ERROR_CODE eEC = ER_NOT_CONNECTED;
  ERROR_CODE eEC_serial_conn_status = ER_NOT_CONNECTED;
  ERROR_CODE eEC_protocol_conn_status = ER_NOT_CONNECTED;
  tRadio_request tConn_request;
  char * pcRcv_char = NULL;
  char * pcRing_string = NULL;
  char * pcRF_comm_string = NULL;
  char * cRing_conn_buff = cRdo_setup_rcv_buff;
  char * pcINMD_conn_req = cRdo_setup_rcv_buff;
  bool bDo_wait_forever_for_conn = false;
  uint32_t uiClock_time_conn_start = 0;
  uint32_t uiClock_time_conn_timeout = 0;
  uint32_t uiClock_time_current = 0;
  uint32_t uiClock_time_diff = 0;

  memcpy(&tConn_request, ptConn_request, sizeof(tRadio_request));
  memset(cRing_conn_buff,0x00, RING_CONNECTION_BUFF_SIZE);
  pcRcv_char = &cRing_conn_buff[(RING_CONNECTION_BUFF_SIZE - 1)];
  uiClock_time_conn_timeout = tConn_request.uiTimeout;
  if(uiClock_time_conn_timeout == TIMEOUT_CONN_WAIT_FOREVER)
  {
    bDo_wait_forever_for_conn = true;
    uiClock_time_conn_timeout = 100;
  }
  else
  {
    bDo_wait_forever_for_conn = false;
  }
  uiClock_time_conn_start = Clock_getTicks();

  //wait for the serial connection to be made
  while(eEC_serial_conn_status == ER_NOT_CONNECTED)
  {
    uiClock_time_current = Clock_getTicks();
    uiClock_time_diff = uiClock_time_current - uiClock_time_conn_start;
    if(uiClock_time_diff >= uiClock_time_conn_timeout)
    {
      //check wait forever control variable
      if(bDo_wait_forever_for_conn == true)
      {
        //the wait for connection is forever
        //
        //set the return code so the wait for connection will be called again
        eEC = ER_NOT_CONNECTED;
      }
      else
      {
        //the wait for connection has a timeout
        //
        //set the return code so the wait for connection will not be called again
        eEC = ER_TIMEOUT;
      }
      break;
    }else{/*do nothing*/}

    eEC = iRadio_rcv_char(pcRcv_char);

    //Check if the radio receive character was successful
    if(eEC == ER_OK)
    {
      //Radio data was recieved
      //
      //Reset the timeout control variable
      uiClock_time_conn_start = Clock_getTicks();

      //Check if the end of the ring message was reached
      if(cRing_conn_buff[(RING_CONNECTION_BUFF_SIZE - 1)] == '\n')
      {
        //End of the ring message reached
        //
        //Verify presense of the RING string
        pcRing_string = strstr(cRing_conn_buff, RING);
        if(pcRing_string != NULL)
        {
          //RING string is present
          //
          //Verity the presense of the RF COMM string
          pcRF_comm_string = strstr(cRing_conn_buff, RING_RFCOMM);
          if(pcRF_comm_string != NULL)
          {
            //RF COMM string is present
            //
            //set the connection status error code
            eEC_serial_conn_status = ER_CONNECTED;

            //Reset the timeout control variable
            uiClock_time_conn_start = Clock_getTicks();
            continue;
          }
          else
          {
            //RF COMM string was not present
            //
            //Set the error code to invalid and break from the loop
            eEC = ER_INVALID;
            break;
          }
        }
        else
        {
          //RING string was not present
          //
          //Set the error code to invalid and break from the loop
          eEC = ER_INVALID;
          break;
        }
      }
      else
      {
        //End of the ring message not reached
        //
        //Rotate the ring recieve buffer one byte forward
        memcpy(cRing_conn_buff, &cRing_conn_buff[1], (RING_CONNECTION_BUFF_SIZE - 1));
        //Set the last byte in the ring recieve buffer to 0
        *pcRcv_char = 0x00;
      }
    }
    else if(eEC == ER_NODATA)
    {
      //No data was received
      //
      //continue with the receive connection loop
      continue;
    }
    else
    {
      //Unknown return code from the radio recieve function
      //
      //Set the error code and break from the loop
      eEC = ER_FAIL;
      break;
    }
  }

  //Check if the radio connection was made
  if(eEC_serial_conn_status == ER_CONNECTED)
  {
    memset(pcINMD_conn_req, 0x00, INEEDMD_CMND_CONNECTION_FRAME_LEN);
    pcRcv_char = &pcINMD_conn_req[(INEEDMD_CMND_CONNECTION_FRAME_LEN - 1)];
    while(eEC_protocol_conn_status == ER_NOT_CONNECTED)
    {
      uiClock_time_current = Clock_getTicks();
      uiClock_time_diff = uiClock_time_current - uiClock_time_conn_start;
      if(uiClock_time_diff >= uiClock_time_conn_timeout)
      {
        //check wait forever control variable
        if(bDo_wait_forever_for_conn == true)
        {
          //the wait for connection is forever
          //
          //set the return code so the wait for connection will be called again
          eEC = ER_NOT_CONNECTED;
        }
        else
        {
          //the wait for connection has a timeout
          //
          //set the return code so the wait for connection will not be called again
          eEC = ER_TIMEOUT;
        }
        break;
      }else{/*do nothing*/}

      eEC = iRadio_rcv_char(pcRcv_char);

      //Check if the radio receive character was successful
      if(eEC == ER_OK)
      {
        //Radio data was recieved
        //
        //Reset the timeout control variable
        uiClock_time_conn_start = Clock_getTicks();

        //Check if the end of the ring message was reached
        if((pcINMD_conn_req[0] == INEEDMD_CMND_SOF) & \
           (pcINMD_conn_req[(INEEDMD_CMND_CONNECTION_FRAME_LEN - 1)] == INEEDMD_CMND_EOF))
        {
          eEC_protocol_conn_status = ER_CONNECTED;
          continue;
        }
        else
        {
          //End of the ring message not reached
          //
          //Rotate the protocol recieve buffer one byte forward
          memcpy(pcINMD_conn_req, &pcINMD_conn_req[1], (INEEDMD_CMND_CONNECTION_FRAME_LEN - 1));
          //Set the last byte in the ring recieve buffer to 0
          *pcRcv_char = 0x00;
        }
      }
      else if(eEC == ER_NODATA)
      {
        //No data was received
        //
        //continue with the receive connection loop
        continue;
      }
      else
      {
        //Unknown return code from the radio recieve function
        //
        //Set the error code and break from the loop
        eEC = ER_FAIL;
        break;
      }
    }
  }else{/*do nothing*/}

  if((eEC_serial_conn_status == ER_CONNECTED) & \
     (eEC_protocol_conn_status == ER_CONNECTED))
  {
    eEC = ER_CONNECTED;
  }
  else
  {
    if(eEC != ER_TIMEOUT)
    {
      eEC = ER_NOT_CONNECTED;
    }else{/*do nothing*/}
  }

  return eEC;
}

/******************************************************************************
* name:
* description:
* param description: type - value: value description (in order from left to right)
*                    bool - true: do action when set to true
* return value description: type - value: value description
******************************************************************************/
ERROR_CODE eSet_connection_state(eRadio_connection_state eConn_state)
{
  ERROR_CODE eEC = ER_FAIL;

  eCurrent_radio_connection_state = eConn_state;

  if(eCurrent_radio_connection_state != eConn_state)
  {
    eEC = ER_FAIL;
  }
  else
  {
    eEC = ER_OK;
  }

  return eEC;
}

#ifdef NOT_NOW
ERROR_CODE eIneedmd_radio_parse_default_settings(char * cSettings_buff)
{
  ERROR_CODE eEC = ER_FAIL;

  return eEC;
}

/******************************************************************************
* name: eIneedmd_radio_chk_event
* description: checks a frame for potential events from the radio
* param description:
* return value description:
******************************************************************************/
ERROR_CODE eIneedmd_radio_chk_event(char * cEvent_buff)
{
#define DEBUG_eIneedmd_radio_chk_event
#ifdef DEBUG_eIneedmd_radio_chk_event
  #define  vDEBUG_INMD_CHK_EVNT  vDEBUG
#else
  #define vDEBUG_INMD_CHK_EVNT(a)
#endif
  ERROR_CODE eEC = ER_OK;
  uint16_t uiStr_len = 0;
  bool bIs_radio_msg = false;
  bool bIs_event_msg = false;

  //radio events are sent in strings so strlen is valid to use here
  //check if the strlen is longer then 2 which is the minimum frame size for a radio event message
  //
  uiStr_len = strlen(cEvent_buff);
  if(uiStr_len <= 2)
  {
    //
    eEC = ER_INVALID;
  }
  else
  {
    eEC = ER_VALID;
  }

  if(eEC == ER_VALID)
  {
    if(cEvent_buff[uiStr_len - 1] == '\n')
    {
      bIs_radio_msg = strstr((char *)&cEvent_buff[uiStr_len - 2], CR_NL);
      if(bIs_radio_msg == true)
      {
        bIs_event_msg = strstr((char *)cEvent_buff, "BATTERY LOW");
        if(bIs_event_msg == true)
        {
          vDEBUG_INMD_CHK_EVNT("INMD chk evnt, radio batt low");
          eActive_radio_events = RDIO_EVENT_BATT_LOW;
          eEC = ER_EVENT;
        }else{/*do nothing*/}

        bIs_event_msg = strstr((char *)cEvent_buff, "BATTERY SHUTDOWN");
        if(bIs_event_msg == true)
        {
          vDEBUG_INMD_CHK_EVNT("INMD chk evnt, radio batt shutdown");
          eActive_radio_events = RDIO_EVENT_BATT_SHUTDOWN;
          eEC = ER_EVENT;
        }else{/*do nothing*/}
      }else{/*do nothing*/}
    }else{/*do nothing*/}
  }else{/*do nothing*/}

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
  }else{/*do nothing*/}

  memset(uiSend_Frame, 0x00, 512);

  for(uiIndex = 0; uiIndex <= (uiFrame_len - 1); uiIndex++)
  {
    uiSend_index += snprintf((char *)&uiSend_Frame[uiSend_index], 512, cHex_format, uiFrame[uiIndex]);
  }

  vRADIO_ECHO((char *)uiSend_Frame);
}
#endif

ERROR_CODE eIneedMD_radio_process_init(void)
{
  ERROR_CODE eEC = ER_OK;

  return eEC;
}

#endif //#ifdef NOT_NOW

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
eRadio_connection_state eGet_connection_state(void)
{
  return eCurrent_radio_connection_state;
}

/******************************************************************************
* name: eGet_radio_setup_state
* description: returns the radio's current setup state. To be used during initalization
*              and runtime to determine if the radio is ready to transmit and receive.
*
* param description: none
*
* return value description: eRadio_setup_state - enum: the current setup state of the radio
******************************************************************************/
eRadio_setup_state eGet_radio_setup_state(void)
{
  return eCurrent_radio_setup_state;
}

ERROR_CODE eSet_radio_setup_state(eRadio_setup_state eState)
{
  ERROR_CODE eEC = ER_FAIL;
  if(eState >= RDIO_SETUP_LIMIT)
  {
    eEC = ER_PARAM;
  }
  else
  {
    eCurrent_radio_setup_state = eState;
  }

  if(eCurrent_radio_setup_state != eState)
  {
    eEC = ER_FAIL;
  }
  else
  {
    eEC = ER_OK;
  }

  return eEC;
}

/******************************************************************************
* name: ineedmd_radio_power
* description: This function controls the power enable setting for the radio as
* well as the radio UART. The radio UART is not needed if the radio is powered
* off. In order to save power, the uart is disabled when the radio is powered
* down. When the radio is powered back up it will re-enable the radio uart.
* param description: bool - true: will perform radio power up
*                    bool - false: will perform radio power down
* return value description:
******************************************************************************/
void ineedmd_radio_power(bool power_up)
{
  if (power_up == true)
  {
    //power on the radio
    iRadio_Power_On();

    //todo: add a CTS check to make sure radio is on
  }
  else
  {
    //otherwise sets it to low
    //		GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_RADIO_ENABLE_PIN, 0x00);
    iRadio_Power_Off();

    //todo: add a CTS check to make sure radio is off
    // Disables the UART
//    UARTDisable(INEEDMD_RADIO_UART);
//    // then get rid of the FIFO
//    UARTFIFODisable(INEEDMD_RADIO_UART);
  }
}

/******************************************************************************
* name: ineedmd_radio_reset
* description: This function will perform a software reset on the radio. It
* calls the bsp function that performs the actual pin toggling and delay
* between toggles.
* param description:
* return value description:
******************************************************************************/
void ineedmd_radio_reset(void)
{
  //perform the reset pin toggle
  eBSP_Radio_Reset();
}
#ifdef NOT_NOW
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

ERROR_CODE eIneedmd_radio_rfd(void)
{
#define DEBUG_eIneedmd_radio_rfd
#ifdef DEBUG_eIneedmd_radio_rfd
  #define  vDEBUG_RDIO_RFD  vDEBUG
#else
  #define vDEBUG_RDIO_RFD(a)
#endif
  ERROR_CODE eEC = ER_FAIL;
  ERROR_CODE eEC_radio_in_mux_mode = ER_NO;
#define RFD_SEND_SZ  64
  char cRFD_Send_buff[RFD_SEND_SZ];
#define RFD_RCV_SZ   128
  char cRFD_Rcv_buff[RFD_RCV_SZ];
  uint16_t uiBaud_index = 0;
  uint32_t uiBaud_to_set = 0;
  uint32_t uiSystem_Baud = 0;
  uint32_t uiCurrent_Baud = 0;
  uint32_t i_delay = 0;

  //get the current system baud
  eBSP_Get_radio_uart_baud(&uiSystem_Baud);

  //set the radio to command mode
  iIneedmd_radio_cmnd_mode(true);

  //clear any data in the UART
  eRadio_clear_rcv_buffer();

  ineedmd_radio_reset();
  eEC = eGet_Radio_CTS_status();
  while(eEC == ER_FALSE)
  {
    eEC = eGet_Radio_CTS_status();
    if(eEC == ER_TRUE)
    {
      //clear any data in the UART
      eRadio_clear_rcv_buffer();
    }else{/*do nothing*/}
  }

  //attempt to get the radio out of mux mode if it is in it
  if(eEC_radio_in_mux_mode == ER_YES)
  {
    eBT_RDIO_mux_mode_disable();
  }else{/*do nothing*/}

  //SET RESET, reset to factory defaults
  //
  eEC = eSend_Radio_CMND(SET_RESET, strlen(SET_RESET));
  //Wait for the radio CTS pin to go low signifying reset
  eEC = eGet_Radio_CTS_status();
  while(eEC == ER_TRUE)
  {
    //delay for 1ms
    i_delay += iHW_delay(1);
    //check if the timeout was reached
    if(i_delay == TIMEOUT_RFD_CTS_HL)
    {
      eEC = ER_TIMEOUT;
      break;
    }else{/*do nothing*/}

    //get the cts status
    eEC = eGet_Radio_CTS_status();
    if(eEC == ER_FALSE)
    {
      //cts went from high to low signifying radio went into rfd
      eEC = ER_OK;

      //check if the current system baud is different then the default radio baud rate
      eBSP_Get_radio_uart_baud(&uiCurrent_Baud);
      if(uiCurrent_Baud != INEEDMD_RADIO_UART_DEFAULT_BAUD)
      {
        //the current baud is not the same speed as the radio default baud
        //set the system to the default radio baud
        eBSP_Set_radio_uart_baud(INEEDMD_RADIO_UART_DEFAULT_BAUD);
      }
      break;
    }else{/*do nothing*/}
  }

  //check if radio successfully RFD'ed
  if(eEC == ER_OK)
  {
    vDEBUG_RDIO_RFD("Rdio rfd, SET RESET, RFD");
    vRADIO_ECHO(SET_RESET);

    //wait for radio to come back from RFD by monitoring the CTS pin transition from low to high
    eEC = eGet_Radio_CTS_status();
    while(eEC == ER_FALSE)
    {
      eEC = eGet_Radio_CTS_status();
    }

    //check if the radio is back from RFD
    if(eEC == ER_TRUE)
    {
      //radio came back from RFD, preparing to receive the boot message
      memset(cRFD_Rcv_buff, 0x00, RFD_RCV_SZ);
      eEC = iIneedmd_radio_rcv_boot_msg(cRFD_Rcv_buff, RFD_RCV_SZ);
      if(eEC == ER_VALID)
      {
        vRADIO_ECHO(cRFD_Rcv_buff);

        //set the baud rate to system required baud speed
//        memset(cRFD_Send_buff, 0x00, RFD_SEND_SZ);
//        snprintf(cRFD_Send_buff, RFD_SEND_SZ, SET_CONTROL_BAUD, INEEDMD_RADIO_UART_BAUD, SET_CONTROL_BAUD_PARITY, SET_CONTROL_BAUD_STOP_BITS_1);
//        eEC = eSend_Radio_CMND(cRFD_Send_buff, strlen(cRFD_Send_buff));
//        if(eEC == ER_OK)
//        {
//          vDEBUG_RDIO_RFD("Rdio rfd, SET CONTROL BAUD, set the baud rate");
//          vRADIO_ECHO(cRFD_Send_buff);
//        }else{/*do nothing*/}
//
//        //set the baud rate
//        eBSP_Set_radio_uart_baud(INEEDMD_RADIO_UART_BAUD);
//        memset(cRFD_Rcv_buff, 0x00, RFD_RCV_SZ);
//        iIneedmd_radio_rcv_string(cRFD_Rcv_buff, RFD_RCV_SZ);
//        vRADIO_ECHO(cRFD_Rcv_buff);
      }else{/*do nothing*/}
    }else{/*do nothing*/}
  }
  else
  {
    //The radio did not transition into a RFD via CTS pin signalling. This means the radio did not receive
    //the request to RFD properly. Attempt to cycle through baud rates to RFD radio.
    //
    //init control variables
    i_delay = 0;
    uiBaud_index = 0;
    eEC = ER_FAIL;

    while(eEC != ER_VALID)
    {
      //Change baud rate to find the one the radio is using
      vDEBUG_RDIO_RFD("Rdio rfd, no response to SET RESET, changing baud rate");
      if(uiBaud_index == 0)
        uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_57600;
      else if(uiBaud_index == 1)
        uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_76800;
      else if(uiBaud_index == 2)
        uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_115200;
      else if(uiBaud_index == 3)
        uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_230400;
      else if(uiBaud_index == 4)
        uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_460800;
      else if(uiBaud_index == 5)
        uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_921600;
      else if(uiBaud_index == 6)
        uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_1382400;
      else
      {
        vDEBUG_RDIO_RFD("Rdio rfd, Baud selection failure");
        while(1){};
      }

      //set the baud rate
      eBSP_Set_radio_uart_baud(uiBaud_to_set);

      //soft reset the radio
      ineedmd_radio_reset();
      eEC = eGet_Radio_CTS_status();
      while(eEC == ER_FALSE)
      {
        eEC = eGet_Radio_CTS_status();
        if(eEC == ER_TRUE)
        {
          vDEBUG_RDIO_RFD("Rdio rfd, radio was soft reset");
        }else{/*do nothing*/}
      }

      //clear any potential uart traffic
      eSend_Radio_CMND(CR_NL, strlen(CR_NL));
      eRadio_clear_rcv_buffer();
      i_delay = 0;

      //send the reset command
      eSend_Radio_CMND(SET_RESET, strlen(SET_RESET));

      //wait for the radio to go into RFD by monitoring CTS pin transition from high to low
      eEC = eGet_Radio_CTS_status();
      while(eEC == ER_TRUE)
      {
        //delay for 1ms
        i_delay += iHW_delay(1);
        eEC = eGet_Radio_CTS_status();
        //check if the timeout was reached
        if(i_delay == TIMEOUT_RFD_CTS_HL)
        {
          vDEBUG_RDIO_RFD("Rdio rfd, radio RFD CTS HL monitoring timed out");
          eEC = ER_TIMEOUT;
          break;
        }else{/*do nothing*/}
      }

      //check if the radio did not go into RFD
      if(eEC == ER_TIMEOUT)
      {
        //set control variables
        uiBaud_index++;
        continue;
      }
      else if(eEC == ER_FALSE)
      {
        //the radio did go into RFD by CTS pin transition from high to low
        eEC = eGet_Radio_CTS_status();
        while(eEC == ER_FALSE)
        {
          eEC = eGet_Radio_CTS_status();
        }
        memset(cRFD_Rcv_buff, 0x00, RFD_RCV_SZ);
        eEC = iIneedmd_radio_rcv_boot_msg(cRFD_Rcv_buff, RFD_RCV_SZ);
        if(eEC == ER_VALID)
        {
          vRADIO_ECHO(cRFD_Rcv_buff);

          //set the baud rate to system required baud speed
          memset(cRFD_Send_buff, 0x00, RFD_SEND_SZ);
          snprintf(cRFD_Send_buff, RFD_SEND_SZ, SET_CONTROL_BAUD, INEEDMD_RADIO_UART_BAUD, SET_CONTROL_BAUD_PARITY, SET_CONTROL_BAUD_STOP_BITS_1);
          eEC = eSend_Radio_CMND(cRFD_Send_buff, strlen(cRFD_Send_buff));
          if(eEC == ER_OK)
          {
            vDEBUG_RDIO_RFD("Rdio rfd, SET CONTROL BAUD, set the baud rate");
            vRADIO_ECHO(cRFD_Send_buff);
          }else{/*do nothing*/}

          //set the baud rate
          eBSP_Set_radio_uart_baud(INEEDMD_RADIO_UART_BAUD);
          memset(cRFD_Rcv_buff, 0x00, RFD_RCV_SZ);
          iIneedmd_radio_rcv_string(cRFD_Rcv_buff, RFD_RCV_SZ);
          vRADIO_ECHO(cRFD_Rcv_buff);

          break;
        }
        //check if the rcv boot message timed out
        else if(eEC == ER_TIMEOUT)
        {
          //clear any potential uart traffic
          eRadio_clear_rcv_buffer();
          eEC = ER_FAIL;
        }else{/*do nothing*/}
      }
    }
  }

  return eEC;

#undef vDEBUG_RDIO_RFD
}
#endif
/*
 * send message to the radio
 * This function in it's current state just passes the parameters to the HAL.
 * It is meant to be here for future api, rtos, parsing use.
 */
void ineedmd_radio_send_string(char *send_string, uint16_t uiBuff_size)
{
//#define DEBUG_ineedmd_radio_send_string
#ifdef DEBUG_ineedmd_radio_send_string
  #define  vDEBUG_RDIO_SND_STR  vDEBUG
#else
  #define vDEBUG_RDIO_SND_STR(a)
#endif
  int i;

  i = iRadio_send_string(send_string, uiBuff_size);
#ifdef DEBUG
  if(i < strlen(send_string))
  {
    vDEBUG_RDIO_SND_STR("Rdio send str SYS HALT, send string failed");
    while(1){};
  }else{/*do nothing*/}
#endif //DEBUG

  //todo return i;

#undef vDEBUG_RDIO_SND_STR
}



#ifdef NOT_NOW
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
ERROR_CODE eIneedmd_radio_int_rcv_frame(uint8_t *uiRcv_frame, uint16_t uiBuff_size, uint32_t * uiBytes_rcvd)
{
#define DEBUG_iIneedmd_radio_int_rcv_frame
#ifdef DEBUG_iIneedmd_radio_int_rcv_frame
  #define  vDEBUG_INMD_INT_RCV_FRM  vDEBUG
#else
  #define vDEBUG_INMD_INT_RCV_FRM(a)
#endif
  int i;
  ERROR_CODE eEC = ER_FAIL;
  for(i = 0; i < uiBuff_size; i++)
  {
    iRadio_rcv_byte(&uiRcv_frame[i]);
//    MAP_UARTIntClear(INEEDMD_RADIO_UART, MAP_UARTIntStatus(INEEDMD_RADIO_UART, true));

    if(uiRcv_frame[i] == INEEDMD_CMND_EOF)
    {
      //increment i to compensate for the last byte
      i++;
      *uiBytes_rcvd = i;
      eEC = ER_OK;
      break;
    }else{/*do nothing*/}

    //check if the recieved frame is a radio event frame
    eEC = eIneedmd_radio_chk_event((char *)uiRcv_frame);
    if(eEC == ER_EVENT)
    {
      //the recieve frame is an event frame, break from the loop
      //increment i to compensate for the last byte
      i++;
      *uiBytes_rcvd = i;
      break;
    }else{/*do nothing*/}
  }

  return eEC;
#undef vDEBUG_INMD_INT_RCV_FRM
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
#endif //#ifdef NOT_NOW
/******************************************************************************
* name:iIneedMD_radio_setup
* description: this function sets the radio interface for the corresponding
*   platform. It should only be called once during inital device startup. Refer
*   to other functions to wake the radio up when coming out of a hibernate or
*   system sleep mode.
*   The function executes a relativley lengthy setup process for the defined
*   platform radio. When this function is called the process assumes that the
*   radio is in an unknown state and will be RFD'ed and setup for the entire
*   system specific application. Any previous settings will be erased.
* param description: none
* return value description: ERROR_CODE - ER_OK: radio successfully setup
*                           ERROR_CODE - ER_DONE: radio was already successfully setup
*                           ERROR_CODE - ER_TIMEOUT: radio was unresponsive to setup
*                           ERROR_CODE - ER_FAIL: radio failed setup
*                           ERROR_CODE - ER_UNKNOWN_INTERFACE: the wireless radio to
*                                        setup is unknown.
******************************************************************************/
ERROR_CODE eIneedMD_radio_setup(void)
{
#define DEBUG_iIneedMD_radio_setup
#ifdef DEBUG_iIneedMD_radio_setup
  #define  vDEBUG_RDIO_SETUP  vDEBUG
#else
  #define vDEBUG_RDIO_SETUP(a)
#endif
  uint16_t iEC;
  ERROR_CODE eEC = ER_OK;
  ERROR_CODE eEC_radio_in_mux_mode = ER_NO;
  char cSend_buff[BG_SIZE];
//  char cRcv_buff[BG_SIZE];
  char * cRcv_buff = cRdo_setup_rcv_buff;
  uint32_t ui32SysClock = MAP_SysCtlClockGet();
  uint16_t uiBaud_index = 0;
  uint32_t uiBaud_to_set = 0;
  uint32_t uiSystem_Baud = 0;
  uint32_t uiCurrent_Baud = 0;
  uint32_t i_delay = 0;

  //check if the radio was already set up
  if(bIs_Radio_setup == false)
  {
    //radio was not already set up
    //
    //Check the radio type to set up
    if(uiIneedmd_radio_type == INEEDMD_BT_RADIO_PLATFORM)
    {
      //The radio type to set up is bluetooth
      //
      //preset the tx rx setup buffers
      memset(uiBT_addr,0x00, BT_MACADDR_NUM_BYTES);
      memset(uiRemote_dev_addr,0x00, BT_MACADDR_NUM_BYTES);
      memset(uiSet_ctrl_config,0x0000, 4); //todo: magic number!

      //enable radio interface
      eSet_radio_setup_state(RDIO_SETUP_ENABLE_INTERFACE);
      eEC = eRadio_interface_enable();
      eSet_radio_setup_state(RDIO_SETUP_INTERFACE_ENABLED);

      if(eEC == ER_OK)
      {
        //get the current system baud
        eBSP_Get_radio_uart_baud(&uiSystem_Baud);

      }

      //turn power on to the radio
      eSet_radio_setup_state(RDIO_SETUP_START_POWER);
      ineedmd_radio_power(true);
      eSet_radio_setup_state(RDIO_SETUP_FINISH_POWER);

      eRadio_clear_rcv_buffer();
      //todo: perform a DTR pin set and check if radio responsive to commands, will be released in board rev 5

      eIneedmd_radio_cmnd_mode(true);

      //clear any data in the UART
      eRadio_clear_rcv_buffer();

      //software reset the radio
      //
      vDEBUG_RDIO_SETUP("Rdio setup, radio software reset");
      eSet_radio_setup_state(RDIO_SETUP_START_SOFT_RESET);
      ineedmd_radio_reset();
      eEC = eGet_Radio_CTS_status();
      while(eEC == ER_FALSE)
      {
        eEC = eGet_Radio_CTS_status();
        if(eEC == ER_TRUE)
        {
          //clear any data in the UART
          eRadio_clear_rcv_buffer();
          eSet_radio_setup_state(RDIO_SETUP_FINISH_SOFT_RESET);
        }else{/*do nothing*/}
      }

      //attempt to get the radio out of mux mode if it is in it
      if(eEC_radio_in_mux_mode == ER_YES)
      {
        eBT_RDIO_mux_mode_disable();
      }else{/*do nothing*/}

      //SET RESET, reset to factory defaults
      //
      eSet_radio_setup_state(RDIO_SETUP_START_RFD);
      eEC = eSend_Radio_CMND(SET_RESET, strlen(SET_RESET));
      //Wait for the radio CTS pin to go low signifying reset
      eEC = eGet_Radio_CTS_status();
      while(eEC == ER_TRUE)
      {
        //delay for 1ms
//        eOSAL_delay(1, NULL);
        i_delay += iHW_delay(1);
//        i_delay += 1;
        //check if the timeout was reached
        if(i_delay == TIMEOUT_RFD_CTS_HL)
        {
          //time out was reached, the radio did not go into RFD, prep for another baud change RFD loop
          vDEBUG_RDIO_SETUP("Rdio setup, radio RFD CTS HL monitoring timed out");
          eEC = ER_TIMEOUT;
          break;
        }else{/*do nothing*/}

        //get the cts status
        eEC = eGet_Radio_CTS_status();
        if(eEC == ER_FALSE)
        {
          //cts went from high to low signifying radio went into rfd
          eEC = ER_OK;

          //check if the current system baud is different then the default radio baud rate
          eBSP_Get_radio_uart_baud(&uiCurrent_Baud);
          if(uiCurrent_Baud != INEEDMD_RADIO_UART_DEFAULT_BAUD)
          {
            //the current baud is not the same speed as the radio default baud
            //set the system to the default radio baud
            eBSP_Set_radio_uart_baud(INEEDMD_RADIO_UART_DEFAULT_BAUD);
          }
          break;
        }else{/*do nothing*/}
      }

      //check if radio successfully RFD'ed
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET RESET, RFD");
        vRADIO_ECHO(SET_RESET);

        //wait for radio to come back from RFD by monitoring the CTS pin transition from low to high
        eEC = eGet_Radio_CTS_status();
        while(eEC == ER_FALSE)
        {
          eEC = eGet_Radio_CTS_status();
        }

        vDEBUG_RDIO_SETUP("Rdio setup, radio RFD CTS transitioned LH");

        //check if the radio is back from RFD
        if(eEC == ER_TRUE)
        {
          //radio came back from RFD, preparing to receive the boot message
          memset(cRcv_buff, 0x00, BG_SIZE);
          eEC = iIneedmd_radio_rcv_boot_msg(cRcv_buff, BG_SIZE);
          if(eEC == ER_VALID)
          {
            vRADIO_ECHO(cRcv_buff);

            //set the baud rate to system required baud speed
            memset(cSend_buff, 0x00, BG_SIZE);
            snprintf(cSend_buff, BG_SIZE, SET_CONTROL_BAUD, INEEDMD_RADIO_UART_BAUD, SET_CONTROL_BAUD_PARITY, SET_CONTROL_BAUD_STOP_BITS_1);
            eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
            if(eEC == ER_OK)
            {
              //get the echoed set control buffer
              memset(cRcv_buff, 0x00, BG_SIZE);
              eEC = eIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE, &iEC);
              if(iEC != 0)
              {
                vRADIO_ECHO(cRcv_buff);
              }
              vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL BAUD, set the baud rate");
//              vRADIO_ECHO(cSend_buff);
            }else{/*do nothing*/}

            //set the systems baud rate
            eBSP_Set_radio_uart_baud(INEEDMD_RADIO_UART_BAUD);
          }else{/*do nothing*/}
        }else{/*do nothing*/}
      }
      else
      {
        //The radio did not transition into a RFD via CTS pin signalling. This means the radio did not receive
        //the request to RFD properly. Attempt to cycle through baud rates to RFD radio.
        //
        //init control variables
        i_delay = 0;
        uiBaud_index = 0;
        eEC = ER_FAIL;

        while(eEC != ER_VALID)
        {
          //Change baud rate to find the one the radio is using
          vDEBUG_RDIO_SETUP("Rdio setup, no response to SET RESET, changing baud rate");
          if(uiBaud_index == 0)
            uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_115200;
          else if(uiBaud_index == 1)
            uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_1382400;
          else if(uiBaud_index == 2)
            uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_76800;
          else if(uiBaud_index == 3)
            uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_230400;
          else if(uiBaud_index == 4)
            uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_460800;
          else if(uiBaud_index == 5)
            uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_921600;
          else if(uiBaud_index == 6)
            uiBaud_to_set = INEEDMD_RADIO_UART_BAUD_57600;
          else
          {
            vDEBUG_RDIO_SETUP("Rdio setup, Baud selection failure");
            while(1){};
          }

          //set the baud rate
          eBSP_Set_radio_uart_baud(uiBaud_to_set);

          //soft reset the radio
          ineedmd_radio_reset();
          eEC = eGet_Radio_CTS_status();
          while(eEC == ER_FALSE)
          {
            eEC = eGet_Radio_CTS_status();
            if(eEC == ER_TRUE)
            {
              vDEBUG_RDIO_SETUP("Rdio setup, radio was soft reset");
            }else{/*do nothing*/}
          }

          //clear any potential uart traffic
          eSend_Radio_CMND(CR_NL, strlen(CR_NL));
          eRadio_clear_rcv_buffer();
          i_delay = 0;

          //send the reset command
          eSend_Radio_CMND(SET_RESET, strlen(SET_RESET));

          //wait for the radio to go into RFD by monitoring CTS pin transition from high to low
          eEC = eGet_Radio_CTS_status();
          while(eEC == ER_TRUE)
          {
            //delay for 1ms
            i_delay += iHW_delay(1);
            eEC = eGet_Radio_CTS_status();
            //check if the timeout was reached
            if(i_delay == TIMEOUT_RFD_CTS_HL)
            {
              //time out was reached, the radio did not go into RFD, prep for another baud change RFD loop
              vDEBUG_RDIO_SETUP("Rdio setup, radio RFD CTS HL monitoring timed out");
              eEC = ER_TIMEOUT;
              break;
            }else{/*do nothing*/}
          }

          //check if the radio did not go into RFD
          if(eEC == ER_TIMEOUT)
          {
            //Radio CTS pin did not transition in the timeout period, prep to go into another baud change RFD loop
            //set control variables
            uiBaud_index++;
            continue;
          }
          else if(eEC == ER_FALSE)
          {
            //the radio did go into RFD by CTS pin transition from high to low
            //
            //Poll the radio CTS pin
            eEC = eGet_Radio_CTS_status();
            while(eEC == ER_FALSE)
            {
              //radio is still resetting, keep polling the CTS status until it goes high
              eEC = eGet_Radio_CTS_status();
            }

            //The radio finished resetting, prepare to receive the boot message
            memset(cRcv_buff, 0x00, BG_SIZE);

            //recieve the boot message
            eEC = iIneedmd_radio_rcv_boot_msg(cRcv_buff, BG_SIZE);

            //check if the recieved boot message vas valid
            if(eEC == ER_VALID)
            {
              //The boot message was valid and the radio successfully RFD'ed
              //
              vRADIO_ECHO(cRcv_buff);

              //set the radio's baud rate to system required baud speed
              memset(cSend_buff, 0x00, BG_SIZE);
              snprintf(cSend_buff, BG_SIZE, SET_CONTROL_BAUD, INEEDMD_RADIO_UART_BAUD, SET_CONTROL_BAUD_PARITY, SET_CONTROL_BAUD_STOP_BITS_1);
              eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
              if(eEC == ER_OK)
              {
                //recieve the radio's baud change command echo
                memset(cRcv_buff, 0x00, BG_SIZE);
                eEC = eIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE, &iEC);
                if(iEC != 0)
                {
                  vRADIO_ECHO(cRcv_buff);
                }
                vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL BAUD, set the baud rate");
              }else{/*do nothing*/}

              //set the system's baud rate
              eBSP_Set_radio_uart_baud(INEEDMD_RADIO_UART_BAUD);

              break;
            }
            //check if the rcv boot message timed out
            else if(eEC == ER_TIMEOUT)
            {
              //Receing the boot message failed, prep for another baud change RFD loop
              //
              //clear any potential uart traffic
              eRadio_clear_rcv_buffer();
              //set the control variables
              uiBaud_index++;
              eEC = ER_FAIL;
            }else{/*do nothing*/}
          }
        }
      }

      eSet_radio_setup_state(RDIO_SETUP_FINISH_RFD);

      //SET CONTROL ECHO, set the radio echo
      //
      memset(cSend_buff, 0x00, BG_SIZE);
      snprintf(cSend_buff, BG_SIZE, SET_CONTROL_ECHO, SET_CONTROL_ECHO_SETTING);
      eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL ECHO, set the echo");
//        vRADIO_ECHO(cSend_buff);
      }else{/*do nothing*/}
      memset(cRcv_buff, 0x00, BG_SIZE);
      eEC = eIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE, &iEC);
      if(iEC != 0)
      {
        vRADIO_ECHO(cRcv_buff);
      }

      //SET BT AUTH, tell the radio what auth method we are using
      //
      memset(cSend_buff, 0x00, BG_SIZE);
      snprintf(cSend_buff, BG_SIZE, SET_BT_AUTH, SET_BT_AUTH_MODE, SET_BT_AUTH_PIN_CODE);
      eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET BT AUTH, set the BT auth method");
        vRADIO_ECHO(cSend_buff);
      }else{/*do nothing*/}

      //SET BT SSP, tell the radio we are using BT SSP pairing
      //
      memset(cSend_buff, 0x00, BG_SIZE);
      snprintf(cSend_buff, BG_SIZE, SET_BT_SSP, SET_BT_SSP_CPLTES, SET_BT_SSP_MITM);
      eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET BT SSP, set the SSP pairing");
        vRADIO_ECHO(cSend_buff);
      }else{/*do nothing*/}

      //SET PROFILE SPP, tells the radio we are using SPP protocol
      //
      memset(cSend_buff, 0x00, BG_SIZE);
      iEC = snprintf(cSend_buff, BG_SIZE, SET_PROFILE_SPP, SET_PROFILE_SPP_PARAM);
//      ineedmd_radio_send_string(cSend_buff, strlen(cSend_buff));
      eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET PROFILE SPP, set the SPP protocol");
        vRADIO_ECHO(cSend_buff);
      }else{/*do nothing*/}

      // SET CONTROL BATTERY sets the battery mode for the radio,
      // configures the, low bat warning voltage, the low voltage lock out,  the charge release voltage
      // and that this PIO pin to signal low batt is PIO 01
      //
      memset(cSend_buff, 0x00, BG_SIZE);
      iEC = snprintf(cSend_buff, BG_SIZE, SET_CONTROL_BATT, SET_CONTROL_BATT_LOW, SET_CONTROL_BATT_SHTDWN, SET_CONTROL_BATT_FULL, SET_CONTROL_BATT_MASK);
      eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET_CONTROL_BATT");
        vRADIO_ECHO(cSend_buff);
      }else{/*do nothing*/}

      //SET BT BDADDR, get BT address
      //
      eEC = eSend_Radio_CMND(SET_BT_BDADDR, strlen(SET_BT_BDADDR));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET BT BDADDR, get the BT mac addr");
        vRADIO_ECHO(SET_BT_BDADDR);
      }else{/*do nothing*/}
      memset(cRcv_buff, 0x00, BG_SIZE);
      eEC = eIneedmd_radio_rcv_string(cRcv_buff, BG_SIZE, &iEC);
      if(iEC != 0)
      {
        vRADIO_ECHO(cRcv_buff);
      }

      //parse string for the BT address
      //
      iEC = iIneedmd_parse_addr(cRcv_buff, uiBT_addr);
    #ifdef DEBUG
      if(iEC != 1)
      {
        vDEBUG_RDIO_SETUP("Rdio setup SYS HALT, no BT address found");
        while(1){}
      };//todo: proper error code, no magic number
    #endif

      //SET BT NAME, format the send string with the BT name
      //
      memset(cSend_buff, 0x00, BG_SIZE);
      snprintf(cSend_buff, BG_SIZE, SET_BT_NAME, uiBT_addr[4], uiBT_addr[5]);
      //send the new name
      eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET BT NAME, set the new bt name");
        vRADIO_ECHO(cSend_buff);
      }else{/*do nothing*/}

      //SET BT PAIRCOUNT, Set the max number of pairings
      //
      memset(cSend_buff, 0x00, BG_SIZE);
      snprintf(cSend_buff, BG_SIZE, SET_BT_PAIRCOUNT, SET_BT_PAIRCOUNT_MAX_PAIRINGS);
      eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET BT PAIRCOUNT, set max number of pairings");
        vRADIO_ECHO(cSend_buff);
      }else{/*do nothing*/}

      //SET CONTROL BIND, bind commands to PIO
      //
      memset(cSend_buff, 0x00, BG_SIZE);
      snprintf(cSend_buff, BG_SIZE, SET_CONTROL_BIND, SET_CONTROL_BIND_PRIORITY, SET_CONTROL_BIND_IO_MASK, SET_CONTROL_BIND_DIRECTION, SET_CONTROL_COMMAND);
      eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL BIND, set the PIO bind");
        vRADIO_ECHO(cSend_buff);
      }else{/*do nothing*/}

      //SET CONTROL VREGEN, use the VREG_ENA pin to turn radio power on or off
      //
      memset(cSend_buff, 0x00, BG_SIZE);
      snprintf(cSend_buff, BG_SIZE, SET_CONTROL_VREGEN, SET_CONTROL_VREGE_MODE, SET_CONTROL_VREGEN_PIOMASK);
      eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL VREGEN, use the VREG_ENA pin");
        vRADIO_ECHO(cSend_buff);
      }else{/*do nothing*/}

      //SET CONTROL CONFIG, GET the radio config
      //
      eEC = eSend_Radio_CMND(SET_CONTROL_CONFIG_GET, strlen(SET_CONTROL_CONFIG_GET));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL CONFIG, set the radio config");
        vRADIO_ECHO(SET_CONTROL_CONFIG_GET);
      }else{/*do nothing*/}
      memset(cRcv_buff, 0x00, BG_SIZE);
      iEC = eIneedmd_radio_rcv_config(cRcv_buff, BG_SIZE, uiSet_ctrl_config);
      if(iEC == ER_OK)
      {
        vRADIO_ECHO(cRcv_buff);
      }else{/*do nothing*/}

      //Set the control config bits in the config array according to the app
      uiSet_ctrl_config[0] |= SET_CONTROL_CONFIG_OPT_BLK_3;
      uiSet_ctrl_config[1] |= SET_CONTROL_CONFIG_OPT_BLK_2;
      uiSet_ctrl_config[2] |= SET_CONTROL_CONFIG_OPT_BLK_1;
      uiSet_ctrl_config[3] |= SET_CONTROL_CONFIG_CFG_BLK;

      //SET CONTROL CONFIG, config the radio to notify for certian events
      //
      memset(cSend_buff, 0x00, BG_SIZE);
      snprintf(cSend_buff, BG_SIZE, SET_CONTROL_CONFIG, uiSet_ctrl_config[0], uiSet_ctrl_config[1], uiSet_ctrl_config[2], uiSet_ctrl_config[3]);
      eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL CONFIG, config the radio to notify for certian events");
        vRADIO_ECHO(cSend_buff);
      }else{/*do nothing*/}

      //SET CONTROL ESCAPE, set the escape mode
      //
      memset(cSend_buff, 0x00, BG_SIZE);
      snprintf(cSend_buff, BG_SIZE, SET_CONTROL_ESCAPE, SET_CONTROL_ESCAPE_ESC_CHAR, SET_CONTROL_ESCAPE_DTR_MASK, SET_CONTROL_ESCAPE_DTR_MODE);
      eEC = eSend_Radio_CMND(cSend_buff, strlen(cSend_buff));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL ESCAPE, set the escape mode");
        vRADIO_ECHO(cSend_buff);
      }else{/*do nothing*/}

      //RESET, reset the radio to make the settings take hold
      //
      eEC = eSend_Radio_CMND(RESET, strlen(RESET));
      if(eEC == ER_OK)
      {
        vDEBUG_RDIO_SETUP("Rdio setup, RESET, perform software reset");
        vRADIO_ECHO(RESET);
      }else{/*do nothing*/}

      vDEBUG_RDIO_SETUP("Rdio setup, waiting for the radio to come back from reset");
      eEC = eGet_Radio_CTS_status();
      while(eEC == ER_FALSE)
      {
        eEC = eGet_Radio_CTS_status();
      }

      //SET, get the settings after perfroming the radio setup
  //todo: received settings > 1024. DMA Buffers >1024 causing problems, currently disabled due to returned size
  //    eEC = eSend_Radio_CMND(SET_SET, strlen(SET_SET));
  //    if(eEC == ER_OK)
  //    {
  //      vDEBUG_RDIO_SETUP("SET, get settings");
  //    }else{/*do nothing*/}
  //    memset(cRcv_buff, 0x00, BG_SIZE);
  //    eEC = iIneedmd_radio_rcv_settings(cRcv_buff, BG_SIZE);
  //    if(eEC == ER_OK)
  //    {
  //      vRADIO_ECHO(cRcv_buff);
  //    }else{/*do nothing*/}
  //    todo: verify settings properly set

      //set the connection status to false while waiting for an outside connection
      bIs_connection = false;

      //clear data pending status
      bRadio_is_data();

//      //enable the interrupt to the radio
//      iRadio_interface_int_enable();

      //set low the gpio pin to put the radio in command mode
      eIneedmd_radio_cmnd_mode(false);

      vDEBUG_RDIO_SETUP("Rdio setup, Radio ready");
      //Set the error code to done
      eEC = ER_DONE;
      eSet_radio_setup_state(RDIO_SETUP_READY);
    }
    else
    {
      //the radio to setup is unknown
      //
      //set the error code to signify unknown interface
      eEC = ER_UNKNOWN_INTERFACE;
  #ifdef DEBUG
      vDEBUG_RDIO_SETUP("Rdio setup SYS HALT, additional platforms are not defined");
      while(1){};
  #endif
    }
  }
  else
  {
    //Radio was already set up
    //
    //Set the error code to done
    eEC = ER_DONE;
  }

#ifdef DEBUG
  //error code check
  if((eEC != ER_OK)      & \
     (eEC != ER_TIMEOUT) & \
     (eEC != ER_FAIL)    & \
     (eEC != ER_DONE)    &\
     (eEC != ER_UNKNOWN_INTERFACE))
  {
    vDEBUG_RDIO_SETUP("Radio setup SYS HALT, invalid error code to return");
    while(1){};
  }
#endif
  return eEC;
#undef vDEBUG_RDIO_SETUP
}

void vIneedMD_radio_read_cb(UART_Handle sHandle, void *buf, int count)
//void vIneedMD_radio_read_cb(RADIO_INTERFACE_RXCB_PARAMS)
{
//  int i = 0;
//  char * c = (char *)buf;
  eRadio_connection_state eConn_state;
  if(count > 0)
  {
    eConn_state = eGet_connection_state();
    if(eConn_state == RADIO_CONN_WAITING_FOR_CONNECTION)
    {

//      i = strlen(cRdo_setup_rcv_buff);
////      cRdo_setup_rcv_buff[i] = *c;
////      i = strlen(cRdo_setup_rcv_buff);
//      if(i == RING_CONNECTION_STRING_LEN)
//      {
//        return;
//      }
//      else if(i == 0)
//      {
//        cRdo_setup_rcv_buff[0] = *c;
//        iRadio_rcv_char(&cRdo_setup_rcv_buff[1]);
//      }
//      else
//      {
//        iRadio_rcv_char(&cRdo_setup_rcv_buff[i]);
//      }
    }

    return;
  }
  else
  {
    return;
  }
}

//void vIneedMD_radio_write_cb(UART_Handle sHandle, void *buf, int count)
void vIneedMD_radio_write_cb(RADIO_INTERFACE_TXCB_PARAMS)
{
//#if defined(RADIO_INTERFACE_TXCB_PARAMS == TI_RTOS_UART_CB_PARAMS)
//#else
//#endif
  bDid_rcv_write_callback = true;

  return;
}

///*
// * Check for BT connection
// */
//int  iIneedMD_radio_check_for_connection(void)
//{
//#define DEBUG_radio_check_for_connection
//#ifdef DEBUG_radio_check_for_connection
//  #define  vDEBUG_RDIO_CHKCONN  vDEBUG
//#else
//  #define vDEBUG_RDIO_CHKCONN(a)
//#endif
//#define rc_chkbuff_size 128
//  uint32_t uiFrame_size_rcvd = 0;
//  ERROR_CODE eEC = ER_OK;
//  uint8_t uiRcv_buff[rc_chkbuff_size];
//  bool bIs_radio_data = false;
//
//  //check if connection with a remote device has been established
//  if(bIs_connection == false)
//  {
////    ineedmd_led_pattern(BT_ATTEMPTING);
//
//    //begin establishing a connection with a remote device
//    bIs_radio_data = bRadio_is_data();
//
//    if(bIs_radio_data == true)
//    {
//      memset(uiRcv_buff, 0x00, rc_chkbuff_size);
//      //receive the ssp confirm from the remote device
//      bIs_radio_data = UARTCharsAvail(INEEDMD_RADIO_UART);
//      eEC = eIneedmd_radio_int_rcv_frame(uiRcv_buff, rc_chkbuff_size, &uiFrame_size_rcvd);
//
//      //determine origin of the data frame
//      if(eEC == ER_OK)
//      {
//        vRADIO_ECHO_FRAME(uiRcv_buff, uiFrame_size_rcvd);
//
//        //check if the frame is a ineedMD command protocol frame
//        if(uiRcv_buff[0] == 0x9C)
//        {
//          iIneedmd_Rcv_cmnd_frame(uiRcv_buff, uiFrame_size_rcvd);
//        }
//      }
//      else if(eEC == ER_EVENT)
//      {
//        vDEBUG_RDIO_CHKCONN("Rdio chk conn, radio event!");
//      }else{/*do nothing*/}
//    }
//    else
//    {
//      if(bIs_connection == true)
//      {
//        return 1;
//      }
//    }
//  }
//
//  return 0;
//#undef vDEBUG_RDIO_CHKCONN
//}

/******************************************************************************
* name: eIneedmd_radio_txrx_params_init
* description: Initalizes the parameters that will be used to queue transmission
*  frames to the wireless radio. The initilizaion parameters are:
*    ePriority:   TXRX_PRIORITY_QUEUE
*    uiBuff_size: 0
*    vBuff:       NULL
* param description: tINMD_tx_rx_frame - pointer:Pointer to the parameters to
*                                        initalize.
*
* return value description: ERROR_CODE - ER_OK: if the parameters were properly
*                                        initalized.
*                           ERROR_CODE - ER_FAIL: if the parameters were not
*                                        properly intalized.
*                           ERROR_CODE - ER_PARAM: if the pointer passed in to
*                                        hold the parameters is not valid.
******************************************************************************/
ERROR_CODE eIneedmd_radio_request_params_init (tRadio_request * tParams)
{
  ERROR_CODE eEC = ER_FAIL;

  //verify that the pointer to use is valid
  if(tParams == NULL)
  {
    //pointer is invalid
    eEC = ER_PARAM;
  }
  else
  {
    //pointer is valid
    //
    //Init params
    memset(tParams->uiBuff, 0x00, 256); //todo magic number!
    tParams->uiBuff_size = 0;
    tParams->uiTimeout = 0;
    tParams->eRequest = RADIO_REQUEST_NONE;
    tParams->eSetting = RADIO_SETTINGS_NONE;
    tParams->eTX_Priority = TX_PRIORITY_QUEUE;
    tParams->vBuff_sent_callback = NULL;
    tParams->vBuff_receive_callback = NULL;
    tParams->vChange_setting_callback = NULL;
    tParams->vConnection_status_callback = NULL;


    //Check if params were properly set
    if((tParams->eRequest == RADIO_REQUEST_NONE)  &\
       (tParams->uiBuff_size == 0)                &\
       (tParams->eSetting == RADIO_SETTINGS_NONE) &\
       (tParams->eTX_Priority == TX_PRIORITY_QUEUE))
    {
      //Params were properly set
      eEC = ER_OK;
    }
    else
    {
      //Params were not properly set
      eEC = ER_FAIL;
    }
  }

  //return the error code
  return eEC;
}

/******************************************************************************
* name: eIneedmd_radio_request
* description: This function queues a data frame defined in the passed in
*  parameters to be sent out the wireless radio.
*
* param description: tINMD_tx_rx_frame - pointer: pointer to the parameter
*  structure that will be used to transmit a data frame.
*
* return value description: ERROR_CODE - ER_FAIL: frame to send was not queued
*                                        or transmitted.
*                           ERROR_CODE - ER_OK: frame was queued or transmitted.
*                           ERROR_CODE - ER_PARAM: pointer to the parameters or
*                                        parameter values are invalid.
*                           ERROR_CODE - ER_PARAM1: the priority parameter is
*                                        invalid.
*                           ERROR_CODE - ER_OFF: the radio is currently turned
*                                        off and needs to be powered on
*                                        ER_REQUEST: the request was invalid
******************************************************************************/
ERROR_CODE eIneedmd_radio_request(tRadio_request * tRequest)
{
  ERROR_CODE eEC = ER_FAIL;

  bool bDid_message_post = false;
  eRadio_connection_state eConn_State;
  eRadio_setup_state  eSetup_state;

  eSetup_state = eGet_radio_setup_state();

  if(eSetup_state == RDIO_SETUP_READY)
  {
    //Check if the passed in parameters are valid
    if(tRequest == NULL)
    {
      //the parameters were not valid
      eEC = ER_PARAM;
    }
    else
    {
      if(tRequest->eRequest < RADIO_REQUEST_LIMIT)
      {
        switch(tRequest->eRequest)
        {
          case RADIO_REQUEST_CHANGE_SETTINGS:
            bDid_message_post = Mailbox_post(tTIRTOS_Radio_mailbox, tRequest, BIOS_WAIT_FOREVER);
            break;
          case RADIO_REQUEST_SEND_FRAME:
            //check if the buffer size is within limits
            if(tRequest->uiBuff_size > 256) //todo: magic number, associate with the max protocol frame size
            {
              eEC = ER_BUFF_SIZE;
            }
            else
            {
              bDid_message_post = Mailbox_post(tTIRTOS_Radio_mailbox, tRequest, BIOS_WAIT_FOREVER);
            }

            break;
          case RADIO_REQUEST_RECEIVE_FRAME:
            bDid_message_post = Mailbox_post(tTIRTOS_Radio_mailbox, tRequest, BIOS_WAIT_FOREVER);
            break;
          case RADIO_REQUEST_WAIT_FOR_CONNECTION:
            eConn_State = eGet_connection_state();
            if(eConn_State == RADIO_CONN_NONE)
            {
  //            memset(cRdo_setup_rcv_buff,0x00, BG_SIZE);
  //            eBSP_Set_radio_uart_to_callback();
              bDid_message_post = Mailbox_post(tTIRTOS_Radio_mailbox, tRequest, BIOS_WAIT_FOREVER);
            }
  //          else if(eConn_State == RADIO_CONN_WAITING_FOR_CONNECTION)
  //          {
  //            Mailbox_post(tTIRTOS_Radio_mailbox, tRequest, BIOS_WAIT_FOREVER);
  //          }
            break;
          case RADIO_REQUEST_HALT_WAIT_FOR_CONNECTION:
            eConn_State = eGet_connection_state();
            if(eConn_State == RADIO_CONN_WAITING_FOR_CONNECTION)
            {
              eSet_connection_state(RADIO_CONN_HALT);
            }
            break;
          case RADIO_REQUEST_BREAK_CONNECTION:
            eConn_State = eGet_connection_state();
            if(eConn_State == RADIO_CONN_CONNECTED)
            {
              bDid_message_post = Mailbox_post(tTIRTOS_Radio_mailbox, tRequest, BIOS_WAIT_FOREVER);
            }
            break;
          default:
            bDid_message_post = false;
            break;
        }

        //check if the message was posted
        if(bDid_message_post == true)
        {
          eEC = ER_OK;
        }else{/*do nothing*/}
      }
      else
      {
        eEC = ER_REQUEST;
      }
    }
  }
  else if(eSetup_state == RDIO_SETUP_RADIO_READY_POWERED_OFF)
  {
    if(tRequest->eRequest == RADIO_REQUEST_POWER_ON)
    {
      bDid_message_post = Mailbox_post(tTIRTOS_Radio_mailbox, tRequest, BIOS_WAIT_FOREVER);
      if(bDid_message_post == true)
      {
        eEC = ER_OK;
      }
      else
      {
        eEC = ER_FAIL;
      }
    }
    else
    {
      eEC = ER_OFF;
    }
  }
  else
  {
    eEC = ER_NOT_READY;
  }

  return eEC;
}

ERROR_CODE  eIneedmd_radio_change_settings_params_init  (eRadio_Settings eRdo_set_request)
{
  ERROR_CODE eEC = ER_FAIL;

  return eEC;
}

/******************************************************************************
* name: eIneedmd_radio_change_settings
* description: changes the wireless radio settings
* param description: eRadio_Settings - RADIO_SETTINGS_RFD,
*                    eRadio_Settings - RADIO_SETTINGS_BAUD_115K,
*                    eRadio_Settings - RADIO_SETTINGS_BAUD_1382K,
*                    eRadio_Settings - RADIO_SETTINGS_APPLICATION_DEFAULT
*
* return value description: ERROR_CODE -
******************************************************************************/
ERROR_CODE  eIneedmd_radio_change_settings  (eRadio_Settings eRdo_set_request)
{
  ERROR_CODE eEC = ER_FAIL;
  bool bDid_message_post = false;
  eRadio_setup_state eRdo_State = RDIO_SETUP_UNKNOWN;

  eRdo_State = eGet_radio_setup_state();

  if(eRdo_State == RDIO_SETUP_RADIO_READY_POWERED_OFF)
  {
//    todo: added power on code
    while(1){};
  }

  if(eRdo_State == RDIO_SETUP_READY)
  {
    switch(eRdo_set_request)
    {
      case RADIO_SETTINGS_RFD:
        //todo: add change code
        break;
      case RADIO_SETTINGS_BAUD_115K:
        //todo: add change code
        break;
      case RADIO_SETTINGS_BAUD_1382K:
        //todo: add change code
        break;
      case RADIO_SETTINGS_APPLICATION_DEFAULT:
        //todo: add change code
        break;
      default:
        break;
    }

    //todo: add message post code
    if(bDid_message_post == true)
    {
      eEC = ER_OK;
    }
    else
    {
      eEC = ER_FAIL;
    }
  }
  else
  {
    eEC = ER_NOT_READY;
  }

  return eEC;
}

/*
 * Main radio task
 */
//int iIneedMD_radio_process(void)
void vIneedMD_radio_task(UArg a0, UArg a1)
{
#define DEBUG_iIneedMD_radio_task
#ifdef DEBUG_iIneedMD_radio_task
  #define  vDEBUG_RDIO_TASK  vDEBUG
#else
  #define vDEBUG_RDIO_TASK(a)
#endif
  ERROR_CODE eEC = ER_FAIL;

  eRadio_connection_state eConn_State;
  tRadio_request tRadio_msg;
  tINMD_protocol_req_notify tProtocol_req_notify;

  uint32_t uiMsg_size = sizeof(tRadio_request);
  uint32_t uiMbox_size = 0;
  uiMbox_size = Mailbox_getMsgSize(tTIRTOS_Radio_mailbox);
  if(uiMsg_size != uiMbox_size)
  {
    vDEBUG_RDIO_TASK("vIneedMD_radio_task SYS HALT, invalid mailbox msg size!");
    while(1){};
  }else{/*do nothing*/}

  eEC = eIneedMD_radio_setup();
  if((eEC == ER_OK) | (eEC == ER_DONE))
  {
    vDEBUG_RDIO_TASK("vIneedMD_radio_task, radio setup success");
    eSet_connection_state(RADIO_CONN_NONE);

    //turn the radio off untill needed
    eSet_radio_setup_state(RDIO_SETUP_RADIO_READY_POWERED_OFF);
    ineedmd_radio_power(false);
  }
  else
  {
    vDEBUG_RDIO_TASK("vIneedMD_radio_task SYS HALT, radio setup failure!");
    while(1){};
  }

//  eEC = eOSAL_Mailbox_Params_Init(&tRadio_mailbox);

//  eEC = eOSAL_Mailbox_Create(&tRadio_mailbox);

//  eIneedmd_radio_request_params_init(&tRadio_msg);
//  tRadio_msg.eRequest = RADIO_REQUEST_WAIT_FOR_CONNECTION;
//  eIneedmd_radio_request(&tRadio_msg);



  while(1)
  {
    if(Mailbox_pend(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_WAIT_FOREVER) == true)
//    if(eOSAL_Mailbox_Get_msg(&tRadio_mailbox) == ER_OK)
    {

      switch(tRadio_msg.eRequest)
      {
        case RADIO_REQUEST_CHANGE_SETTINGS:
          break;
        case RADIO_REQUEST_POWER_ON:
          ineedmd_radio_power(true);
          eEC = eGet_Radio_CTS_status();
          while(eEC == ER_FALSE)
          {
            Task_sleep(100);
            eEC = eGet_Radio_CTS_status();
          }

          eSet_radio_setup_state(RDIO_SETUP_READY);

          if(tRadio_msg.vSetup_state_callback != NULL)
          {
            tRadio_msg.vSetup_state_callback(RDIO_SETUP_READY);
          }
          break;
        case RADIO_REQUEST_POWER_OFF:
          break;
        case RADIO_REQUEST_SEND_FRAME:
          Task_sleep(100);
          break;
        case RADIO_REQUEST_RECEIVE_FRAME:
          break;
        case RADIO_REQUEST_WAIT_FOR_CONNECTION:
          eConn_State = eGet_connection_state();

          //check the connection state
          if(eConn_State == RADIO_CONN_HALT)
          {
            //A request to halt connection was requested
            //
            //check callback and notify connection halted
            continue;
          }
          else if((eConn_State == RADIO_CONN_NONE) |
                  (eConn_State == RADIO_CONN_WAITING_FOR_CONNECTION))
          {
            //No connection or waiting for connection state
            //
            //Set the state to waiting for connection
            eSet_connection_state(RADIO_CONN_WAITING_FOR_CONNECTION);

            eEC = eIneedmd_radio_wait_for_conn(&tRadio_msg);

            //Check the error codes
            if(eEC == ER_CONNECTED)
            {
              //Successfully connected to a remote device
              //

              //Set the radio connection state to connected
              eSet_connection_state(RADIO_CONN_CONNECTED);

              //check callback notify connection established
              if(tRadio_msg.vConnection_status_callback != NULL)
              {
                tRadio_msg.vConnection_status_callback(RADIO_CONN_CONNECTED);
              }else{/*do nothing*/}

              vDEBUG_RDIO_TASK("Radio task, radio connected!");

              eIneedmd_cmnd_Proto_ReqNote_params_init(&tProtocol_req_notify);
              tProtocol_req_notify.eReq_Notify = CMND_REQUEST_RCV_PROTOCOL_FRAME;
              tProtocol_req_notify.uiCmnd_Frame = (uint8_t *)cRdo_setup_rcv_buff;
              tProtocol_req_notify.uiFrame_Len = 5;
              eIneedmd_cmnd_Proto_Request_Notify(&tProtocol_req_notify);
            }
            else if(eEC == ER_TIMEOUT)
            {
              //Connection timeout exceeded
              //
              eSet_connection_state(RADIO_CONN_NONE);
              //check callback and notify connection timed out
              if(tRadio_msg.vConnection_status_callback != NULL)
              {
                tRadio_msg.vConnection_status_callback(RADIO_CONN_NONE);
              }else{/*do nothing*/}

            }
            else if(eEC == ER_NOT_CONNECTED)
            {
              //No connection detected
              //
              eConn_State = eGet_connection_state();
              if(eConn_State == RADIO_CONN_HALT)
              {
                eSet_connection_state(RADIO_CONN_NONE);
                //check callback and notify connection halted
              }
              else
              {
                eSet_connection_state(RADIO_CONN_NONE);
                eIneedmd_radio_request_params_init(&tRadio_msg);
                tRadio_msg.eRequest = RADIO_REQUEST_WAIT_FOR_CONNECTION;
                eIneedmd_radio_request(&tRadio_msg);
                //check callback and notify connection continuing
              }
            }
          }
          else if(eConn_State == RADIO_CONN_CONNECTED)
          {
            //Radio is already connected
            //
            //check callback and notfiy connection already established
            continue;
          }
          else
          {
            //invalid conn state
            //
            continue;
          }
          break;
        default:
          //error
          break;
      }
    }else{/*do nothing*/}
  }
//
//  iRadio_conn_status = iIneedMD_radio_check_for_connection();
//  if(iRadio_conn_status == 1)
//  {
//    bIs_connection = true;
//  }
//  else
//  {
//    bIs_connection = false;
//  }
//
//  if(bIs_connection == true)
//  {
//    bIs_data = bRadio_is_data();
//    if(bIs_data == true)
//    {
//      //receive the ssp confirm from the remote device
//      iIneedmd_radio_rcv_string(cRcv_buff, 128);
//    }
//  }
//
//  if(eActive_radio_events != RDIO_NO_EVENT)
//  {
//    if(eActive_radio_events == RDIO_EVENT_BATT_LOW)
//    {
//      eActive_radio_events = RDIO_NO_EVENT;
//    }
//    else if(eActive_radio_events == RDIO_EVENT_BATT_SHUTDOWN)
//    {
//      eActive_radio_events = RDIO_NO_EVENT;
//    }
//    else
//    {
//#ifdef DEBUG
//      vDEBUG_RDIO_PROCSS("Rdio proc SYS HALT, unknown radio event!");
//      while(1){};
//#endif
//    }
//  }else{/*do nothing*/}
//
//  return 1;

#undef vDEBUG_RDIO_TASK
}
/*
 * Get status from the radio
 */


/*
 * END
 */
#endif //__INEEDMD_BLUETOOTH_RADIO_C__
