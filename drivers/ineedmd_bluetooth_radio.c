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
#include "utils_inc/osal.h"

#include "board.h"
#include "drivers_inc/ineedmd_bluetooth_radio.h"
#include "drivers_inc/ineedmd_USB.h"
#include "drivers_inc/nvram.h"
#include "app_inc/ineedmd_command_protocol.h"
#include "utils_inc/proj_debug.h"

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

#define CR_NL                   "\r\n"
#define PIO_GET                 "PIO GET\r\n"
#define PIO_GETDIR              "PIO GETDIR\r\n"
#define PIO_GETDIR_SETTING      "PIO_GETDIR %.2x\r\n"
#define PIO_GETDIR_SETTING_LEN  14
#define PIO_GETBIAS             "PIO GETBIAS\r\n"
#define PIO_SET                 "PIO SET %.2x %.2x\r\n"
#define PIO_SETDIR              "PIO SETDIR %.2x %.2x\r\n"
#define PIO_SETBIAS             "PIO SETBIAS %.2x %.2x\r\n"
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
    #define PIO_SETDIR_MASK    PIO_MASK_PIO2
    #define PIO_SETDIR_STATE   PIO_STATE_LOW_PIO2
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

#define SET_BT_GET           "SET BT\r\n"

//#define SET_BT_NAME       "\r\nSET BT NAME IneedMD-00:07:80:0D:73:E0\r\n"
#define SET_BT_AUTH       "SET BT AUTH %c %.4d\r\n"
  #define SET_BT_AUTH_MODE_DISP     '*'
  #define SET_BT_AUTH_MODE_NO_DISP  '-'
    #define SET_BT_AUTH_MODE        SET_BT_AUTH_MODE_DISP
  #define SET_BT_AUTH_PIN_CODE      2124 //TODO: read out of flash maybe?
#define SET_BT_AUTH_RCV_SOF  "SET BT AUTH "
#define SET_BT_AUTH_RCV_LEN  20

#define SET_BT_WHAT_NAME  "SET BT NAME IneedMD-NUTS\r\n"
#define SET_BT_NAME       "SET BT NAME IneedMD-%x%x\r\n"
#define SET_BT_NAME_HDR   "SET BT NAME "
#define SET_BT_SYSTEM_NAME  "SET BT NAME %s\r\n"
#define SET_BT_NAME_RCV_LEN  26
#define SET_BT_PAGEMODE_ON  "SET BT PAGEMODE 4 2000 1\r\n"
#define SET_BT_PAGEMODE_OFF "SET BT PAGEMODE 0 2000 1\r\n"
#define SET_BT_PAIRCOUNT  "SET BT PAIRCOUNT %d\r\n"
    #define SET_BT_PAIRCOUNT_2             2
    #define SET_BT_PAIRCOUNT_4             4
    #define SET_BT_PAIRCOUNT_8             8
    #define SET_BT_PAIRCOUNT_MAX_PAIRINGS  16
  #define BT_PAIRCOUNT SET_BT_PAIRCOUNT_4
#define SET_BT_BDADDR        "SET BT BDADDR\r\n"  //gets the BT address, yes gets it even though it says SET
#define SET_BT_BDADDR_PARSE  "%s %s %s %hhx %c %hhx %c %hhx %c %hhx %c %hhx %c %hhx"
#define SET_BT_BDADDR_CHK    "SET BT BDADDR "
#define SET_BT_BDADDR_RCV_LEN  33
#define SET_BT_SSP      "SET BT SSP %d %d\r\n"
  #define SET_BT_SSP_CPBLTES_DISP_ONLY    0  //Display only
  #define SET_BT_SSP_CPBLTES_DISP_AND_YN  1  //Display + yes/no button
  #define SET_BT_SSP_CPBLTES_KEYBRD_ONLY  2  //Keyboard only
  #define SET_BT_SSP_CPBLTES_NONE         3  //None
    #define SET_BT_SSP_CPLTES             SET_BT_SSP_CPBLTES_NONE
  #define SET_BT_SSP_MITM_NO_PROT    0  //no man in the middle protection
  #define SET_BT_SSP_MITM_PROT       1  //man in the middle protection required
    #define SET_BT_SSP_MITM          SET_BT_SSP_MITM_NO_PROT
#define SET_BT_SSP_RCV_LEN    16

#define SET_PROFILE_SPP   "SET PROFILE SPP %s\r\n"
  #define SET_PROFILE_SPP_ON   "ON"
  #define SET_PROFILE_SPP_OFF  "OFF"
    #define SET_PROFILE_SPP_PARAM  SET_PROFILE_SPP_ON

#define SET_CONTROL_GET    "SET CONTROL\r\n"

#define SET_CONTROL_BATT  "SET CONTROL BATTERY %d %d %d %d\r\n"
  #define SET_CONTROL_BATT_LOW     INEEDMD_LOW_BATT_VOLTAGE
  #define SET_CONTROL_BATT_SHTDWN  INEEDMD_SHTDWN_BATT_VOLTAGE
  #define SET_CONTROL_BATT_FULL    INEEDMD_FULL_BATT_VOLTAGE
  #define SET_CONTROL_BATT_MASK    PIO_MASK_PIO1
#define SET_CONTROL_BATT_RCV_LEN   38

#define SET_CONTROL_BAUD  "SET CONTROL BAUD %d,8%c%d\r\n"
  #define SET_CONTROL_BAUD_PARITY_NONE  'N'
  #define SET_CONTROL_BAUD_PARITY_EVEN  'E'
  #define SET_CONTROL_BAUD_PARITY_ODD   'O'
    #define SET_CONTROL_BAUD_PARITY  SET_CONTROL_BAUD_PARITY_NONE
  #define SET_CONTROL_BAUD_STOP_BITS_1  1
  #define SET_CONTROL_BAUD_STOP_BITS_2  2
      #define SET_CONTROL_BAUD_STOP_BITS  SET_CONTROL_BAUD_STOP_BITS_1
#define SET_CONTROL_BAUD_GET  "SET CONTROL BAUD\r\n"
//#define SET_CONTROL_BIND    "SET CONTROL BIND %d %.4x %s %s\r\n"
#define SET_CONTROL_BIND    "SET CONTROL BIND %d %x %c %s\r\n"
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
  #define SET_CONTROL_BIND_DIRECTION_RISE    'R'//"RISE"
  #define SET_CONTROL_BIND_DIRECTION_FALL    'F'//"FALL"
  #define SET_CONTROL_BIND_DIRECTION_CHANGE  'C'//"CHANGE"
    #define SET_CONTROL_BIND_DIRECTION  SET_CONTROL_BIND_DIRECTION_RISE
    #define SET_CONTROL_COMMAND  "CLOSE 0"
#define SET_CONTROL_BIND_RCV_LEN   32

#define SET_CONTROL_CD "SET CONTROL CD %.2x %d\r\n"
    #define SET_CONTROL_CD_MASK    PIO_MASK_PIO2
  #define SET_CONTROL_CD_MODE_CONNECTION     0
  #define SET_CONTROL_CD_MODE_DATA           1
  #define SET_CONTROL_CD_MODE_CONN_AND_DATA  2
    #define SET_CONTROL_CD_MODE    SET_CONTROL_CD_MODE_CONNECTION
    #define SET_CONTROL_CD_DATA_MODE_MASK    PIO_MASK_NONE
#define SET_CONTROL_CD_RCV_LEN   21

#define SET_CONTROL_CONFIG_GET  "SET CONTROL CONFIG\r\n"
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
#define SET_CONTROL_CONFIG_RCV_LEN  40

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
                                      | SET_CONTROL_ECHO_SET_EVENT_OFF  \
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
#define SET_CONTROL_ESCAPE_RCV_LEN   27

#define SET_CONTROL_MUX  "\r\nSET CONTROL MUX %d\r\n"
  #define SET_CONTROL_MUX_MODE_DISABLE  0
  #define SET_CONTROL_MUX_MODE_ENABLE   1
    #define SET_CONTROL_MUX_MODE  SET_CONTROL_MUX_MODE_ENABLE
#define SET_CONTROL_MUX_HEX_DISABLE

#define SET_CONTROL_PIO  "SET CONTROL PIO %.2x %.2x %.2x\r\n"
    #define SET_CONTROL_PIO_DIR     (PIO_MASK_PIO2)
    #define SET_CONTROL_PIO_BIAS    PIO_MASK_NONE
    #define SET_CONTROL_PIO_IGNORE  PIO_MASK_NONE

#define SET_CONTROL_VREGEN  "SET CONTROL VREGEN %d %.2x\r\n"
  #define SET_CONTROL_VREGEN_MODE_0  0
  #define SET_CONTROL_VREGEN_MODE_1  1
  #define SET_CONTROL_VREGEN_MODE_2  2
    #define SET_CONTROL_VREGE_MODE SET_CONTROL_VREGEN_MODE_1
  #define SET_CONTROL_VREGEN_PIOMASK  PIO_MASK_NONE
#define SET_CONTROL_VREGEN_RCV_LEN   25

#define SSP_CONFIRM  "\r\nSSP CONFIRM %x:%x:%x:%x:%x:%x OK\r\n"
#define SSP_PASSKEY  "\r\nSSP PASSKEY %x:%x:%x:%x:%x:%x OK\r\n"
#define SSP_PASSKEY_PARSE  "%s %s %hhx %c %hhx %c %hhx %c %hhx %c %hhx %c %hhx %d"
#define SSP_PASSKEY_PARSE_NUM_ELEMENTS  15
#define SET_RESET         "SET RESET\r\n"  //sets and returns the factory settings of the module.

#define SYNTAX_ERROR      "SYNTAX ERROR\r\n"
#define NO_CARRIER        "NO CARRIER "
#define READY             "READY.\r\n"
#define READY_STRLEN      8
#define RING              "RING "
#define RING_RFCOMM       "RFCOMM \r\n"
#define RING_PARSE        "%s %n %x%c%x%c%x%c%x%c%x%c%x %n %s"
#define RING_CONNECTION_STRING_LEN   36

#define BT_MACADDR_NUM_BYTES  6
//#define BG_SIZE              1024
#define BG_SIZE               512
#define BG_SEND_SIZE          128
#define BG_SEND_SIZE_SMALL    50
#define SET_NAME_BUFF_SIZE    64
#define DEVICE_ID_BUFF_SIZE   48
#define SET_CONFIG_BUFF_SIZE  64
#define MSG_BUFF_SIZE         256
#define CHK_SETTINGS_BUFF_SZ  1024


//Delay times
#define TIMEOUT_RFD_CTS_HL         30
#define TIMEOUT_RCV_ECHO           30
#define TIMEOUT_RESET_CTS_LH       30
#define TIMEOUT_CONN_WAIT_FOREVER  0
#define MUX_ESC_CHAR_DELAY         1000
#define TIMEOUT_SEND_FRAME         1000

//Callback defines
#define RADIO_CB_ARRAY_LIMIT    3

//radio system defines
#define SYSTEM_DEFAULT_NAME "-Dongle--ID-"

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//radio setup and init control variables
bool bIs_Radio_setup = false;

//connection and data control variables
volatile bool bIs_connection = false;
bool     bDo_wait_forever_for_conn = false;

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

uint16_t  uiSend_frame_len = 0;

char      cRdo_setup_rcv_buff[BG_SIZE];

uint32_t  uiRemote_dev_key = 0;
uint8_t   uiSet_control_mux_hex_disable[] = {0xBF, 0xFF, 0x00, 0x11, 0x53, 0x45, 0x54, 0x20, 0x43, 0x4f, 0x4e, 0x54, 0x52, 0x4f, 0x4c, 0x20, 0x4d, 0x55, 0x58, 0x20, 0x30, 0x00};
uint8_t   uiIneedmd_radio_type = INEEDMD_PLATFORM_RADIO_TYPE;

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

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

typedef enum RADIO_MESSAGE_ID
{
  RADIO_MSG_UNKNOWN,
  RADIO_MSG_RADIO_CHANGE_SETTINGS,   //RADIO_REQUEST_CHANGE_SETTINGS
  RADIO_MSG_SEND,                    //RADIO_REQUEST_SEND_FRAME
  RADIO_MSG_RCV_RADIO_MESSAGE,        //RADIO_REQUEST_RECEIVE_RADIO_FRAME
  RADIO_MSG_RCV_PROTOCOL_FRAME,      //RADIO_REQUEST_RECEIVE_PROTOCOL_FRAME
  RADIO_MSG_FINISH_RCV_PROTOCOL_FRAME,
  RADIO_MSG_RADIO_SETUP,
  RADIO_MSG_RADIO_PWR_DOWN,  //RADIO_REQUEST_POWER_OFF
  RADIO_MSG_RADIO_PWR_UP,    //RADIO_REQUEST_POWER_ON
  RADIO_MSG_WAIT_FOR_CONNECTION,  //RADIO_REQUEST_WAIT_FOR_CONNECTION
  RADIO_MSG_BREAK_CONNECTION,  //RADIO_REQUEST_BREAK_CONNECTION
  RADIO_MSG_EVENT_CARRIER_CONNECTED,  //RADIO_REQUEST_CARRIER_CONNECTD
  RADIO_MSG_EVENT_CARRIER_DISCONNECT, //RADIO_REQUEST_CARRIER_DISCONNECT
}RADIO_MESSAGE_ID;

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
typedef struct tRadio_message
{
  RADIO_MESSAGE_ID eMessage;
  eRadio_Settings eSetting;
  uint8_t uiBuff[MSG_BUFF_SIZE];
  uint32_t uiFrame_len;
  uint32_t uiTimeout;
}tRadio_message;

