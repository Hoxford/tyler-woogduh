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
#define INMD_UI_ELEMENT_NONE       0x0000
#define INMD_UI_ELEMENT_HEART_LED  0x0001
#define INMD_UI_ELEMENT_COMMS_LED  0x0002
#define INMD_UI_ELEMENT_POWER_LED  0x0004
#define INMD_UI_ELEMENT_SPEAKER    0x0010
/******************************************************************************
* variables
******************************************************************************/

/******************************************************************************
* external variables
******************************************************************************/

/******************************************************************************
* enums
******************************************************************************/
//typedef enum INMD_UI_LED_SEQ
//{
//  LED_SEQ_NONE,
//  LED_SEQ_OFF,
//  LED_SEQ_POWER_ON_BATT_GOOD,
//  LED_SEQ_BATT_CHARGING,
//  LED_SEQ_BATT_CHARGING_LOW,
//  LED_SEQ_POWER_ON_BATT_LOW,
//  LED_SEQ_LEAD_LOOSE,
//  LED_SEQ_LEAD_GOOD_UPLOADING,
//  LED_SEQ_DIG_FLATLINE,
//  LED_SEQ_BT_CONNECTED,
//  LED_SEQ_BT_ATTEMPTING,
//  LED_SEQ_BT_FAILED,
//  LED_SEQ_USB_CONNECTED,
//  LED_SEQ_USB_FAILED,
//  LED_SEQ_DATA_TRANSFER,
//  LED_SEQ_TRANSFER_DONE,
//  LED_SEQ_STORAGE_WARNING,
//  LED_SEQ_ERASING,
//  LED_SEQ_ERASE_DONE,
//  LED_SEQ_DFU_MODE,
//  LED_SEQ_MV_CAL,
//  LED_SEQ_TRI_WVFRM,
//  LED_SEQ_REBOOT,
//  LED_SEQ_HIBERNATE_LOW,
//  LED_SEQ_HIBERNATE_MEDIUM,
//  LED_SEQ_HIBERNATE_GOOD,
//  LED_SEQ_LEADS_ON,
//  LED_SEQ_MEMORY_TEST,
//  LED_SEQ_COM_BUS_TEST,
//  LED_SEQ_CPU_CLOCK_TEST,
//  LED_SEQ_FLASH_TEST,
//  LED_SEQ_TEST_PASS,
//  LED_SEQ_POWER_UP_GOOD,
//  LED_SEQ_ACTUAL_DFU
//}INMD_UI_LED_SEQ;

//typedef enum INMD_UI_SPEAKER_SEQ
//{
//  SPEAKER_SEQ_NONE,
//  SPEAKER_SEQ_FAST,
//  SPEAKER_SEQ_ERROR,
//  SPEAKER_SEQ_WARNING,
//  SPEAKER_SEQ_ATTN
//}eINMD_UI_SPEAKER_SEQ;

typedef enum eHEART_LED_UI
{
        HEART_LED_NO_UI,
        HEART_LED_UI_OFF,
        HEART_LED_LEAD_OFF_NO_DATA,
        HEART_LED_LEAD_ON,
        HEART_LED_DIGITAL_FLATLINE,
        HEART_LED_DFU
} eHEART_LED_UI;

typedef enum eCOMMS_LED_UI
{
        COMMS_LED_NO_UI,
        COMMS_LED_UI_OFF,
        COMMS_LED_BLUETOOTH_CONNECTION_SUCESSFUL,
        COMMS_LED_BLUETOOTH_PAIRING,
        COMMS_LED_BLUETOOTH_PAIRING_FAILIED,
        COMMS_LED_USB_CONNECTION_SUCESSFUL,
        COMMS_LED_USB_CONNECTION_FAILED,
        COMMS_LED_DATA_TRANSFERING_FROM_DONGLE,
        COMMS_LED_DATA_TRANSFER_SUCESSFUL,
        COMMS_LED_DONGLE_STORAGE_WARNING,
        COMMS_LED_ERASING_STORED_DATA,
        COMMS_LED_ERASE_COMPLETE,
        COMMS_LED_DFU
} eCOMMS_LED_UI;

typedef enum ePOWER_LED_UI
{
        POWER_LED_NO_UI,
        POWER_LED_UI_OFF,
        POWER_LED_POWER_ON_90to100,
        POWER_LED_POWER_ON_50to90,
        POWER_LED_POWER_ON_25to50,
        POWER_LED_POWER_ON_0to25,
        POWER_LED_POWER_ON_CHARGE_90to100,
        POWER_LED_POWER_ON_CHARGE_50to90,
        POWER_LED_POWER_ON_CHARGE_25to50,
        POWER_LED_POWER_ON_CHARGE_0to25,
        POWER_LED_POWER_ON_BLIP_90to100,
        POWER_LED_POWER_ON_BLIP_50to90,
        POWER_LED_POWER_ON_BLIP_25to50,
        POWER_LED_POWER_ON_BLIP_0to25,
        POWER_LED_DFU
} ePOWER_LED_UI;

typedef enum eALERT_SOUND_UI
{
        ALERT_SOUND_NO_UI,
        ALERT_SOUND_UI_OFF
} eALERT_SOUND_UI;


/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//This structure used to pass in a UI change to the UI task
typedef struct tUI_request
{
  uint16_t         uiUI_element;  //The element to change, it can be any combination of INMD_UI_ELEMENT_HEART_LED, INMD_UI_ELEMENT_COMMS_LED, INMD_UI_ELEMENT_POWER_LED, & INMD_UI_ELEMENT_SPEAKER
  eALERT_SOUND_UI  eAlert_sound;  //The sound to play according to the eALERT_SOUND_UI, can be only one value
  eHEART_LED_UI    eHeart_led_sequence; //Heart LED sequence according to eHEART_LED_UI, can be only one value
  eCOMMS_LED_UI    eComms_led_sequence; //Comms LED sequence according to eCOMMS_LED_UI, can be only one value
  ePOWER_LED_UI    ePower_led_sequence; //Comms LED sequence according to ePOWER_LED_UI, can be only one value
  bool             bPerform_Immediatly;
}tUI_request;

/******************************************************************************
* external functions
******************************************************************************/

/******************************************************************************
* public functions
******************************************************************************/
ERROR_CODE eIneedmd_UI_task_init(void);  //initalize the UI task
ERROR_CODE eIneedmd_UI_params_init(tUI_request * ptUI_Request); //initalize the UI request params
ERROR_CODE eIneedmd_UI_request(tUI_request * ptUI_Request);  //request a UI change

#endif //__INEEDMD_UI_H__
