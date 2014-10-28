//*****************************************************************************
//
// ineedmd_command_protocol.h - include file for the indeedMD command protocol
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __INEEDMD_COMMAND_PROTOCOL_H__
#define __INEEDMD_COMMAND_PROTOCOL_H__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
*public defines ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#define COMMAND_DATA_BYTE 0x04
#define INMD_FRAME_BUFF_SIZE    256
#define NACK_FRAME_SIZE  12
#define ACK_FRAME_SIZE   12
//#define DWORD    unsigned long

#define COMMAND_FLAG      0x01
#define STATUS_FLAG       0x02
#define MEASURE_FLAG      0x03
#define STATUS_HIBERNATE  0x00
#define STATUS_SLEEP      0x20
#define STATUS_ON         0x40
#define STATUS_POWER      0x60

#define REQUEST_TO_TEST   0x18

#define STOP_EKG_TEST_PAT 0x00

#define EKG_TEST_PAT      0x01
#define TRIANGLE_TEST_PAT 0x00
#define SQUARE_TEST_PAT   0x01
#define WAVEFORM_TEST_PAT 0x02

#define LED_TEST_PATTERN  0x02
#define REQ_FOR_DFU       0x03
#define REQ_FOR_RESET     0x04
/******************************************************************************
*public variables /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
*public enums /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//led commands
typedef enum
{
  INMD_LED_CMND_LED_OFF = 0x00,
  INMD_LED_CMND_POWER_ON_BATT_GOOD = 0x01,
  INMD_LED_CMND_POWER_ON_BATT_LOW = 0x02,
  INMD_LED_CMND_BATT_CHARGING = 0x03,
  INMD_LED_CMND_BATT_CHARGING_LOW = 0x04,
  INMD_LED_CMND_LEAD_LOOSE = 0x05,
  INMD_LED_CMND_LEAD_GOOD_UPLOADING = 0x06,
  INMD_LED_CMND_DIG_FLATLINE = 0x07,
  INMD_LED_CMND_BT_CONNECTED = 0x08,
  INMD_LED_CMND_BT_ATTEMPTING = 0x09,
  INMD_LED_CMND_BT_FAILED = 0x0A,
  INMD_LED_CMND_USB_CONNECTED = 0x0B,
  INMD_LED_CMND_USB_FAILED = 0x0C,
  INMD_LED_CMND_DATA_TRANSFER = 0x0D,
  INMD_LED_CMND_TRANSFER_DONE = 0x0E,
  INMD_LED_CMND_STORAGE_WARNING = 0x0F,
  INMD_LED_CMND_ERASING = 0x10,
  INMD_LED_CMND_ERASE_DONE = 0x11,
  INMD_LED_CMND_DFU_MODE = 0x12,
  INMD_LED_CMND_MV_CAL = 0x13,
  INMD_LED_CMND_TRI_WVFRM = 0x14,
  INMD_LED_CMND_REBOOT = 0x15,
  INMD_LED_CMND_HIBERNATE = 0x16,
  INMD_LED_CMND_LEADS_ON = 0x17,
  INMD_LED_CMND_MEMORY_TEST = 0x18,
  INMD_LED_CMND_COM_BUS_TEST = 0x19,
  INMD_LED_CMND_CPU_CLOCK_TEST = 0x1A,
  INMD_LED_CMND_FLASH_TEST = 0x1B,
  INMD_LED_CMND_TEST_PASS = 0x1C,
  INMD_LED_CMND_POWER_UP_GOOD,
  INMD_LED_CMND_ACTUAL_DFU,
  INMD_LED_CMND_LIMIT
}INMD_LED_COMMAND;

typedef enum eCommand_Request_notify
{
  CMND_REQUEST_NONE,
  CMND_REQUEST_WAKEUP,
  CMND_REQUEST_SLEEP,
  CMND_REQUEST_RCV_PROTOCOL_FRAME,
  CMND_NOTIFY_INTERFACE_ESTABLISHED,
  CMND_NOTIFY_PROTOCOL_INTERFACE_ESTABLISHED,
  CMND_NOTIFY_PROTOCOL_INTERFACE_TIME_OUT,
  CMND_NOTIFY_PROTOCOL_INTERFACE_CLOSED,
}eCommand_Request_notify;
/******************************************************************************
*public structures ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
typedef struct tINMD_protocol_req_notify
{
  eCommand_Request_notify eReq_Notify;
  uint8_t *               uiCmnd_Frame;
  uint32_t                uiFrame_Len;
}tINMD_protocol_req_notify;
/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eIneedmd_cmnd_Proto_ReqNote_params_init(tINMD_protocol_req_notify * ptParams);
ERROR_CODE eIneedmd_cmnd_Proto_Request_Notify(tINMD_protocol_req_notify * ptParams);
int iIneedmd_Rcv_cmnd_frame(uint8_t * uiCmnd_Frame, uint16_t uiCmnd_Frame_len);
int iIneedmd_Send_cmnd_frame(uint8_t * uiCmnd_Frame, uint16_t uiCmnd_Frame_len);
//int iIneedmd_command_process(void);

#endif //__INEEDMD_COMMAND_PROTOCOL_H__