typedef struct tRadio_Activity_State
{
  //radio setup control variables
  bool bIs_Radio_setup;
  bool bIs_Radio_Permanetly_off;

  //radio USB connection control variables
  bool bIs_USB_Phys_conn;
  bool bIs_USB_Data_conn;

  //wireless connection and data control variables
  eRadio_connection_state eRadio_conn_state;
  bool     bDo_wait_forever_for_conn;
  bool     bIs_Radio_waiting_to_rcv;
  uint32_t  uiRemainder_frame_rcv_len;
  uint16_t uiSend_frame_len;
  bool     bDid_frame_send;
  bool     bIs_Interface_In_callback_mode;

  //radio address and settings
  uint8_t   uiBT_addr[6];                //BT module mac address
  char      cBT_addr_string[18];         //BT module mac address in string format
  uint8_t   uiRemote_dev_addr[6];        //remote BT module mac address
  char      cRemore_dev_addr_string[18]; //remote BT module mac address in string format
  uint16_t  uiSet_ctrl_config[4];        //The Radio's feature configuration
  uint32_t  uiRemote_dev_key;
  uint8_t   uiSet_control_mux_hex_disable[22];
  char      cRadio_Device_ID[48];
  uint8_t   uiIneedmd_radio_type;

}tRadio_Activity_State;

tRadio_Activity_State tRT_Activity_State =
{
    //radio setup control variables
    false,  //bool bIs_Radio_setup;
    false,  //bIs_Radio_Permanetly_off

    //radio USB connection control variables
    false, //bool bIs_USB_Phys_conn;
    false, //bool bIs_USB_Data_conn;

    //wireless connection and data control variables
    RADIO_CONN_UNKNOWN,//eRadio_connection_state eCurrent_radio_connection_state;
    false, //bool bDo_wait_forever_for_conn;
    false, //bool bIs_Radio_waiting_to_rcv;
    0x00000000, //uint32_t  uiRemainder_frame_rcv_len;
    0x0000,     //uint16_t  uiSend_frame_len;
    false, //bool  bDid_frame_send;
    false, //bool  bIs_Interface_In_callback_mode;

    //radio address and settings
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   //uint8_t   uiBT_addr[6]; //BT module mac address
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //char cBT_addr_string[18]; //BT module mac address in string format
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //uint8_t uiRemote_dev_addr[6]; //remote BT module mac address
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //char cRemore_dev_addr_string[18]; //remote BT module mac address in string format
    {0x0000,0x0000,0x0000,0x0000}, //uint16_t uiSet_ctrl_config[4]; //The Radio's feature configuration
    0x00000000, //uint32_t  uiRemote_dev_key;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //uint8_t   uiSet_control_mux_hex_disable[22];
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //char cRadio_Device_ID[48];
    INEEDMD_PLATFORM_RADIO_TYPE, //uint8_t   uiIneedmd_radio_type;
};

extern Mailbox_Handle tTIRTOS_Radio_mailbox;
extern Clock_Handle tRadio_conn_timeout_timer;
extern Clock_Handle tRadio_rcv_timeout;
extern UART_Handle sRadio_UART_handle;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

