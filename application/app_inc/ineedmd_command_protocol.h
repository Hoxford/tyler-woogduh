//*****************************************************************************
//
// ineedmd_command_protocol.h - include file for the indeedMD command protocol
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __INEEDMD_COMMAND_PROTOCOL_H__
#define __INEEDMD_COMMAND_PROTOCOL_H__
//*****************************************************************************
// includes
//*****************************************************************************

//*****************************************************************************
// defines
//*****************************************************************************
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
//*****************************************************************************
// variables
//*****************************************************************************

//*****************************************************************************
// external variables
//*****************************************************************************

//*****************************************************************************
// enums
//*****************************************************************************
//led commands
typedef enum
{
  INMD_LED_CMND_LED_OFF = 0x00,
  INMD_LED_CMND_POWER_ON_BATT_GOOD = 0x01,
  INMD_LED_CMND_BATT_CHARGING,
  INMD_LED_CMND_BATT_CHARGING_LOW,
  INMD_LED_CMND_POWER_ON_BATT_LOW,
  INMD_LED_CMND_LEAD_LOOSE,
  INMD_LED_CMND_LEAD_GOOD_UPLOADING,
  INMD_LED_CMND_DIG_FLATLINE,
  INMD_LED_CMND_BT_CONNECTED,
  INMD_LED_CMND_BT_ATTEMPTING,
  INMD_LED_CMND_BT_FAILED,
  INMD_LED_CMND_USB_CONNECTED,
  INMD_LED_CMND_USB_FAILED,
  INMD_LED_CMND_DATA_TRANSFER,
  INMD_LED_CMND_TRANSFER_DONE,
  INMD_LED_CMND_STORAGE_WARNING,
  INMD_LED_CMND_ERASING,
  INMD_LED_CMND_ERASE_DONE,
  INMD_LED_CMND_DFU_MODE,
  INMD_LED_CMND_MV_CAL,
  INMD_LED_CMND_TRI_WVFRM,
  INMD_LED_CMND_REBOOT,
  INMD_LED_CMND_HIBERNATE,
  INMD_LED_CMND_LEADS_ON,
  INMD_LED_CMND_MEMORY_TEST,
  INMD_LED_CMND_COM_BUS_TEST,
  INMD_LED_CMND_CPU_CLOCK_TEST,
  INMD_LED_CMND_FLASH_TEST,
  INMD_LED_CMND_TEST_PASS,
  INMD_LED_CMND_POWER_UP_GOOD,
  INMD_LED_CMND_ACTUAL_DFU,
}INMD_LED_COMMAND;
//*****************************************************************************
// structures
//*****************************************************************************

//*****************************************************************************
// external functions
//*****************************************************************************

//*****************************************************************************
// function declarations
//*****************************************************************************
int iIneedmd_Rcv_cmnd_frame(uint8_t * uiCmnd_Frame, uint16_t uiCmnd_Frame_len);
int iIneedmd_Send_cmnd_frame(uint8_t * uiCmnd_Frame, uint16_t uiCmnd_Frame_len);
int iIneedmd_command_process(void);

#endif //__INEEDMD_COMMAND_PROTOCOL_H__
