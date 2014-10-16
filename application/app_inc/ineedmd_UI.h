/******************************************************************************
*
* ineedmd_UI.h - user interface include file
* Copyright (c) notice
*
******************************************************************************/
#ifndef __INEEDMD_UI_H__
#define __INEEDMD_UI_H__
/******************************************************************************
* includes
******************************************************************************/

/******************************************************************************
* defines
******************************************************************************/
#define INMD_UI_LED      0x01
#define INMD_UI_SPEAKER  0x02
/******************************************************************************
* variables
******************************************************************************/

/******************************************************************************
* external variables
******************************************************************************/

/******************************************************************************
* enums
******************************************************************************/
typedef enum
{
  LED_SEQ_NONE,
  LED_SEQ_OFF,
  LED_SEQ_POWER_ON_BATT_GOOD,
  LED_SEQ_BATT_CHARGING,
  LED_SEQ_BATT_CHARGING_LOW,
  LED_SEQ_POWER_ON_BATT_LOW,
  LED_SEQ_LEAD_LOOSE,
  LED_SEQ_LEAD_GOOD_UPLOADING,
  LED_SEQ_DIG_FLATLINE,
  LED_SEQ_BT_CONNECTED,
  LED_SEQ_BT_ATTEMPTING,
  LED_SEQ_BT_FAILED,
  LED_SEQ_USB_CONNECTED,
  LED_SEQ_USB_FAILED,
  LED_SEQ_DATA_TRANSFER,
  LED_SEQ_TRANSFER_DONE,
  LED_SEQ_STORAGE_WARNING,
  LED_SEQ_ERASING,
  LED_SEQ_ERASE_DONE,
  LED_SEQ_DFU_MODE,
  LED_SEQ_MV_CAL,
  LED_SEQ_TRI_WVFRM,
  LED_SEQ_REBOOT,
  LED_SEQ_HIBERNATE_LOW,
  LED_SEQ_HIBERNATE_MEDIUM,
  LED_SEQ_HIBERNATE_GOOD,
  LED_SEQ_LEADS_ON,
  LED_SEQ_MEMORY_TEST,
  LED_SEQ_COM_BUS_TEST,
  LED_SEQ_CPU_CLOCK_TEST,
  LED_SEQ_FLASH_TEST,
  LED_SEQ_TEST_PASS,
  LED_SEQ_POWER_UP_GOOD,
  LED_SEQ_ACTUAL_DFU
}INMD_UI_LED_SEQ;

typedef enum
{
  SPEAKER_SEQ_NONE,
  SPEAKER_SEQ_FAST,
  SPEAKER_SEQ_ERROR,
  SPEAKER_SEQ_WARNING,
  SPEAKER_SEQ_ATTN
}INMD_UI_SPEAKER_SEQ;

/******************************************************************************
* structures
******************************************************************************/

/******************************************************************************
* external functions
******************************************************************************/

/******************************************************************************
* public functions
******************************************************************************/
INMD_UI_LED_SEQ eIneedmd_present_UI_Process(void); // return's the present UI state for LEDS
ERROR_CODE eIneedmd_UI_process_init(void);  //initalize the UI process
ERROR_CODE eIneedmd_UI_request(uint8_t uiUser_Interface, INMD_UI_LED_SEQ eUI_LED_Sequence, INMD_UI_SPEAKER_SEQ eUI_Spkr_Sequence, bool bDo_immediatly);
ERROR_CODE eIneedmd_UI_process(void); //main UI process function

#endif //__INEEDMD_UI_H__