void (* vRadio_Buff_sent_callback          [RADIO_CB_ARRAY_LIMIT]) (uint32_t uiBytes_sent);
void (* vRadio_Buff_receive_callback       [RADIO_CB_ARRAY_LIMIT]) (uint8_t * pBuff, uint32_t uiRcvd_Buff_Len);
void (* vRadio_Change_setting_callback     [RADIO_CB_ARRAY_LIMIT]) (bool bRadio_Setting_Changed);
void (* vRadio_Connection_status_callback  [RADIO_CB_ARRAY_LIMIT]) (bool bRadio_Carrier_Connection, bool bRadio_Protocol_Connection);
void (* vRadio_Setup_state_callback        [RADIO_CB_ARRAY_LIMIT]) (bool bRadio_Ready, bool bRadio_On);

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eIneedMD_radio_setup        (void);//sets the radio and radio interface up for application specifications
ERROR_CODE eRadio_power                (bool power_up);//controls power on off to the radio from the uP
void       ineedmd_radio_reset         (void);//controls the radio reset pin to reset the radio
ERROR_CODE eIneedmd_radio_cmnd_mode    (bool bMode_Set);//controls the radio command mode pin
ERROR_CODE eBT_RDIO_mux_mode_disable   (void); //attempts to get radio out of mux mode
ERROR_CODE eSend_Radio_CMND            (char * cCmnd_buff,     uint16_t uiCmnd_Buff_size);
ERROR_CODE eIneedmd_parse_addr         (char * cString_buffer, uint8_t * uiAddr);//parses the returned radio hw address and saves it into the address pointer parameter
void       ineedmd_radio_send_string   (char *send_string, uint16_t uiBuff_size);//send an ascii string to the radio
ERROR_CODE eIneedmd_radio_send_frame   (uint8_t *send_frame, uint16_t uiFrame_size);//
ERROR_CODE iIneedmd_radio_rcv_boot_msg (char * cRcv_string,    uint16_t uiBuff_size);
ERROR_CODE eIneedmd_radio_rcv_string   (char *cRcv_string, uint16_t uiBuff_size, uint16_t * puiRcvd_str_len);
ERROR_CODE eIneedmd_radio_rcv_frame    (uint8_t *cRcv_frame, uint16_t uiFrame_len);
ERROR_CODE eIneedmd_radio_chk_settings (char * cRcv_string,    uint16_t uiBuff_size);
ERROR_CODE eIneedmd_radio_rcv_config   (char * cRcv_string,    uint16_t uiBuff_size, uint16_t * uiSetcfg_Param);
ERROR_CODE eIneedmd_radio_set_echo     (void);
ERROR_CODE eIneedmd_radio_set_name     (void);
ERROR_CODE eGet_Radio_Name             (char * pBuffer);
ERROR_CODE eIneedmd_radio_set_config   (void);
ERROR_CODE eIneedmd_radio_wait_for_conn(tRadio_message * ptMessage);
void       vRadio_CD_int_callback      (void);
void       vRadio_LOW_BATT_int_callback(void);
void       vRadio_conn_timout_int_service(void);
void       vRadio_rcv_timeout_service_INT(void);
void       vRadio_USB_conn_callback    (bool bIs_Phys_conn, bool bIs_Data_conn);
ERROR_CODE eSet_connection_state       (eRadio_connection_state eConn_state);
ERROR_CODE eSet_radio_setup_state      (eRadio_setup_state eState);
ERROR_CODE eInit_Radio_CB_Functions    (void);
ERROR_CODE eNotify_Buff_sent_CB_Functions             (uint32_t uiBytes_sent);
ERROR_CODE eNotify_Buff_receive_CB_Functions          (uint8_t * pBuff, uint32_t uiRcvd_Buff_Len);
ERROR_CODE eNotify_Radio_Changed_Settings_CB_Functions(bool bRadio_Setting_Changed);
ERROR_CODE eNotify_Radio_Connection_State_CB_Functions(bool bRadio_Carrier_Connection, bool bRadio_Protocol_Connection);
ERROR_CODE eNotify_Radio_Setup_State_CB_Functions     (bool bRadio_Ready, bool bRadio_On);
ERROR_CODE eRadio_Message_Params_Init  (tRadio_message * pParams);
ERROR_CODE eInit_Radio_CB_Functions    (void);
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
* name:  eIneedMD_radio_setup
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
*                                      - ER_NOT_READY: radio battery is low and radio cant init
******************************************************************************/
ERROR_CODE eIneedMD_radio_setup(void)
{
#define DEBUG_iIneedMD_radio_setup
#ifdef DEBUG_iIneedMD_radio_setup
  #define  vDEBUG_RDIO_SETUP  vDEBUG
#else
  #define vDEBUG_RDIO_SETUP(a)
#endif

  ERROR_CODE eEC = ER_OK;
  ERROR_CODE eEC_radio_in_mux_mode = ER_NO;
  char cSend_buff[BG_SEND_SIZE_SMALL];
//  char cRcv_buff[BG_SIZE];
  char * cRcv_buff = cRdo_setup_rcv_buff;
  char * cBaud_Change_Echo = NULL;
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
      eEC = eBSP_Set_radio_uart_to_callback();
      eSet_radio_setup_state(RDIO_SETUP_INTERFACE_ENABLED);

      //turn power on to the radio
      eSet_radio_setup_state(RDIO_SETUP_START_POWER);
      eRadio_power(true);
      eSet_radio_setup_state(RDIO_SETUP_FINISH_POWER);

      GPIO_setupCallbacks(&Board_gpioCallbacks2);
      GPIO_setupCallbacks(&Board_gpioCallbacks3);

      if(eEC == ER_OK)
      {
        //get the current system baud
        eBSP_Get_radio_uart_baud(&uiSystem_Baud);
      }

      //Check if the radio already has a carrier connection
      //
      eEC = eBSP_Get_Radio_CD_status();
      if(eEC == ER_SET)
      {
        //Radio already has a carrier connection and is pre-set
        //
        GPIO_enableInt(EK_TM4C123GXL_RADIO_CD, GPIO_INT_FALLING);

        //set low the gpio pin to put the radio in data mode
        eIneedmd_radio_cmnd_mode(false);

        eEC = ER_DONE;
      }
      else
      {
        GPIO_enableInt(EK_TM4C123GXL_RADIO_CD, GPIO_INT_RISING);
        eEC = ER_OK;
      }

      eEC = eBSP_Get_Radio_low_bat_status();
      if(eEC == ER_SET)
      {
        //Radio already has a carrier connection and is pre-set
        //
        GPIO_enableInt(EK_TM4C123GXL_RADIO_LOW_BATT, GPIO_INT_FALLING);

        //set the error code to not ready to signify low bat
        eEC = ER_NOT_READY;
      }
      else
      {
        GPIO_enableInt(EK_TM4C123GXL_RADIO_LOW_BATT, GPIO_INT_RISING);
        eEC = ER_OK;
      }

      if(eEC == ER_OK)
      {
        //todo: perform a DTR pin set and check if radio responsive to commands, will be released in board rev 5
        eIneedmd_radio_cmnd_mode(true);

        //software reset the radio
        //
        vDEBUG_RDIO_SETUP("Rdio setup, radio software reset");
        eSet_radio_setup_state(RDIO_SETUP_START_SOFT_RESET);

        ineedmd_radio_reset();
        eEC = eBSP_Get_Radio_CTS_status();
        while(eEC == ER_FALSE)
        {
          Task_sleep(100);
          eEC = eBSP_Get_Radio_CTS_status();
          if(eEC == ER_TRUE)
          {
            eBSP_Radio_clear_rcv_buffer();
            eSet_radio_setup_state(RDIO_SETUP_FINISH_SOFT_RESET);
            break;
          }else{/*do nothing*/}
        }
        vDEBUG_ASSERT("Rdio setup soft reset fail",(RDIO_SETUP_FINISH_SOFT_RESET == eGet_radio_setup_state()));

        //attempt to get the radio out of mux mode if it is in it
        if(eEC_radio_in_mux_mode == ER_YES)
        {
          eBT_RDIO_mux_mode_disable();
        }else{/*do nothing*/}

        //Check the radio setings to see if it was already set for the application
        eEC = eIneedmd_radio_chk_settings(NULL, 0);
        if(eEC == ER_OK)
        {
          //radio is already pre-set
          eEC = ER_DONE;
          eIneedmd_radio_cmnd_mode(false);
          vDEBUG_RDIO_SETUP("Rdio setup, radio is pre-set");
        }
        else
        {
          //SET RESET, reset to factory defaults
          //
          memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
          memcpy(cSend_buff, SET_RESET, strlen(SET_RESET));
          eSet_radio_setup_state(RDIO_SETUP_START_RFD);
          eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(SET_RESET));
          //Wait for the radio CTS pin to go low signifying reset
          eEC = eBSP_Get_Radio_CTS_status();
          i_delay = 0;
          while(eEC == ER_TRUE)
          {
            //delay for 100ms
            eOSAL_delay(100, NULL);
            i_delay += 1;
            //check if the timeout was reached
            if(i_delay >= TIMEOUT_RFD_CTS_HL)
            {
              //time out was reached, the radio did not go into RFD, prep for another baud change RFD loop
              vDEBUG_RDIO_SETUP("Rdio setup, radio RFD CTS HL monitoring timed out");
              eEC = ER_TIMEOUT;
              break;
            }else{/*do nothing*/}

            //get the cts status
            eEC = eBSP_Get_Radio_CTS_status();
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
            eEC = eBSP_Get_Radio_CTS_status();
            while(eEC == ER_FALSE)
            {
              Task_sleep(100);
              eEC = eBSP_Get_Radio_CTS_status();
            }

            vDEBUG_RDIO_SETUP("Rdio setup, radio RFD CTS transitioned LH");

            //check if the radio is back from RFD
            if(eEC == ER_TRUE)
            {
              //radio came back from RFD, preparing to receive the boot message
              memset(cRcv_buff, 0x00, BG_SIZE);
              eEC = iIneedmd_radio_rcv_boot_msg(cRcv_buff, BG_SIZE);
              vDEBUG_ASSERT("Rdio setup SYS HALT, rcv boot msg fail",(eEC == ER_VALID));
              if(eEC == ER_VALID)
              {
                vRADIO_ECHO(cRcv_buff);

                //set the baud rate to system required baud speed
                memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
                snprintf(cSend_buff, BG_SIZE, SET_CONTROL_BAUD, INEEDMD_RADIO_UART_BAUD, SET_CONTROL_BAUD_PARITY, SET_CONTROL_BAUD_STOP_BITS_1);
                eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(cSend_buff));
                while(1)
                {
                  eOSAL_delay(1, NULL);
                  if(tRT_Activity_State.bDid_frame_send == true)
                  {
                    eEC = ER_OK;
                    break;
                  }
                }
                vDEBUG_ASSERT("Rdio setup, radio set baud command send fail", (eEC == ER_OK));

                //recieve the radio's baud change command echo
                memset(cRcv_buff, 0x00, BG_SIZE);
                eEC = eBSP_Radio_rcv_frame((uint8_t *)cRcv_buff, BG_SIZE);
                cBaud_Change_Echo = NULL;
                i_delay = 0;
                while(cBaud_Change_Echo == NULL)
                {
                  cBaud_Change_Echo = strstr(cRcv_buff, cSend_buff);
                  if(cBaud_Change_Echo == NULL)
                  {
                    eOSAL_delay(100, NULL);
                    i_delay++;
                    vDEBUG_ASSERT("Rdio setup, radio resp to set baud fail", (i_delay < (TIMEOUT_RCV_ECHO)));
                  }
                  else
                  {
                    vRADIO_ECHO(cRcv_buff);
                    break;
                  }
                }
                eBSP_Radio_rcv_frame_halt();

                vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL BAUD, uart baud set");

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
                vDEBUG_ASSERT("Rdio setup, Baud selection failure", 0);
              }

              //set the baud rate
              eBSP_Set_radio_uart_baud(uiBaud_to_set);

              //soft reset the radio
              i_delay = 0;
              ineedmd_radio_reset();
              eEC = eBSP_Get_Radio_CTS_status();
              while(eEC == ER_FALSE)
              {
                eOSAL_delay(100, NULL);
                i_delay++;
                vDEBUG_ASSERT("Rdio setup, radio failed an assured soft reset", (i_delay < TIMEOUT_RFD_CTS_HL));

                eEC = eBSP_Get_Radio_CTS_status();
                if(eEC == ER_TRUE)
                {
                  vDEBUG_RDIO_SETUP("Rdio setup, radio was soft reset");
                  break;
                }
              }

              eBSP_Radio_clear_rcv_buffer();

              //send the reset command
              memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
              memcpy(cSend_buff, SET_RESET, strlen(SET_RESET));
              UART_writeCancel(sRadio_UART_handle);
              eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(SET_RESET));

              //wait for the radio to go into RFD by monitoring CTS pin transition from high to low
              i_delay = 0;
              eEC = eBSP_Get_Radio_CTS_status();
              while(eEC == ER_TRUE)
              {
                //delay for 100ms
                eOSAL_delay(100, NULL);
                i_delay++;
                //check if the timeout was reached
                if(i_delay >= TIMEOUT_RFD_CTS_HL)
                {
                  //time out was reached, the radio did not go into RFD, prep for another baud change RFD loop
                  vDEBUG_RDIO_SETUP("Rdio setup, radio RFD CTS HL monitoring timed out");
                  eEC = ER_TIMEOUT;
                  break;
                }else{/*do nothing*/}

                eEC = eBSP_Get_Radio_CTS_status();
                if(eEC == ER_FALSE)
                {
                  break;
                }
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
                i_delay = 0;
                eEC = eBSP_Get_Radio_CTS_status();
                while(eEC == ER_FALSE)
                {
                  //delay for 100ms
                  eOSAL_delay(1, NULL);
                  i_delay++;
                  vDEBUG_ASSERT("Rdio setup, radio failed to come back from RFD in timeout period", (i_delay < (TIMEOUT_RFD_CTS_HL * 100)));

                  //radio is still resetting, keep polling the CTS status until it goes high
                  eEC = eBSP_Get_Radio_CTS_status();
                  if(eEC == ER_TRUE)
                  {
                    break;
                  }
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
                  memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
                  memset(cRcv_buff, 0x00, BG_SIZE);
                  snprintf(cSend_buff, BG_SEND_SIZE_SMALL, SET_CONTROL_BAUD, INEEDMD_RADIO_UART_BAUD, SET_CONTROL_BAUD_PARITY, SET_CONTROL_BAUD_STOP_BITS_1);
                  eEC = eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(cSend_buff));
                  while(1)
                  {
                    eOSAL_delay(1, NULL);
                    if(tRT_Activity_State.bDid_frame_send == true)
                    {
                      eEC = ER_OK;
                      break;
                    }
                  }
                  vDEBUG_ASSERT("Rdio setup, radio set baud command send fail", (eEC == ER_OK));

                  //recieve the radio's baud change command echo
                  eEC = eBSP_Radio_rcv_frame((uint8_t *)cRcv_buff, BG_SIZE);
                  cBaud_Change_Echo = NULL;
                  i_delay = 0;
                  while(cBaud_Change_Echo == NULL)
                  {
                    cBaud_Change_Echo = strstr(cRcv_buff, cSend_buff);
                    if(cBaud_Change_Echo == NULL)
                    {
                      eOSAL_delay(100, NULL);
                      i_delay++;
                      vDEBUG_ASSERT("Rdio setup, radio resp to set baud fail", (i_delay < (TIMEOUT_RCV_ECHO)));
                    }
                    else
                    {
                      vRADIO_ECHO(cRcv_buff);
                      break;
                    }
                  }
                  eBSP_Radio_rcv_frame_halt();

                  vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL BAUD, uart baud set");

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
                  eBSP_Radio_clear_rcv_buffer();
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
          eEC = eIneedmd_radio_set_echo();

          //SET BT AUTH, tell the radio what auth method we are using
          //
          memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
          snprintf(cSend_buff, BG_SEND_SIZE_SMALL, SET_BT_AUTH, SET_BT_AUTH_MODE, SET_BT_AUTH_PIN_CODE);
          eEC = eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(cSend_buff));
          if(eEC == ER_OK)
          {
            vDEBUG_RDIO_SETUP("Rdio setup, SET BT AUTH, set the BT auth method");
            vRADIO_ECHO(cSend_buff);
          }else{/*do nothing*/}

          //SET BT SSP, tell the radio we are using BT SSP pairing
          //
          memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
          snprintf(cSend_buff, BG_SEND_SIZE_SMALL, SET_BT_SSP, SET_BT_SSP_CPLTES, SET_BT_SSP_MITM);
          eEC = eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(cSend_buff));
          if(eEC == ER_OK)
          {
            vDEBUG_RDIO_SETUP("Rdio setup, SET BT SSP, set the SSP pairing");
            vRADIO_ECHO(cSend_buff);
          }else{/*do nothing*/}

          //SET PROFILE SPP, tells the radio we are using SPP protocol
          //
          memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
          snprintf(cSend_buff, BG_SEND_SIZE_SMALL, SET_PROFILE_SPP, SET_PROFILE_SPP_PARAM);
          eEC = eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(cSend_buff));
          if(eEC == ER_OK)
          {
            vDEBUG_RDIO_SETUP("Rdio setup, SET PROFILE SPP, set the SPP protocol");
            vRADIO_ECHO(cSend_buff);
          }else{/*do nothing*/}

          // SET CONTROL BATTERY sets the battery mode for the radio,
          // configures the, low bat warning voltage, the low voltage lock out,  the charge release voltage
          // and that this PIO pin to signal low batt is PIO 01
          //
          memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
          snprintf(cSend_buff, BG_SEND_SIZE_SMALL, SET_CONTROL_BATT, SET_CONTROL_BATT_LOW, SET_CONTROL_BATT_SHTDWN, SET_CONTROL_BATT_FULL, SET_CONTROL_BATT_MASK);
          eEC = eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(cSend_buff));
          if(eEC == ER_OK)
          {
            vDEBUG_RDIO_SETUP("Rdio setup, SET_CONTROL_BATT");
            vRADIO_ECHO(cSend_buff);
          }else{/*do nothing*/}

          eEC = eIneedmd_radio_set_name();

          //SET BT PAIRCOUNT, Set the max number of pairings
          //
          memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
          snprintf(cSend_buff, BG_SEND_SIZE_SMALL, SET_BT_PAIRCOUNT, BT_PAIRCOUNT);
          eEC = eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(cSend_buff));
          if(eEC == ER_OK)
          {
            vDEBUG_RDIO_SETUP("Rdio setup, SET BT PAIRCOUNT, set max number of pairings");
            vRADIO_ECHO(cSend_buff);
          }else{/*do nothing*/}

          //SET CONTROL CD, set CD to a PIO pin
          //
          memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
          snprintf(cSend_buff, BG_SEND_SIZE_SMALL, SET_CONTROL_CD, SET_CONTROL_CD_MASK, SET_CONTROL_CD_MODE);
          eEC = eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(cSend_buff));
          if(eEC == ER_OK)
          {
            vDEBUG_RDIO_SETUP("Rdio setup, SET PIO, set the PIO dir");
            vRADIO_ECHO(cSend_buff);
          }else{/*do nothing*/}

          //SET CONTROL BIND, bind commands to PIO
          //
          memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
          snprintf(cSend_buff, BG_SEND_SIZE_SMALL, SET_CONTROL_BIND, SET_CONTROL_BIND_PRIORITY, SET_CONTROL_BIND_IO_MASK, SET_CONTROL_BIND_DIRECTION, SET_CONTROL_COMMAND);
          eEC = eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(cSend_buff));
          if(eEC == ER_OK)
          {
            vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL BIND, set the PIO bind");
            vRADIO_ECHO(cSend_buff);
          }else{/*do nothing*/}

          //SET CONTROL VREGEN, use the VREG_ENA pin to turn radio power on or off
          //
          memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
          snprintf(cSend_buff, BG_SEND_SIZE_SMALL, SET_CONTROL_VREGEN, SET_CONTROL_VREGE_MODE, SET_CONTROL_VREGEN_PIOMASK);
          eEC = eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(cSend_buff));
          if(eEC == ER_OK)
          {
            vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL VREGEN, use the VREG_ENA pin");
            vRADIO_ECHO(cSend_buff);
          }else{/*do nothing*/}

          eIneedmd_radio_set_config();

          //SET CONTROL ESCAPE, set the escape mode
          //
          memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
          snprintf(cSend_buff, BG_SIZE, SET_CONTROL_ESCAPE, SET_CONTROL_ESCAPE_ESC_CHAR, SET_CONTROL_ESCAPE_DTR_MASK, SET_CONTROL_ESCAPE_DTR_MODE);
          eEC = eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(cSend_buff));
          if(eEC == ER_OK)
          {
            vDEBUG_RDIO_SETUP("Rdio setup, SET CONTROL ESCAPE, set the escape mode");
            vRADIO_ECHO(cSend_buff);
          }else{/*do nothing*/}

          //RESET, reset the radio to make the settings take hold
          //
          memset(cSend_buff, 0x00, BG_SEND_SIZE_SMALL);
          memcpy(cSend_buff, RESET, strlen(RESET));
          eEC = eIneedmd_radio_send_frame((uint8_t *)cSend_buff, strlen(cSend_buff));
          if(eEC == ER_OK)
          {
            vDEBUG_RDIO_SETUP("Rdio setup, RESET, perform software reset");
            vRADIO_ECHO(RESET);
          }else{/*do nothing*/}

          vDEBUG_RDIO_SETUP("Rdio setup, waiting for the radio to come back from reset");
          eEC = eBSP_Get_Radio_CTS_status();
          while(eEC == ER_FALSE)
          {
            eOSAL_delay(100, NULL);
            i_delay++;
            vDEBUG_ASSERT("Rdio setup, radio assured reset fail", (i_delay < (TIMEOUT_RESET_CTS_LH)));
            eEC = eBSP_Get_Radio_CTS_status();
          }

          //set the connection status to non while waiting for the command to wait for a connection
          eSet_connection_state(RADIO_CONN_NONE);

          //set low the gpio pin to put the radio in command mode
          eIneedmd_radio_cmnd_mode(false);

          vDEBUG_RDIO_SETUP("Rdio setup, Radio ready");
          //Set the error code to done
          eEC = ER_DONE;
          eSet_radio_setup_state(RDIO_SETUP_READY);
        }
      }
    }
    else
    {
      //the radio to setup is unknown
      //
      //set the error code to signify unknown interface
      eEC = ER_UNKNOWN_INTERFACE;

      vDEBUG_ASSERT("Rdio setup, additional platforms are not defined",0);
    }
  }
  else
  {
    //Radio was already set up
    //
    //Set the error code to done
    eEC = ER_DONE;
  }

  vDEBUG_ASSERT("Radio setup, invalid error code to return", ((eEC == ER_OK)   |  (eEC == ER_TIMEOUT) | \
                                                              (eEC == ER_FAIL) |  (eEC == ER_DONE)    |\
                                                              (eEC == ER_UNKNOWN_INTERFACE)           |\
                                                              (eEC == ER_NOT_READY)));
  return eEC;
#undef vDEBUG_RDIO_SETUP
}

/******************************************************************************
* name: eRadio_power
* description: This function controls the power enable setting for the radio.
* !!!NOTE!!! In Board Revision 0.5 the radio power controller is used to charge the
* battery. So, when USB is plugged in the radio will continue to stay on even with
* the power enable pin set to off.
* param description: bool - true: will perform radio power up
*                    bool - false: will perform radio power down
* return value description:  ERROR_CODE eEC - ER_ON:
*                                           - ER_OFF: radio power enable pin is set to off. See NOTE!
******************************************************************************/
ERROR_CODE eRadio_power(bool power_up)
{
  ERROR_CODE eEC = ER_FAIL;
  int i;
  if (power_up == true)
  {
    //power on the radio
    eBSP_Radio_Power_On();

    eEC = eBSP_Get_Radio_CTS_status();
    for(i = 0; (eEC == ER_FALSE) & (i < 100); i++)
    {
      Task_sleep(100);
      eEC = eBSP_Get_Radio_CTS_status();
      if(eEC == ER_TRUE)
      {
        break;
      }
    }
    vDEBUG_ASSERT("eRadio_power power on failure", eEC == ER_TRUE);

    eEC = ER_ON;
  }
  else
  {
    //power off the radio
    eBSP_Radio_Power_Off();

    eEC = ER_OFF;

  }

  return eEC;
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
    eEC = eBSP_Set_radio_to_cmnd_mode();
  }
  else if(bMode_Set == false)
  {
    eEC = eBSP_Set_radio_to_data_mode();
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
  eIneedmd_radio_send_frame(uiSet_control_mux_hex_disable, 23);

  while(true)
  {
    if(i == READY_STRLEN)
    {
      eEC = ER_BUFF_SIZE;
      break;
    }else{/*do nothing*/}

    eEC = eBSP_Radio_rcv_char(&cMux_rcv_buff[i]);

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
  eBSP_Radio_clear_rcv_buffer();
  memset(cMux_rcv_buff,0x00, READY_STRLEN);
  i = 0;

  if(eEC != ER_VALID)
  {
    eOSAL_delay(MUX_ESC_CHAR_DELAY, NULL);
    eBSP_Radio_send_byte((uint8_t *)&cEsc_char);
    eBSP_Radio_send_byte((uint8_t *)&cEsc_char);
    eBSP_Radio_send_byte((uint8_t *)&cEsc_char);
    eOSAL_delay(MUX_ESC_CHAR_DELAY, NULL);

    while(true)
    {
      if(i == READY_STRLEN)
      {
        eEC = ER_BUFF_SIZE;
        break;
      }else{/*do nothing*/}

      eEC = eBSP_Radio_rcv_char(&cMux_rcv_buff[i]);

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
  eBSP_Radio_clear_rcv_buffer();

  return eEC;
}

/*
 * parses a received string for a BT address
 */
ERROR_CODE eIneedmd_parse_addr(char * cString_buffer, uint8_t * uiAddr)
{
  ERROR_CODE eEC = ER_FAIL;
  char cThrow_away[10];
  memset(cThrow_away, 0x00, 10);
  int iError;

  //                                                     %s           %s           %s           %x        %c           %x        %c           %x        %c           %x        %c           %x        %c           %x
  iError = sscanf ( cString_buffer, SET_BT_BDADDR_PARSE, cThrow_away, cThrow_away, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr++, cThrow_away, uiAddr);

  //check if the raw BT address string from the modem is in the correct format
  if(iError != 14)  //todo: remove this magic number
  {
    eEC = ER_FAIL;
  }
  else
  {
    eEC = ER_OK;
  }

  return eEC;
}


/*
 * send a raw hex message to the radio
 * This function in it's current state just passes the parameters to the HAL.
 * It is meant to be here for future api, rtos, parsing use.
 */
ERROR_CODE eIneedmd_radio_send_frame(uint8_t *send_frame, uint16_t uiFrame_size)
{
//#define DEBUG_eIneedmd_radio_send_frame
#ifdef DEBUG_eIneedmd_radio_send_frame
  #define  vDEBUG_RDIO_SND_FR  vDEBUG
#else
  #define vDEBUG_RDIO_SND_FR(a)
#endif

  int i;
  ERROR_CODE eEC = ER_FAIL;
  tRT_Activity_State.uiSend_frame_len = uiFrame_size;

  tRT_Activity_State.bDid_frame_send = false;

  eBSP_Radio_send_frame(send_frame, uiFrame_size);

  i = 0;
  while(tRT_Activity_State.bDid_frame_send == false)
  {
    eOSAL_delay(1, NULL);
    i++;
    if(i >=TIMEOUT_SEND_FRAME)
    {
      vDEBUG_ASSERT("vIneedMD_radio_task, RADIO_MSG_SEND timeout", 0);
      eEC = ER_FAIL;
      eBSP_Radio_send_frame_halt();
      break;
    }
  }

  return eEC;
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
  uint8_t  i_timeout = 0;
  ERROR_CODE eEC = ER_FAIL;
  char * cBoot_string = NULL;


  eBSP_Radio_rcv_frame((uint8_t *)cRcv_string, uiBuff_size);
  eOSAL_delay(100, NULL);

  while(true)
  {

    cBoot_string = strstr(cRcv_string, READY);
    if(cBoot_string != NULL)
    {
      vDEBUG_RDIO_RCV_BOOTMSG("Rdo boot msg, RFD dev rdy");
      eEC = ER_VALID;
      eBSP_Radio_rcv_frame_halt();
      break;
    }
    else
    {
      eOSAL_delay(100, NULL);
      i_timeout++;
      if(i_timeout >= TIMEOUT_RFD_CTS_HL)
      {
        eEC = ER_TIMEOUT;
        eBSP_Radio_rcv_frame_halt();
        break;
      }
    }
  }

  vDEBUG_ASSERT("Rdio setup, rcv boot msg fail",(eEC == ER_VALID));

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
  char * pRcv_buff = NULL;
  uint16_t uiRcvd_len = 0;

  //Zero out the number or received characters
  if(puiRcvd_str_len != NULL)
  {
    *puiRcvd_str_len = 0;
  }else{/*do nothing*/}

  eEC = ER_TRUE;
  if(eEC == ER_TRUE)
  {
    pRcv_buff = calloc(uiBuff_size, sizeof(char));
    eBSP_Radio_rcv_string(pRcv_buff, uiBuff_size);
    while(uiRcvd_len < uiBuff_size)
    {
      uiRcvd_len = strlen(pRcv_buff);
      if(pRcv_buff[uiRcvd_len - 1] == '\n')
      {
        break;
      }
    }
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

      eEC = eBSP_Radio_rcv_char(&cRcv_string[i]);

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

ERROR_CODE eIneedmd_radio_rcv_frame(uint8_t *cRcv_frame, uint16_t uiFrame_len)
{
  ERROR_CODE eEC = ER_FAIL;

  eEC = eBSP_Radio_rcv_frame(cRcv_frame, uiFrame_len);

  if(eEC == ER_TIMEOUT)
  {
    //in callback mode, the received bytes will be 0.
    if(tRT_Activity_State.bIs_Interface_In_callback_mode == true)
    {
      eEC = ER_OK;
    }
    else
    {
      eEC = ER_TIMEOUT;
    }
  }

  eBSP_Radio_rcv_frame_halt();

  return eEC;
}

/*
 * Check the settings of the radio
 */
ERROR_CODE eIneedmd_radio_chk_settings (char * cRcv_string,  uint16_t uiBuff_size)
{
#define DEBUG_iIneedmd_radio_rcv_settings
#ifdef DEBUG_iIneedmd_radio_rcv_settings
  #define  vDEBUG_RDIO_RCV_SET  vDEBUG
#else
  #define vDEBUG_RDIO_RCV_SET(a)
#endif

  ERROR_CODE eEC = ER_OK;
  ERROR_CODE eEC_set_bt = ER_FAIL;
  ERROR_CODE eEC_set_cntrl = ER_FAIL;
  char * cBT_Rcvd_Settings = NULL;
  eEC = eUsing_radio_uart_dma();
  char * cSetting = NULL;
  char cBT_setting[64];
  int i = 0;
  typedef enum
  {
    CHK_SET_NONE = 0,
    CHK_SET_ECHO,
    CHK_SET_ADDR,
    CHK_SET_NAME,
    CHK_SET_AUTH,
    CHK_SET_SSP,
    CHK_SET_END_OF_SET_BT,
    CHK_SET_BIND,
    CHK_SET_CD,
    CHK_SET_CONFIG,
    CHK_SET_ESCAPE,
    CHK_SET_VREGEN,
    CHK_SET_BATT,
    CHK_SET_END_OF_SET_CONTROL,

  }CHK_SET;
  CHK_SET eSettings_Check_Track = CHK_SET_NONE;

  cBT_Rcvd_Settings = calloc(CHK_SETTINGS_BUFF_SZ, sizeof(char));
  while(cBT_Rcvd_Settings == NULL)
  {
    cBT_Rcvd_Settings = calloc(CHK_SETTINGS_BUFF_SZ, sizeof(char));
  }

  memset(cBT_setting, 0x00, 64);
  memcpy(cBT_setting, SET_BT_GET, strlen(SET_BT_GET));

  eIneedmd_radio_send_frame((uint8_t *)cBT_setting, strlen(SET_BT_GET));

  eBSP_Radio_rcv_frame((uint8_t *)cBT_Rcvd_Settings, CHK_SETTINGS_BUFF_SZ);

  for(i = 0; i<10; i++)
  {
    if(strstr(cBT_Rcvd_Settings, ENDOF_SET_SETTINGS) != NULL)
    {
      eEC = ER_OK;
      break;
    }
    else
    {
      Task_sleep(100);
      eEC = ER_FAIL;
    }
  }
  eBSP_Radio_rcv_frame_halt();

  //check if the command was echoed back
  //
  if(eEC == ER_OK)
  {
    cSetting = strstr(cBT_Rcvd_Settings, SET_BT_GET);
    if(cSetting == NULL)
    {
      eEC = ER_OK;
      eSettings_Check_Track++;
      if(eSettings_Check_Track != CHK_SET_ECHO)
      {
        eEC = ER_FAIL;
      }
    }
    else
    {
      eEC = ER_FAIL;
      vDEBUG("eIneedmd_radio_chk_settings, echo on fail");
    }
  }

  //check the BD addr and save the last two bytes to verify the name
  //
  if(eEC == ER_OK)
  {
    cSetting = strstr(cBT_Rcvd_Settings, SET_BT_BDADDR_CHK);
    if(cSetting == NULL)
    {
      eEC = ER_FAIL;
      vDEBUG_RDIO_RCV_SET("Radio set check, address rcv fail");
    }
    else
    {
      eEC = eIneedmd_parse_addr(cBT_Rcvd_Settings, uiBT_addr);
      if(eEC != ER_OK)
      {
        eEC = ER_FAIL;
        vDEBUG_RDIO_RCV_SET("Radio set check, address parse fail");
        vDEBUG_RDIO_RCV_SET(cBT_Rcvd_Settings);
      }
      else
      {
        eSettings_Check_Track++;
        if(eSettings_Check_Track != CHK_SET_ADDR)
        {
          eEC = ER_FAIL;
        }
      }
    }
  }

  //check the BT name
  //
  if(eEC == ER_OK)
  {
    cSetting = strstr(cBT_Rcvd_Settings, SET_BT_NAME_HDR);
    if(cSetting == NULL)
    {
      eEC = ER_FAIL;
      vDEBUG_RDIO_RCV_SET("Radio set check, name rcv fail");
    }
    else
    {
      memset(cBT_setting, 0x00, 64);
    	read_dongle_ID(cBT_setting);
      cSetting = strstr(cBT_Rcvd_Settings, cBT_setting);
      if(cSetting == NULL)
      {
        eEC = ER_FAIL;
        vDEBUG("Radio set check, name rcv fail");
      }
      else
      {
        eEC = ER_OK;
        eSettings_Check_Track++;
        if(eSettings_Check_Track != CHK_SET_NAME)
        {
          eEC = ER_FAIL;
        }
      }
    }
  }

  //check the BT authentication
  //
  if(eEC == ER_OK)
  {

    snprintf(cBT_setting, 64, SET_BT_AUTH, SET_BT_AUTH_MODE, SET_BT_AUTH_PIN_CODE);
    cSetting = strstr(cBT_Rcvd_Settings, cBT_setting);
    if(cSetting == NULL)
    {
      eEC = ER_FAIL;
      vDEBUG_RDIO_RCV_SET("Radio set check, auth rcv fail");
    }
    else
    {
      eEC = ER_OK;
      eSettings_Check_Track++;
      if(eSettings_Check_Track != CHK_SET_AUTH)
      {
        eEC = ER_FAIL;
      }
    }
  }

  //Check the SSP
  //
  if(eEC == ER_OK)
  {
    snprintf(cBT_setting, 64, SET_BT_SSP, SET_BT_SSP_CPLTES, SET_BT_SSP_MITM);
    cSetting = strstr(cBT_Rcvd_Settings, cBT_setting);
    if(cSetting == NULL)
    {
      eEC = ER_FAIL;
      vDEBUG_RDIO_RCV_SET("Radio set check, ssp rcv fail");
    }
    else
    {
      eEC = ER_OK;
      eSettings_Check_Track++;
      if(eSettings_Check_Track != CHK_SET_SSP)
      {
        eEC = ER_FAIL;
      }
    }
  }

  //check if the set bt settings passed
  if(eEC == ER_FAIL)
  {
    eEC_set_bt = ER_FAIL;
  }
  else
  {
    //the SET BT settings were correct
    //
    eEC = ER_OK;
    eEC_set_bt = ER_OK;
    eSettings_Check_Track++;
    if(eSettings_Check_Track != CHK_SET_END_OF_SET_BT)
    {
      eEC = ER_FAIL;
    }
  }

  //Check if the SET BT settings check succeded
  if(eEC_set_bt == ER_OK)
  {
    //SET BT settings were good
    //
    memset(cBT_Rcvd_Settings, 0x00, CHK_SETTINGS_BUFF_SZ);
    memset(cBT_setting, 0x00, 64);
    memcpy(cBT_setting, SET_CONTROL_GET, strlen(SET_CONTROL_GET));

    eIneedmd_radio_send_frame((uint8_t *)cBT_setting, strlen(SET_CONTROL_GET));

    eBSP_Radio_rcv_frame((uint8_t *)cBT_Rcvd_Settings, CHK_SETTINGS_BUFF_SZ);

    for(i = 0; i<30; i++)
    {
      if(strstr(cBT_Rcvd_Settings, ENDOF_SET_SETTINGS) != NULL)
      {
        eEC = ER_OK;
        break;
      }
      else
      {
        Task_sleep(100);
        eEC = ER_FAIL;
      }
    }
    eBSP_Radio_rcv_frame_halt();

    //Check the Control bind
    //
    if(eEC == ER_OK)
    {
      snprintf(cBT_setting, 64, SET_CONTROL_BIND, SET_CONTROL_BIND_PRIORITY, SET_CONTROL_BIND_IO_MASK, SET_CONTROL_BIND_DIRECTION, SET_CONTROL_COMMAND);

      cSetting = strstr(cBT_Rcvd_Settings, cBT_setting);
      if(cSetting == NULL)
      {
        eEC = ER_FAIL;
        vDEBUG_RDIO_RCV_SET("Radio set check, bind rcv fail");
      }
      else
      {
        eEC = ER_OK;
        eSettings_Check_Track++;
        if(eSettings_Check_Track != CHK_SET_BIND)
        {
          eEC = ER_FAIL;
        }
      }
    }

    //Check the Control CD
    //
    if(eEC == ER_OK)
    {
      memset(cBT_setting, 0x00, 64);
      snprintf(cBT_setting, 64, SET_CONTROL_CD, SET_CONTROL_CD_MASK, SET_CONTROL_CD_MODE);
      cSetting = strstr(cBT_Rcvd_Settings, cBT_setting);
      if(cSetting == NULL)
      {
        eEC = ER_FAIL;
        vDEBUG("Radio set check, cd rcv fail");
      }
      else
      {
        eEC = ER_OK;
        eSettings_Check_Track++;
        if(eSettings_Check_Track != CHK_SET_CD)
        {
          eEC = ER_FAIL;
        }
      }
    }

    //Check the Control CONFIG
    //
    if(eEC == ER_OK)
    {
      memset(cBT_setting, 0x00, 64);
      snprintf(cBT_setting, 64, SET_CONTROL_CONFIG, 0x0000, 0x0000, 0x400, 0x1120);
      cSetting = strstr(cBT_Rcvd_Settings, cBT_setting);
      if(cSetting == NULL)
      {
        eEC = ER_FAIL;
        vDEBUG_RDIO_RCV_SET("Radio set check, config rcv fail");
      }
      else
      {
        eEC = ER_OK;
        eSettings_Check_Track++;
        if(eSettings_Check_Track != CHK_SET_CONFIG)
        {
          eEC = ER_FAIL;
        }
      }
    }

    //Check the Control ESCAPE
    //
    if(eEC == ER_OK)
    {
      memset(cBT_setting, 0x00, 64);
      snprintf(cBT_setting, 64, SET_CONTROL_ESCAPE, SET_CONTROL_ESCAPE_ESC_CHAR, SET_CONTROL_ESCAPE_DTR_MASK, SET_CONTROL_ESCAPE_DTR_MODE);
      cSetting = strstr(cBT_Rcvd_Settings, cBT_setting);
      if(cSetting == NULL)
      {
        eEC = ER_FAIL;
        vDEBUG_RDIO_RCV_SET("Radio set check, esc rcv fail");
      }
      else
      {
        eEC = ER_OK;
        eSettings_Check_Track++;
        if(eSettings_Check_Track != CHK_SET_ESCAPE)
        {
          eEC = ER_FAIL;
        }
      }
    }

    //Check the Control VREGEN
    //
    if(eEC == ER_OK)
    {
      memset(cBT_setting, 0x00, 64);
      snprintf(cBT_setting, 64, SET_CONTROL_VREGEN, SET_CONTROL_VREGE_MODE, SET_CONTROL_VREGEN_PIOMASK);
      cSetting = strstr(cBT_Rcvd_Settings, cBT_setting);
      if(cSetting == NULL)
      {
        eEC = ER_FAIL;
        vDEBUG_RDIO_RCV_SET("Radio set check, vregen rcv fail");
      }
      else
      {
        eEC = ER_OK;
        eSettings_Check_Track++;
        if(eSettings_Check_Track != CHK_SET_VREGEN)
        {
          eEC = ER_FAIL;
        }
      }
    }

    //Check the Control BATT
    //
    if(eEC == ER_OK)
    {
      memset(cBT_setting, 0x00, 64);
      snprintf(cBT_setting, 64, SET_CONTROL_BATT, SET_CONTROL_BATT_LOW, SET_CONTROL_BATT_SHTDWN, SET_CONTROL_BATT_FULL, SET_CONTROL_BATT_MASK);
      cSetting = strstr(cBT_Rcvd_Settings, cBT_setting);
      if(cSetting == NULL)
      {
        eEC = ER_FAIL;
        vDEBUG_RDIO_RCV_SET("Radio set check, batt rcv fail");
      }
      else
      {
        eEC = ER_OK;
        eSettings_Check_Track++;
        if(eSettings_Check_Track != CHK_SET_BATT)
        {
          eEC = ER_FAIL;
        }
      }
    }

    //check if all the set control settings passed
    if(eEC == ER_FAIL)
    {
      //the SET BT settings were incorrect
      //
      eEC_set_cntrl = ER_FAIL;
    }
    else
    {
      //the SET CONTROL settings were correct
      //
      eEC = ER_OK;
      eEC_set_cntrl = ER_OK;
      eSettings_Check_Track++;
      if(eSettings_Check_Track != CHK_SET_END_OF_SET_CONTROL)
      {
        eEC = ER_FAIL;
      }
    }
  }

  //check if the SET BT, SET CONTROL and PIO settings all passed
  if((eEC_set_cntrl == ER_OK) &\
     (eEC_set_bt    == ER_OK))
  {
    //Settings were all correct
    eEC = ER_OK;
  }
  else
  {
    //settings were incorrect
    eEC = ER_FAIL;
  }

  free(cBT_Rcvd_Settings);

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

ERROR_CODE eIneedmd_radio_set_echo(void)
{
  ERROR_CODE eEC = ER_FAIL;
  char cSet_echo[21];
  char cRcv_echo[21];
  int iStr_Len = 0;
  int i = 0;
  int iCmp_rslt = -1;

  memset(cSet_echo, 0x00, 21);
  memset(cRcv_echo, 0x00, 21);
  snprintf(cSet_echo, 21, SET_CONTROL_ECHO, SET_CONTROL_ECHO_SETTING);
  iStr_Len = strlen(cSet_echo);

  eIneedmd_radio_send_frame((uint8_t *)cSet_echo, iStr_Len);
  eBSP_Radio_rcv_frame((uint8_t *)cRcv_echo, iStr_Len);

  while(iCmp_rslt != 0)
  {
    iCmp_rslt = strcmp(cSet_echo, cRcv_echo);
    if(iCmp_rslt == 0)
    {
      eEC = ER_OK;
      vDEBUG(cRcv_echo);
      break;
    }
    else
    {
      eOSAL_delay(100, NULL);
      i++;
      vDEBUG_ASSERT("radio_set_echo, cmnd echo never received", i < TIMEOUT_RCV_ECHO);
      eEC = ER_FAIL;
    }
  }

  eBSP_Radio_rcv_frame_halt();
  return eEC;
}

ERROR_CODE eIneedmd_radio_set_name(void)
{
  ERROR_CODE eEC = ER_FAIL;
  uint16_t iEC = 0;
  char * cSys_Default_Name = NULL;
  char * cBT_BDAddr = malloc(SET_NAME_BUFF_SIZE);
  char * pBT_BD_Max_Addr = NULL;

  cSys_Default_Name = strstr(SYSTEM_DEFAULT_NAME, tRT_Activity_State.cRadio_Device_ID);

  if(cSys_Default_Name != NULL)
  {
    //SET BT BDADDR, get BT address
    //
    memset(cBT_BDAddr, 0x00, SET_NAME_BUFF_SIZE);
    memcpy(cBT_BDAddr, SET_BT_BDADDR, strlen(SET_BT_BDADDR));
    eEC = eIneedmd_radio_send_frame((uint8_t *)cBT_BDAddr, strlen(cBT_BDAddr));
    if(eEC == ER_OK)
    {
      vDEBUG("Rdio set name, SET BT BDADDR, get the BT mac addr");
      vRADIO_ECHO(SET_BT_BDADDR);

      memset(cBT_BDAddr, 0x00, SET_NAME_BUFF_SIZE);
      eEC = eBSP_Radio_rcv_frame((uint8_t *)cBT_BDAddr, SET_NAME_BUFF_SIZE);
      if(eEC == ER_OK)
      {
        iEC = 0;
        while(1)
        {
          pBT_BD_Max_Addr = strstr(cBT_BDAddr, "\r\n");
          if(pBT_BD_Max_Addr == NULL)
          {
            eOSAL_delay(100, NULL);
            iEC++;
            if(iEC == 10)
            {
              eEC = ER_FAIL;
              break;
            }
          }
          else
          {
            break;
          }
        }
        vRADIO_ECHO(cBT_BDAddr);
      }

      eBSP_Radio_rcv_frame_halt();
    }
    else
    {
      vDEBUG_ASSERT("Send frame to radio failed", eEC == ER_OK);
    }


    if(eEC == ER_OK)
    {
      //parse string for the BT address
      //
      eEC = eIneedmd_parse_addr(cBT_BDAddr, uiBT_addr);
      vDEBUG_ASSERT("Rdio set name SYS HALT, no BT address found",(eEC == ER_OK));


      //SET BT NAME, format the send string with the BT name
      //
      memset(cBT_BDAddr, 0x00, SET_NAME_BUFF_SIZE);
      snprintf(cBT_BDAddr, SET_NAME_BUFF_SIZE, SET_BT_NAME, uiBT_addr[4], uiBT_addr[5]);
      //send the new name
      eEC = eIneedmd_radio_send_frame((uint8_t *)cBT_BDAddr, strlen(cBT_BDAddr));
      if(eEC == ER_OK)
      {
        vDEBUG("Rdio set name, set the new bt name");
        vRADIO_ECHO(cBT_BDAddr);
      }else{/*do nothing*/}
    }
  }
  else
  {
    //SET BT NAME, format the send string with the BT name
    //
    memset(cBT_BDAddr, 0x00, SET_NAME_BUFF_SIZE);

    //make sure the device ID is set
    if(tRT_Activity_State.cRadio_Device_ID[0] == 0x00)
    {
      read_dongle_ID(tRT_Activity_State.cRadio_Device_ID);
    }

    snprintf(cBT_BDAddr, SET_NAME_BUFF_SIZE, SET_BT_SYSTEM_NAME, tRT_Activity_State.cRadio_Device_ID);
    //send the new name
    eEC = eIneedmd_radio_send_frame((uint8_t *)cBT_BDAddr, strlen(cBT_BDAddr));
    if(eEC == ER_OK)
    {
      vDEBUG("Rdio set name, set the new bt name");
      vRADIO_ECHO(cBT_BDAddr);
    }else{/*do nothing*/}
  }
  free(cBT_BDAddr);
  return eEC;
}

ERROR_CODE eGet_Radio_Name(char * pBuff)
{
  ERROR_CODE eEC = ER_FAIL;

  return eEC;
}

ERROR_CODE eIneedmd_radio_set_config(void)
{
  ERROR_CODE eEC = ER_FAIL;
  char * pSyntax_error = NULL;
  char * cBT_Config = calloc(SET_CONFIG_BUFF_SIZE, sizeof(char));
  char * cBT_Get_Config = calloc(SET_CONFIG_BUFF_SIZE, sizeof(char));
  int iEC = 0;
  eRadio_setup_state eState;

  eState = eGet_radio_setup_state();
  if(eState == RDIO_SETUP_FINISH_RFD)
  {
    uiSet_ctrl_config[0] = (0x0000) | SET_CONTROL_CONFIG_OPT_BLK_3;
    uiSet_ctrl_config[1] = (0x0000) | SET_CONTROL_CONFIG_OPT_BLK_2;
    uiSet_ctrl_config[2] = (0x0400) | SET_CONTROL_CONFIG_OPT_BLK_1;
    uiSet_ctrl_config[3] = (0x1100) | SET_CONTROL_CONFIG_CFG_BLK;

    //SET CONTROL CONFIG, config the radio to notify for certian events
    //
    memset(cBT_Config, 0x00, SET_CONFIG_BUFF_SIZE);
    snprintf(cBT_Config, SET_CONFIG_BUFF_SIZE, SET_CONTROL_CONFIG, uiSet_ctrl_config[0], uiSet_ctrl_config[1], uiSet_ctrl_config[2], uiSet_ctrl_config[3]);
    eIneedmd_radio_send_frame((uint8_t *)cBT_Config, strlen(cBT_Config));
    eEC = ER_OK;
  }
  else
  {
    //SET CONTROL CONFIG, GET the radio config
    //
    memcpy(cBT_Get_Config, SET_CONTROL_CONFIG_GET, strlen(SET_CONTROL_CONFIG_GET));
    eEC = eIneedmd_radio_send_frame((uint8_t *)cBT_Get_Config, strlen(SET_CONTROL_CONFIG_GET));
    if(eEC == ER_OK)
    {
     vDEBUG("Rdio setup, SET CONTROL CONFIG, get the radio config");
     vRADIO_ECHO(SET_CONTROL_CONFIG_GET);
    }else{/*do nothing*/}

    eBSP_Radio_rcv_frame((uint8_t *)cBT_Config, SET_CONFIG_BUFF_SIZE);
    iEC = strlen(cBT_Config);
    while(iEC < SET_CONTROL_CONFIG_RCV_LEN)
    {
      eOSAL_delay(100, NULL);
      iEC = strlen(cBT_Config);
    }
    eBSP_Radio_rcv_frame_halt();

    pSyntax_error = strstr(cBT_Config, SYNTAX_ERROR);
    vDEBUG_ASSERT("Rdio set name SYS HALT, syntax error",(pSyntax_error == NULL));
    if(pSyntax_error == NULL)
    {
      eEC = ER_OK;
    }
    else
    {
      eEC = ER_FAIL;
    }

    sscanf(cBT_Config, SET_CONTROL_CONFIG_PARSE, cBT_Get_Config, cBT_Get_Config, cBT_Get_Config, &uiSet_ctrl_config[0], &uiSet_ctrl_config[1], &uiSet_ctrl_config[2], &uiSet_ctrl_config[3]);

    //todo: add the new config bits to set
//    uiSet_ctrl_config[0] |= ;
//    uiSet_ctrl_config[1] |= ;
//    uiSet_ctrl_config[2] |= ;
//    uiSet_ctrl_config[3] |= ;

    //SET CONTROL CONFIG, config the radio to notify for certian events
    //
    memset(cBT_Config, 0x00, SET_CONFIG_BUFF_SIZE);
    snprintf(cBT_Config, SET_CONFIG_BUFF_SIZE, SET_CONTROL_CONFIG, uiSet_ctrl_config[0], uiSet_ctrl_config[1], uiSet_ctrl_config[2], uiSet_ctrl_config[3]);
    eIneedmd_radio_send_frame((uint8_t *)cBT_Config, strlen(cBT_Config));
    vRADIO_ECHO(cBT_Config);
  }

  free(cBT_Config);
  free(cBT_Get_Config);
  return eEC;
}

/******************************************************************************
* name:
* description:
* param description: type - value: value description (in order from left to right)
*                    bool - true: do action when set to true
* return value description: ERROR_CODE - ER_CONNECTED: External carrier is already connected
*                                        ER_OK: System is waiting for a carrier connection
*                                        ER_FAIL: System failed to set up for wait for connection
******************************************************************************/
ERROR_CODE eIneedmd_radio_wait_for_conn(tRadio_message * ptMessage)
{
  ERROR_CODE eEC = ER_FAIL;

  if(ptMessage->uiTimeout == TIMEOUT_CONN_WAIT_FOREVER)
  {
    bDo_wait_forever_for_conn = true;
    Clock_stop(tRadio_conn_timeout_timer);
  }
  else
  {
    bDo_wait_forever_for_conn = false;
    Clock_stop(tRadio_conn_timeout_timer);
    Clock_setTimeout(tRadio_conn_timeout_timer, ptMessage->uiTimeout);
    Clock_start(tRadio_conn_timeout_timer);
  }

  eEC = eBSP_Get_Radio_CD_status();
  if(eEC == ER_SET)
  {
    GPIO_enableInt(EK_TM4C123GXL_RADIO_CD, GPIO_INT_FALLING);

    Clock_stop(tRadio_conn_timeout_timer);

    eEC = ER_CONNECTED;
  }
  else if(eEC == ER_NOT_SET)
  {
    GPIO_enableInt(EK_TM4C123GXL_RADIO_CD, GPIO_INT_RISING);

    eEC = ER_OK;
  }
  else
  {
    eEC = ER_FAIL;
  }

  return eEC;
}

void vRadio_CD_int_callback(void)
{
  ERROR_CODE eEC = ER_FAIL;
  tRadio_message tRadio_msg;

  GPIO_clearInt(EK_TM4C123GXL_RADIO_CD);

  eRadio_Message_Params_Init(&tRadio_msg);

  eEC = eBSP_Get_Radio_CD_status();
  if(eEC == ER_SET)
  {
    tRadio_msg.eMessage = RADIO_MSG_EVENT_CARRIER_CONNECTED;
    GPIO_enableInt(EK_TM4C123GXL_RADIO_CD, GPIO_INT_FALLING);
  }
  else
  {
    tRadio_msg.eMessage = RADIO_MSG_EVENT_CARRIER_DISCONNECT;
    GPIO_enableInt(EK_TM4C123GXL_RADIO_CD, GPIO_INT_RISING);
  }

  Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_WAIT_FOREVER);

  return;
}

void vRadio_LOW_BATT_int_callback(void)
{
  GPIO_clearInt(EK_TM4C123GXL_RADIO_LOW_BATT);
  vDEBUG("radio low batt!");
  return;
}

void vRadio_conn_timout_int_service(void)
{
  eRadio_connection_state eConn_State = RADIO_CONN_UNKNOWN;
  tRadio_request tRadio_Req;

  eConn_State = eGet_radio_connection_state();

  if(eConn_State == RADIO_CONN_WAITING_FOR_CONNECTION)
  {
    eIneedmd_radio_request_params_init (&tRadio_Req);
    tRadio_Req.eRequest = RADIO_REQUEST_HALT_WAIT_FOR_CONNECTION;
    eIneedmd_radio_request(&tRadio_Req);
  }
  return;
}

void vRadio_rcv_timeout_service_INT(void)
{
  eBSP_Radio_rcv_frame_halt();
  memset(cRdo_setup_rcv_buff, 0x00, BG_SIZE);
  eBSP_Radio_rcv_frame((uint8_t *)cRdo_setup_rcv_buff, 3);
  tRT_Activity_State.bIs_Radio_waiting_to_rcv = true;

  vDEBUG("vRadio_rcv_timeout_service_INT, read timeout");
  return;
}

void vRadio_USB_conn_callback(bool bIs_Phys_conn, bool bIs_Data_conn)
{
  tRadio_message tRadio_msg;
  eRadio_setup_state  eSetup_state;
  bool bDid_Msg_post = false;
  ERROR_CODE eEC = ER_FAIL;

  if((tRT_Activity_State.bIs_USB_Phys_conn != bIs_Phys_conn) |\
     (tRT_Activity_State.bIs_USB_Data_conn != bIs_Data_conn))
  {
    eRadio_Message_Params_Init(&tRadio_msg);
    if(bIs_Data_conn == true)
    {
      tRadio_msg.eMessage = RADIO_MSG_RADIO_PWR_DOWN;
      bDid_Msg_post = Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_NO_WAIT);
    }
    else
    {
      //check if a debugger is attached
      eEC = eBSP_debugger_detect();
      if(eEC == ER_FAIL)
      {
        //no debugger is attached
        if(bIs_Phys_conn == true)
        {
          tRadio_msg.eMessage = RADIO_MSG_RADIO_PWR_DOWN;
          bDid_Msg_post = Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_NO_WAIT);
        }
        else
        {
          tRadio_msg.eMessage = RADIO_MSG_RADIO_PWR_UP;
          bDid_Msg_post = Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_NO_WAIT);
        }
      }
      else
      {
        //debugger is attached

        //determine if the radio was turned off by a data connection
        eSetup_state = eGet_radio_setup_state();
        if(eSetup_state == RDIO_SETUP_RADIO_READY_POWERED_OFF)
        {
          //turn the radio back on
          tRadio_msg.eMessage = RADIO_MSG_RADIO_PWR_UP;
          bDid_Msg_post = Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_NO_WAIT);
        }
        else
        {
          //ignore the setup state
          bDid_Msg_post = true;
        }
      }
    }

    vDEBUG_ASSERT("vRadio_USB_conn_callback msg failed to post", (bDid_Msg_post == true));

    tRT_Activity_State.bIs_USB_Phys_conn = bIs_Phys_conn;
    tRT_Activity_State.bIs_USB_Data_conn = bIs_Data_conn;
  }

  return;
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

//  eCurrent_radio_connection_state = eConn_state;
  tRT_Activity_State.eRadio_conn_state = eConn_state;

//  if(eCurrent_radio_connection_state != eConn_state)
  if(tRT_Activity_State.eRadio_conn_state != eConn_state)
  {
    eEC = ER_FAIL;
  }
  else
  {
    eEC = ER_OK;
  }

  return eEC;
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

ERROR_CODE eInit_Radio_CB_Functions(void)
{
  ERROR_CODE eEC = ER_FAIL;
  int i = 0;

  for(i = 0; i < RADIO_CB_ARRAY_LIMIT; i++)
  {
    vRadio_Buff_sent_callback         [i] = NULL;
    vRadio_Buff_receive_callback      [i] = NULL;
    vRadio_Change_setting_callback    [i] = NULL;
    vRadio_Connection_status_callback [i] = NULL;
    vRadio_Setup_state_callback       [i] = NULL;
  }
  return eEC;
}

ERROR_CODE eRegister_Radio_CB_Functions(tRadio_request * tRequest)
{
  ERROR_CODE eEC = ER_FAIL;
  int i = 0;

  if((tRequest->vBuff_sent_callback         == NULL) |\
     (tRequest->vBuff_receive_callback      == NULL) |\
     (tRequest->vChange_setting_callback    == NULL) |\
     (tRequest->vConnection_status_callback == NULL) |\
     (tRequest->vSetup_state_callback       == NULL))
  {
    eEC = ER_FAIL;
  }
  else
  {
    eEC = ER_OK;
  }

  if(eEC == ER_OK)
  {
    for(i = 0; i < RADIO_CB_ARRAY_LIMIT; i++)
    {
      if((vRadio_Buff_sent_callback         [i] == NULL) |\
         (vRadio_Buff_receive_callback      [i] == NULL) |\
         (vRadio_Change_setting_callback    [i] == NULL) |\
         (vRadio_Connection_status_callback [i] == NULL) |\
         (vRadio_Setup_state_callback       [i] == NULL))
      {
        vRadio_Buff_sent_callback         [i] = tRequest->vBuff_sent_callback;
        vRadio_Buff_receive_callback      [i] = tRequest->vBuff_receive_callback;
        vRadio_Change_setting_callback    [i] = tRequest->vChange_setting_callback;
        vRadio_Connection_status_callback [i] = tRequest->vConnection_status_callback;
        vRadio_Setup_state_callback       [i] = tRequest->vSetup_state_callback;

        eEC = ER_OK;
        break;
      }
      else if((vRadio_Buff_sent_callback         [i] == tRequest->vBuff_sent_callback        ) &\
              (vRadio_Buff_receive_callback      [i] == tRequest->vBuff_receive_callback     ) &\
              (vRadio_Change_setting_callback    [i] == tRequest->vChange_setting_callback   ) &\
              (vRadio_Connection_status_callback [i] == tRequest->vConnection_status_callback) &\
              (vRadio_Setup_state_callback       [i] == tRequest->vSetup_state_callback      ))
      {
        eEC = ER_OK;
        break;
      }
      else
      {
        continue;
      }
    }
  }

  return eEC;
}

ERROR_CODE eNotify_Buff_sent_CB_Functions(uint32_t uiBytes_sent)
{
  ERROR_CODE eEC = ER_FAIL;

  int i;
  for(i = 0; i < RADIO_CB_ARRAY_LIMIT; i++)
  {
    if(vRadio_Buff_sent_callback[i] != NULL)
    {
      vRadio_Buff_sent_callback[i](uiBytes_sent);
      eEC = ER_OK;
    }
  }

  return eEC;
}

ERROR_CODE eNotify_Buff_receive_CB_Functions(uint8_t * pBuff, uint32_t uiRcvd_Buff_Len)
{
  ERROR_CODE eEC = ER_FAIL;

  int i;
  for(i = 0; i < RADIO_CB_ARRAY_LIMIT; i++)
  {
    if(vRadio_Buff_receive_callback[i] != NULL)
    {
      vRadio_Buff_receive_callback[i](pBuff, uiRcvd_Buff_Len);
      eEC = ER_OK;
    }
  }

  return eEC;
}

ERROR_CODE eNotify_Radio_Changed_Settings_CB_Functions(bool bRadio_Setting_Changed)
{
  ERROR_CODE eEC = ER_FAIL;

  vDEBUG_ASSERT("eNotify_Radio_Changed_Settings_CB_Functions SYS HALT, not implemented",0);

  return eEC;
}

ERROR_CODE eNotify_Radio_Connection_State_CB_Functions(bool bRadio_Carrier_Connection, bool bRadio_Protocol_Connection)
{
  ERROR_CODE eEC = ER_FAIL;

  int i;
  for(i = 0; i < RADIO_CB_ARRAY_LIMIT; i++)
  {
    if(vRadio_Connection_status_callback[i] != NULL)
    {
      vRadio_Connection_status_callback[i](bRadio_Carrier_Connection, bRadio_Protocol_Connection);
      eEC = ER_OK;
    }
  }

  return eEC;
}

ERROR_CODE eNotify_Radio_Setup_State_CB_Functions(bool bRadio_Ready, bool bRadio_On)
{
  ERROR_CODE eEC = ER_FAIL;
  int i;
  for(i = 0; i < RADIO_CB_ARRAY_LIMIT; i++)
  {
    if(vRadio_Setup_state_callback[i] != NULL)
    {
      vRadio_Setup_state_callback[i](bRadio_Ready, bRadio_On);
      eEC = ER_OK;
    }
  }

  return eEC;
}

ERROR_CODE eRadio_Message_Params_Init  (tRadio_message * pParams)
{
  ERROR_CODE eEC = ER_FAIL;
  int i;

  memset(pParams, 0x00, sizeof(tRadio_message));

  for(i = 0; i < MSG_BUFF_SIZE; i++)
  {
    if(pParams->uiBuff[i] != 0x00)
    {
      eEC = ER_FAIL;
      break;
    }
    else
    {
      eEC = ER_OK;
    }
  }

  if(eEC == ER_OK)
  {
    if((pParams->eMessage == RADIO_MSG_UNKNOWN) &\
       (pParams->eSetting == RADIO_SETTINGS_NONE))
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
eRadio_connection_state eGet_radio_connection_state(void)
{
//  return eCurrent_radio_connection_state;
  return tRT_Activity_State.eRadio_conn_state;
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

#ifdef NOT_NOW

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
  eBSP_Radio_clear_rcv_buffer();

  ineedmd_radio_reset();
  eEC = eBSP_Get_Radio_CTS_status();
  while(eEC == ER_FALSE)
  {
    eEC = eBSP_Get_Radio_CTS_status();
    if(eEC == ER_TRUE)
    {
      //clear any data in the UART
      eBSP_Radio_clear_rcv_buffer();
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
  eEC = eBSP_Get_Radio_CTS_status();
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
    eEC = eBSP_Get_Radio_CTS_status();
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
    eEC = eBSP_Get_Radio_CTS_status();
    while(eEC == ER_FALSE)
    {
      eEC = eBSP_Get_Radio_CTS_status();
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
      eEC = eBSP_Get_Radio_CTS_status();
      while(eEC == ER_FALSE)
      {
        eEC = eBSP_Get_Radio_CTS_status();
        if(eEC == ER_TRUE)
        {
          vDEBUG_RDIO_RFD("Rdio rfd, radio was soft reset");
        }else{/*do nothing*/}
      }

      //clear any potential uart traffic
      eSend_Radio_CMND(CR_NL, strlen(CR_NL));
      eBSP_Radio_clear_rcv_buffer();
      i_delay = 0;

      //send the reset command
      eSend_Radio_CMND(SET_RESET, strlen(SET_RESET));

      //wait for the radio to go into RFD by monitoring CTS pin transition from high to low
      eEC = eBSP_Get_Radio_CTS_status();
      while(eEC == ER_TRUE)
      {
        //delay for 1ms
        i_delay += iHW_delay(1);
        eEC = eBSP_Get_Radio_CTS_status();
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
        eEC = eBSP_Get_Radio_CTS_status();
        while(eEC == ER_FALSE)
        {
          eEC = eBSP_Get_Radio_CTS_status();
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
          eBSP_Radio_clear_rcv_buffer();
          eEC = ER_FAIL;
        }else{/*do nothing*/}
      }
    }
  }

  return eEC;

#undef vDEBUG_RDIO_RFD
}
#endif

void vIneedMD_radio_read_cb(UART_Handle sHandle, void *buf, size_t count)
{
  tRadio_message tRadio_msg;
  uint8_t * c = (uint8_t * )buf;

  if(count == 3)
  {
    if(c[0] == INEEDMD_CMND_SOF)
    {
      //Protocol frame
      //

      Clock_stop(tRadio_rcv_timeout);
      Clock_start(tRadio_rcv_timeout);
      tRadio_msg.uiFrame_len = c[2];
      memcpy(tRadio_msg.uiBuff, c, 3);
      tRT_Activity_State.uiRemainder_frame_rcv_len = c[2] - 3;

      eBSP_Radio_rcv_frame(&c[3], (c[2] - 3));
    }
    else
    {
      if(tRT_Activity_State.uiRemainder_frame_rcv_len != 0)
      {
        Clock_stop(tRadio_rcv_timeout);
        eRadio_Message_Params_Init(&tRadio_msg);
        tRadio_msg.uiFrame_len = count + 3;

        memcpy(tRadio_msg.uiBuff, cRdo_setup_rcv_buff, count + 3);
        tRadio_msg.eMessage = RADIO_MSG_FINISH_RCV_PROTOCOL_FRAME;
        Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_NO_WAIT);
        tRT_Activity_State.uiRemainder_frame_rcv_len = 0;
        memset(cRdo_setup_rcv_buff, 0x00, count + 3);
        tRT_Activity_State.bIs_Radio_waiting_to_rcv = false;
      }
    }

    return;
  }
  else if(count == tRT_Activity_State.uiRemainder_frame_rcv_len)
  {
    Clock_stop(tRadio_rcv_timeout);
    eRadio_Message_Params_Init(&tRadio_msg);
    tRadio_msg.uiFrame_len = count + 3;

    memcpy(tRadio_msg.uiBuff, cRdo_setup_rcv_buff, count + 3);
    tRadio_msg.eMessage = RADIO_MSG_FINISH_RCV_PROTOCOL_FRAME;
    Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_NO_WAIT);
    tRT_Activity_State.uiRemainder_frame_rcv_len = 0;
    memset(cRdo_setup_rcv_buff, 0x00, count + 3);
    tRT_Activity_State.bIs_Radio_waiting_to_rcv = false;
  }
  else if(count == 1024)
  {
    vDEBUG_ASSERT("Maxxed UART rcv buff", 0);
  }
  else
  {
    return;
  }
}

void vIneedMD_radio_write_cb(UART_Handle sHandle, void *buf, size_t  count)
{
  vDEBUG_ASSERT("vIneedMD_radio_write_cb bytes failed to send", (count == tRT_Activity_State.uiSend_frame_len));
  if(count == tRT_Activity_State.uiSend_frame_len)
  {
    tRT_Activity_State.uiSend_frame_len = 0;
    tRT_Activity_State.bDid_frame_send = true;
  }

  return;
}

/******************************************************************************
* name: eIneedmd_radio_request_params_init
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
  tRadio_message tRadio_msg;

  bool bDid_message_post = false;
  eRadio_connection_state eConn_State;
  eRadio_setup_state  eSetup_state;
  uint8_t * pSend_Buff = NULL;
  int i = 0;

  eSetup_state = eGet_radio_setup_state();

  if(tRequest->eRequest == RADIO_REQUEST_REGISTER_CALLBACKS)
  {
    vDEBUG_ASSERT("eIneedmd_radio_request SYS_HALT, RADIO_REQUEST_REGISTER_CALLBACKS parameters are invalid", ((tRequest->vBuff_sent_callback         != NULL) &\
                                                                                                                           (tRequest->vBuff_receive_callback      != NULL) &\
                                                                                                                           (tRequest->vChange_setting_callback    != NULL) &\
                                                                                                                           (tRequest->vConnection_status_callback != NULL) &\
                                                                                                                           (tRequest->vSetup_state_callback       != NULL)));

    eEC = eRegister_Radio_CB_Functions(tRequest);
  }
  else if(tRequest->eRequest == RADIO_REQUEST_PERMANENT_POWER_OFF)
  {
    switch(eSetup_state)
    {
      case RDIO_SETUP_NOT_STARTED:
        //The radio setup was not started, ensure the radio is off and prevent setup from occuring
        //
        eRadio_power(false);

        tRT_Activity_State.bIs_Radio_Permanetly_off = true;
        break;
      case RDIO_SETUP_READY:
        //force the radio to drop connection
        eIneedmd_radio_cmnd_mode(true);

        //verify carrier has dropped
        eEC = eBSP_Get_Radio_CD_status();
        while(eEC == ER_SET)
        {
          //force the radio to drop connection
          eIneedmd_radio_cmnd_mode(true);
          eEC = eBSP_Get_Radio_CD_status();
        }

        eConn_State = eGet_radio_connection_state();
        if((eConn_State == RADIO_CONN_VERIFYING_CONNECTION) |\
           (eConn_State == RADIO_CONN_CONNECTED))
        {
          eSet_connection_state(RADIO_CONN_NONE);
          //notify callbacs of carrier disconnection
          //
          eNotify_Radio_Connection_State_CB_Functions(false, false);
        }

        //set the radio page mode to ignore in case it stays on through set power off
        pSend_Buff = calloc(BG_SEND_SIZE_SMALL, sizeof(char));
        memcpy(pSend_Buff, SET_BT_PAGEMODE_OFF, strlen(SET_BT_PAGEMODE_OFF));
        eIneedmd_radio_send_frame(pSend_Buff, strlen((char *)pSend_Buff));
        while(tRT_Activity_State.bDid_frame_send == false)
        {
          eOSAL_delay(1, NULL);
          i++;
          if(i >=TIMEOUT_SEND_FRAME)
          {
            vDEBUG_ASSERT("vIneedMD_radio_task, RADIO_MSG_SEND timeout", 0);
            eEC = ER_FAIL;
            eBSP_Radio_send_frame_halt();
            break;
          }
        }

        char cBT_setting[64];
        eBSP_Radio_rcv_frame((uint8_t *)cBT_setting, 64);
        Task_sleep(100);
        eBSP_Radio_rcv_frame_halt();

        eRadio_power(false);

        eSet_radio_setup_state(RDIO_SETUP_RADIO_READY_POWERED_OFF);
        //notify callbacks of current power state
        eNotify_Radio_Setup_State_CB_Functions(true, false);

        tRT_Activity_State.bIs_Radio_Permanetly_off = true;
        break;
      case RDIO_SETUP_RADIO_READY_POWERED_OFF:
        tRT_Activity_State.bIs_Radio_Permanetly_off = true;
        break;
      case RDIO_SETUP_ENABLE_INTERFACE:
      case RDIO_SETUP_INTERFACE_ENABLED:
      case RDIO_SETUP_START_POWER:
      case RDIO_SETUP_FINISH_POWER:
      case RDIO_SETUP_START_SOFT_RESET:
      case RDIO_SETUP_FINISH_SOFT_RESET:
      case RDIO_SETUP_START_RFD:
      case RDIO_SETUP_FINISH_RFD:
        //The radio is in the middle of setup, cannot permanently power off the radio at this time.
        //
        eEC = ER_NOT_READY;
        break;
      default:
        //the radio is in an unsupported state to enable permanent power off
        //
        eEC = ER_FAIL;
        break;
    }
  }
  else
  {
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
        if(tRT_Activity_State.bIs_Radio_Permanetly_off == false)
        {
          if(tRequest->eRequest < RADIO_REQUEST_LIMIT)
          {
            switch(tRequest->eRequest)
            {
              case RADIO_REQUEST_CHANGE_SETTINGS:
                eRadio_Message_Params_Init(&tRadio_msg);
                tRadio_msg.eMessage = RADIO_MSG_RADIO_CHANGE_SETTINGS;
                bDid_message_post = Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_WAIT_FOREVER);
                break;
              case RADIO_REQUEST_SEND_FRAME:
                //check if the buffer size is within limits
                if((tRequest->uiBuff_size > MSG_BUFF_SIZE) |\
                   (tRequest->uiBuff_size == 0))
                {
                  eEC = ER_BUFF_SIZE;
                }
                else
                {
                  eRadio_Message_Params_Init(&tRadio_msg);
                  tRadio_msg.eMessage = RADIO_MSG_SEND;
                  tRadio_msg.uiFrame_len = tRequest->uiBuff_size;
                  memcpy(tRadio_msg.uiBuff, tRequest->uiBuff, tRequest->uiBuff_size);
                  bDid_message_post = Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_WAIT_FOREVER);
                }

                break;
              case RADIO_REQUEST_RECEIVE_FRAME:
                eConn_State = eGet_radio_connection_state();
                vDEBUG_ASSERT("eIneedmd_radio_request SYS HALT, invalid connection state to receive frames", (eConn_State == RADIO_CONN_CONNECTED));
                if(eConn_State == RADIO_CONN_CONNECTED)
                {
                  if(tRT_Activity_State.bIs_Radio_waiting_to_rcv == false)
                  {
                    eBSP_Radio_rcv_frame((uint8_t *)cRdo_setup_rcv_buff, 3);
                    tRT_Activity_State.bIs_Radio_waiting_to_rcv = true;
                  }
                  eEC = ER_OK;
                }
                else
                {
                  eEC = ER_FAIL;
                }

                break;
              case RADIO_REQUEST_WAIT_FOR_CONNECTION:
                eConn_State = eGet_radio_connection_state();
                if(eConn_State == RADIO_CONN_NONE)
                {
                  eRadio_Message_Params_Init(&tRadio_msg);
                  tRadio_msg.eMessage = RADIO_MSG_WAIT_FOR_CONNECTION;
                  tRadio_msg.uiTimeout = tRequest->uiTimeout;
                  bDid_message_post = Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_WAIT_FOREVER);
                }
                else if(eConn_State == RADIO_CONN_CONNECTED)
                {
                  eEC = ER_CONNECTED;
                }
                break;
              case RADIO_REQUEST_HALT_WAIT_FOR_CONNECTION:
                eConn_State = eGet_radio_connection_state();
                if(eConn_State == RADIO_CONN_WAITING_FOR_CONNECTION)
                {
                  eSet_connection_state(RADIO_CONN_HALT);

//                  eBSP_Set_radio_uart_to_blocking();
//                  tRT_Activity_State.bIs_Interface_In_callback_mode = false;

                  //notify callbacks of halt wait for connection
                  //
                  eNotify_Radio_Connection_State_CB_Functions(false, false);
                }
                break;
              case RADIO_REQUEST_BREAK_CONNECTION:
                eConn_State = eGet_radio_connection_state();
                if(eConn_State == RADIO_CONN_CONNECTED)
                {
                  eRadio_Message_Params_Init(&tRadio_msg);
                  tRadio_msg.eMessage = RADIO_MSG_BREAK_CONNECTION;
                  bDid_message_post = Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_WAIT_FOREVER);
                }
                break;
              case RADIO_REQUEST_POWER_ON:
                //radio is already powered on set the error code to ok
                eEC = ER_OK;

                //notify callbacks of radio setup ON state
                //
                eNotify_Radio_Setup_State_CB_Functions(true, true);

                break;
              case RADIO_REQUEST_POWER_OFF:
                //power off the radio
                eRadio_power(false);
                eNotify_Radio_Setup_State_CB_Functions(true, false);
                eEC = ER_OFF;
                break;
              case RADIO_REQUEST_PERMANENT_POWER_OFF:
                //permanently power the radio off
                //!!WARNING!! this request will not allow any task or process to turn the radio back on.
                //Turning the radio back on will require a system reset

                //todo: permanent shut off routine
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
        else
        {
          eEC = ER_OFF;
        }
      }
    }
    else if(eSetup_state == RDIO_SETUP_RADIO_READY_POWERED_OFF)
    {
      if(tRT_Activity_State.bIs_Radio_Permanetly_off == false)
      {
        if(tRT_Activity_State.bIs_USB_Phys_conn == false)
        {
          if(tRequest->eRequest == RADIO_REQUEST_POWER_ON)
          {
            eRadio_Message_Params_Init(&tRadio_msg);
            tRadio_msg.eMessage = RADIO_MSG_RADIO_PWR_UP;
            bDid_message_post = Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_WAIT_FOREVER);
            if(bDid_message_post == true)
            {
              eEC = ER_OK;
            }
            else
            {
              eEC = ER_FAIL;
            }
          }
        }
        else
        {
          //check if a USB data connection is established
          if(tRT_Activity_State.bIs_USB_Data_conn == true)
          {
            //USB has a data connection, radio is off, return off error code
            eEC = ER_OFF;
          }
          else
          {
            //USB only has a physical/charging connection

            //check if a debugger is attached
            eEC = eBSP_debugger_detect();
            if(eEC == ER_OK)
            {
              //debugger is attached, allow the radio to be turned on
              if(tRequest->eRequest == RADIO_REQUEST_POWER_ON)
              {
                eRadio_Message_Params_Init(&tRadio_msg);
                tRadio_msg.eMessage = RADIO_MSG_RADIO_PWR_UP;
                bDid_message_post = Mailbox_post(tTIRTOS_Radio_mailbox, &tRadio_msg, BIOS_WAIT_FOREVER);
                if(bDid_message_post == true)
                {
                  eEC = ER_OK;
                }
                else
                {
                  eEC = ER_FAIL;
                }
              }
            }
            else
            {
              //Debugger is not attached, usb is plugged in, radio cannot be turned on
              eEC = ER_OFF;
            }
          }
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
  tRadio_message tRadio_Message;
  uint16_t uiRcvd_str_len = 0;
  int i;
  tUSB_req tUSB_request;
  uint8_t uiSend_Buff[BG_SEND_SIZE_SMALL];

  uint32_t uiMsg_size = sizeof(tRadio_Message);
  uint32_t uiMbox_size = 0;
  uiMbox_size = Mailbox_getMsgSize(tTIRTOS_Radio_mailbox);
  vDEBUG_ASSERT("vIneedMD_radio_task SYS HALT, invalid mailbox msg size!",(uiMsg_size == uiMbox_size));

  eInit_Radio_CB_Functions();

  read_dongle_ID(tRT_Activity_State.cRadio_Device_ID);

  eUSB_request_params_init(&tUSB_request);
  tUSB_request.eRequest = USB_REQUEST_REGISTER_CONN_CALLBACK;
  tUSB_request.vUSB_connection_callback = &vRadio_USB_conn_callback;
  eUSB_request(&tUSB_request);

  eEC = eIneedMD_radio_setup();
  vDEBUG_ASSERT("vIneedMD_radio_task radio setup failure!", ((eEC == ER_OK) | (eEC == ER_DONE)));

  vDEBUG_RDIO_TASK("vIneedMD_radio_task, radio setup success");
  eSet_connection_state(RADIO_CONN_NONE);

  //turn the radio off untill needed
  eSet_radio_setup_state(RDIO_SETUP_RADIO_READY_POWERED_OFF);
  eRadio_power(false);

  eRadio_Message_Params_Init(&tRadio_Message);

  while(1)
  {
    if(Mailbox_pend(tTIRTOS_Radio_mailbox, &tRadio_Message, BIOS_WAIT_FOREVER) == true)
    {

      switch(tRadio_Message.eMessage)
      {
        case RADIO_MSG_RADIO_CHANGE_SETTINGS://RADIO_REQUEST_CHANGE_SETTINGS:
          break;
        case RADIO_MSG_RADIO_PWR_UP://RADIO_REQUEST_POWER_ON:
          eRadio_power(true);

          //check if a carrier was still connected
          eEC = eBSP_Get_Radio_CD_status();
          if(eEC == ER_NOT_SET)
          {
            //no carrier detected, ensure radio is in proper page mode
            memset(uiSend_Buff, 0x00, BG_SEND_SIZE_SMALL);
            memcpy(uiSend_Buff, SET_BT_PAGEMODE_ON, strlen(SET_BT_PAGEMODE_ON));
            eEC = eIneedmd_radio_send_frame(uiSend_Buff, strlen((char *)uiSend_Buff));
          }

          eSet_radio_setup_state(RDIO_SETUP_READY);

          eNotify_Radio_Setup_State_CB_Functions(true, true);

          vDEBUG_RDIO_TASK("Radio task, radio powered on");
          break;
        case RADIO_MSG_RADIO_PWR_DOWN://RADIO_REQUEST_POWER_OFF:
          //force the radio to drop connection
          eIneedmd_radio_cmnd_mode(false);

          Task_sleep(500);

          //force the radio to drop connection
          eIneedmd_radio_cmnd_mode(true);

          //verify carrier has dropped
          eEC = eBSP_Get_Radio_CD_status();
          while(eEC == ER_SET)
          {
            //force the radio to drop connection
            eIneedmd_radio_cmnd_mode(true);
            eEC = eBSP_Get_Radio_CD_status();
          }

          eConn_State = eGet_radio_connection_state();
          if((eConn_State == RADIO_CONN_VERIFYING_CONNECTION) |\
             (eConn_State == RADIO_CONN_CONNECTED))
          {
            eSet_connection_state(RADIO_CONN_NONE);
            //notify callbacs of carrier disconnection
            //
            eNotify_Radio_Connection_State_CB_Functions(false, false);
          }

          //set the radio page mode to ignore in case it stays on through set power off
          memset(uiSend_Buff, 0x00, BG_SEND_SIZE_SMALL);
          memcpy(uiSend_Buff, SET_BT_PAGEMODE_OFF, strlen(SET_BT_PAGEMODE_OFF));
          eIneedmd_radio_send_frame(uiSend_Buff, strlen((char *)uiSend_Buff));

          eBSP_Radio_clear_rcv_buffer();

          eRadio_power(false);

          eSet_radio_setup_state(RDIO_SETUP_RADIO_READY_POWERED_OFF);
          //notify callbacks of current power state
          eNotify_Radio_Setup_State_CB_Functions(true, false);

          vDEBUG_RDIO_TASK("vIneedMD_radio_task, radio off");

          break;
        case RADIO_MSG_SEND://RADIO_REQUEST_SEND_FRAME:
          i = 0;
          while(tRT_Activity_State.bDid_frame_send == false)
          {
            eOSAL_delay(1, NULL);
            i++;
            if(i >=TIMEOUT_SEND_FRAME)
            {
              vDEBUG_ASSERT("vIneedMD_radio_task, RADIO_MSG_SEND timeout", 0);
              eEC = ER_FAIL;
              eBSP_Radio_send_frame_halt();
              break;
            }
          }
          eEC = eIneedmd_radio_send_frame(tRadio_Message.uiBuff, tRadio_Message.uiFrame_len);

          //Notify callbacks of frame sent
          eNotify_Buff_sent_CB_Functions(tRadio_Message.uiFrame_len);

          break;
        case RADIO_MSG_RCV_RADIO_MESSAGE://RADIO_REQUEST_RECEIVE_RADIO_FRAME:
          eEC = eIneedmd_radio_rcv_string((char *)&tRadio_Message.uiBuff[3], MSG_BUFF_SIZE, &uiRcvd_str_len);
          if(eEC == ER_TIMEOUT)
          {
            //todo: notify callbacks of malformed frame
            eBSP_Radio_clear_rcv_buffer();

            //force the radio to drop connection
            eIneedmd_radio_cmnd_mode(true);

            //verify carrier has dropped
            eEC = eBSP_Get_Radio_CD_status();
            while(eEC == ER_SET)
            {
              eEC = eBSP_Get_Radio_CD_status();
            }

            //notify callbacs of carrier disconnection
            //
            eNotify_Radio_Connection_State_CB_Functions(false, false);

          }
          else
          {
            //todo: parse potential received radio strings
            vDEBUG_ASSERT("vIneedMD_radio_task SYS HALT, received unhandled radio string", 0);
          }

          break;
        case RADIO_MSG_RCV_PROTOCOL_FRAME://RADIO_REQUEST_RECEIVE_PROTOCOL_FRAME:

          eEC = eBSP_Radio_rcv_frame((uint8_t *)&cRdo_setup_rcv_buff[3], (tRadio_Message.uiFrame_len - 3));
          tRT_Activity_State.bIs_Radio_waiting_to_rcv = true;

          break;
        case RADIO_MSG_FINISH_RCV_PROTOCOL_FRAME:
          eConn_State = eGet_radio_connection_state();
          if(eConn_State == RADIO_CONN_VERIFYING_CONNECTION)
          {
            if((tRadio_Message.uiBuff[0] == INEEDMD_CMND_SOF) & \
               (tRadio_Message.uiBuff[4] == INEEDMD_CMND_EOF))
            {
              eSet_connection_state(RADIO_CONN_CONNECTED);

              //notify callbacks of protocol connection
              //
              eNotify_Radio_Connection_State_CB_Functions(true, true);

              //notify callbacks of received frame
              //
              eNotify_Buff_receive_CB_Functions(tRadio_Message.uiBuff, tRadio_Message.uiFrame_len);
            }
            else
            {
              if(tRT_Activity_State.bIs_Radio_waiting_to_rcv == false)
              {
                eBSP_Radio_rcv_frame((uint8_t *)cRdo_setup_rcv_buff, 3);
                tRT_Activity_State.bIs_Radio_waiting_to_rcv = true;
              }
            }
          }
          else if(eConn_State == RADIO_CONN_CONNECTED)
          {
            //notify callbacks of received frame
            //
            eNotify_Buff_receive_CB_Functions(tRadio_Message.uiBuff, tRadio_Message.uiFrame_len);
          }
          else if(eConn_State == RADIO_CONN_NONE)
          {
            tRT_Activity_State.bIs_Radio_waiting_to_rcv = false;
          }
          else
          {
            if(tRT_Activity_State.bIs_Radio_waiting_to_rcv == false)
            {
              eBSP_Radio_rcv_frame((uint8_t *)cRdo_setup_rcv_buff, 3);
              tRT_Activity_State.bIs_Radio_waiting_to_rcv = true;
            }
          }
          break;
        case RADIO_MSG_WAIT_FOR_CONNECTION://RADIO_REQUEST_WAIT_FOR_CONNECTION:
          eConn_State = eGet_radio_connection_state();

          //check the connection state
          if(eConn_State == RADIO_CONN_HALT)
          {
            //A a request to halt connection was requested
            //
            eNotify_Radio_Connection_State_CB_Functions(false, false);
            continue;
          }
          else if((eConn_State == RADIO_CONN_NONE) |
                  (eConn_State == RADIO_CONN_WAITING_FOR_CONNECTION))
          {
            //No connection or waiting for connection state
            //

            //Set the state to waiting for connection
            eSet_connection_state(RADIO_CONN_WAITING_FOR_CONNECTION);

            eEC = eIneedmd_radio_wait_for_conn(&tRadio_Message);

            //Check the error codes
            if(eEC == ER_CONNECTED)
            {
              //External carrier is already connected
              //

              //Set the radio connection state to connected
              eSet_connection_state(RADIO_CONN_CONNECTED);

              //notify callbacks of carrier connection
              //
              eNotify_Radio_Connection_State_CB_Functions(true, false);

              vDEBUG_RDIO_TASK("Radio task, radio carrier already connected!");

              eSet_connection_state(RADIO_CONN_VERIFYING_CONNECTION);

              //Set the radio interface to callback mode to pre-parse any incoming radio frames
              //
              eBSP_Set_radio_uart_to_callback();
              tRT_Activity_State.bIs_Interface_In_callback_mode = true;
              if(tRT_Activity_State.bIs_Radio_waiting_to_rcv == false)
              {
                eBSP_Radio_rcv_frame((uint8_t *)cRdo_setup_rcv_buff, 3);
                tRT_Activity_State.bIs_Radio_waiting_to_rcv = true;
              }
            }
            else if(eEC == ER_OK)
            {
              //Radio is in a wait for connections state
              //
              eSet_connection_state(RADIO_CONN_WAITING_FOR_CONNECTION);

              //If the radio is supposed to wait forever for a connection notify the callbacks of the immediate connection state
              //
              if(bDo_wait_forever_for_conn == true)
              {
                eNotify_Radio_Connection_State_CB_Functions(false, false);
              }
            }
            else if(eEC == ER_FAIL)
            {
              //Radio failed to go into a wait for connection state
              //
              eSet_connection_state(RADIO_CONN_ERROR);

              vDEBUG_ASSERT("vIneedMD_radio_task SYS HALT, RADIO_MSG_WAIT_FOR_CONNECTION unhandled wait for connection failure", 0);

              //notify callback of radio failure
              //
              //todo: callbacks
            }
          }
          else if(eConn_State == RADIO_CONN_CONNECTED)
          {
            //Radio carrier is already connected
            //

            //check callback and notfiy connection already established
            //
            eNotify_Radio_Connection_State_CB_Functions(true, false);
            continue;
          }
          else
          {
            //invalid conn state
            //

            vDEBUG_ASSERT("vIneedMD_radio_task SYS HALT, RADIO_MSG_WAIT_FOR_CONNECTION invalid conn state", 0);

            continue;
          }
          break;
        case RADIO_MSG_BREAK_CONNECTION:
        {
          //Check if the radio already has a carrier connection
          //
          eEC = eBSP_Get_Radio_CD_status();
          if(eEC == ER_SET)
          {
            //set low the gpio pin to put the radio in command mode
            eIneedmd_radio_cmnd_mode(true);

            //Set the radio connection state to breaking connection
            eSet_connection_state(RADIO_CONN_BREAKING_CONNECTION);
          }
          else
          {
            eSet_connection_state(RADIO_CONN_NONE);
            eNotify_Radio_Connection_State_CB_Functions(false, false);
          }
          break;
        }
        case RADIO_MSG_EVENT_CARRIER_CONNECTED://RADIO_REQUEST_CARRIER_CONNECTD:
          //A carrier connection was made

          //get the current radio connection state
          eConn_State = eGet_radio_connection_state();
          vDEBUG_ASSERT("vIneedMD_radio_task SYS HALT, RADIO_REQUEST_CARRIER_CONNECTD received while in invalid wait for connection state", (eConn_State == RADIO_CONN_WAITING_FOR_CONNECTION));
          if(eConn_State == RADIO_CONN_WAITING_FOR_CONNECTION)
          {
            vDEBUG_RDIO_TASK("Radio task, radio carrier connected!");

            eSet_connection_state(RADIO_CONN_VERIFYING_CONNECTION);

            if(tRT_Activity_State.bIs_Radio_waiting_to_rcv == false)
            {
              eBSP_Radio_rcv_frame((uint8_t *)cRdo_setup_rcv_buff, 3);
              tRT_Activity_State.bIs_Radio_waiting_to_rcv = true;
            }

            //notify callbacks of carrier connection
            //
            eNotify_Radio_Connection_State_CB_Functions(true, false);
          }
          else
          {

            eBSP_Radio_clear_rcv_buffer();

            //force the radio to drop connection
            eIneedmd_radio_cmnd_mode(true);

            //verify carrier has dropped
            eEC = eBSP_Get_Radio_CD_status();
            while(eEC == ER_SET)
            {
              eEC = eBSP_Get_Radio_CD_status();
            }

            //notify callbacs of carrier disconnection
            //
            eNotify_Radio_Connection_State_CB_Functions(false, false);
          }
          break;
        case RADIO_MSG_EVENT_CARRIER_DISCONNECT://RADIO_REQUEST_CARRIER_DISCONNECT:
          //A carrier dis-connection was made

          //get the current radio connection state
          eConn_State = eGet_radio_connection_state();
          vDEBUG_ASSERT("vIneedMD_radio_task SYS HALT, RADIO_REQUEST_CARRIER_CONNECTD received while in invalid wait for connection state", ((eConn_State == RADIO_CONN_CONNECTED) | (eConn_State == RADIO_CONN_VERIFYING_CONNECTION) | (eConn_State == RADIO_CONN_NONE) | (eConn_State == RADIO_CONN_BREAKING_CONNECTION)));
          if((eConn_State == RADIO_CONN_CONNECTED)            |
             (eConn_State == RADIO_CONN_VERIFYING_CONNECTION) |
             (eConn_State == RADIO_CONN_BREAKING_CONNECTION))
          {
            vDEBUG_RDIO_TASK("Radio task, radio carrier dis-connected!");

            eSet_connection_state(RADIO_CONN_NONE);

            //Set the radio interface to blocking mode to receive any incoming radio frames
            //
            eBSP_Radio_clear_rcv_buffer();

            //notify callbacs of carrier disconnection
            //
            eNotify_Radio_Connection_State_CB_Functions(false, false);

            tRT_Activity_State.bIs_Radio_waiting_to_rcv = false;
          }
          else
          {
            //Set the radio interface to blocking mode to receive any incoming radio frames
            //
            eBSP_Radio_clear_rcv_buffer();
          }

          break;
        default:
          vDEBUG_ASSERT("vIneedMD_radio_task SYS HALT, unhandled radio message", 0);
          break;
      }
    }else{/*do nothing*/}
  }

#undef vDEBUG_RDIO_TASK
}

/*
 * END
 */
#endif //__INEEDMD_BLUETOOTH_RADIO_C__
