/*
* ineedmd_UI.c - the user interface application code
* Copyright (c) notice
*
******************************************************************************/
#ifndef __INEEDMD_UI_C__
#define __INEEDMD_UI_C__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "utils_inc/error_codes.h"
#include "utils_inc/proj_debug.h"
#include "utils_inc/clock.h"

#include "drivers_inc/ineedmd_led.h"
#include "drivers_inc/INEEDMD_ADC.h"
#include "drivers_inc/ineedmd_sounder.h"
#include "app_inc/ineedmd_UI.h"

#include "driverlib/systick.h"


//#include <ti/drivers/I2C.h>
//#include <ti/drivers/i2c/I2CTiva.h>
#include "USBMSCD.h"

#include "utils_inc/osal.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
/*  ======== sys defines ========  */

#define UI_DEBUG

/*  ======== UI specific defines ========  */

#define INEEDMD_NO_I2C_PORTS_PLUS_ONE   0x02

#define INEEDMD_I2C_BASE_ADDRESS        I2C0_BASE
#define INEEDMD_I2C_INTERUPT_ADDRESS    INT_I2C0

#define INEEDMD_I2C_GPIO_BLOCK          GPIO_PORTB_BASE
#define INEEDMD_I2C_SYSCTL              SYSCTL_PERIPH_I2C0
#define INEEDMD_I2C_SYSCTL_I2C_GPIO     SYSCTL_PERIPH_GPIOB
#define INEEDMD_I2C_SDA_PIN             GPIO_PIN_3
#define INEEDMD_I2C_SCL_PIN             GPIO_PIN_2

#define INEEDMD_I2C_SPEED_100KHZ        I2C_100kHz
#define INEEDMD_I2C_SPEED_400KHZ        I2C_400kHz


//#define INEEDMD_I2C_ADDRESS_LED_DRIVER  0x1b

#define INEEDMD_LED_OFF                  0x00
#define INEEDMD_COMMS_RED_ENABLE         0x01
#define INEEDMD_COMMS_BLUE_ENABLE        0x02
#define INEEDMD_COMMS_GREEN_ENABLE       0x04
#define INEEDMD_HEART_RED_ENABLE         0x20
#define INEEDMD_HEART_BLUE_ENABLE        0x40
#define INEEDMD_HEART_GREEN_ENABLE       0x80

#define INEEDMD_COMMS_RED_ILEVEL         0x00
#define INEEDMD_COMMS_BLUE_ILEVEL        0x01
#define INEEDMD_COMMS_GREEN_ILEVEL       0x02

#define INEEDMD_HEART_RED_ILEVEL         0x05
#define INEEDMD_HEART_BLUE_ILEVEL        0x06
#define INEEDMD_HEART_GREEN_ILEVEL       0x07

#define INEEDMD_COMMS_RED_PWM            0x08
#define INEEDMD_COMMS_BLUE_PWM           0x09
#define INEEDMD_COMMS_GREEN_PWM          0x0a

#define INEEDMD_HEART_RED_PWM            0x0d
#define INEEDMD_HEART_BLUE_PWM           0x0e
#define INEEDMD_HEART_GREEN_PWM          0x0f

#define INEEDMD_LED_OUTPUT_CONTROL      0x10
#define INEEDMD_LED_STATUS_REGISTER     0x11

#define INEEDMD_LED_TOGGLE_ALL          0x00
#define INEEDMD_LED_PROGRAM_SINGLE      0x20
#define INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE 0x40
#define INEEDMD_LED_PROGRAM_ALL_ILEVEL  0x60
#define INEEDMD_LED_PROGRAM_ALL_PWM     0x80


#define LED_1_SEC                       1000
#define LED_5_SEC                       5000
#define LED_2_SEC                       2000
#define LED_0_5_SEC                     500

#define UI_TIMER_PERIOD_10SEC           10000
#define UI_TIMER_PERIOD_5SEC            5000
#define UI_TIMER_PERIOD_2SEC            2000
#define UI_TIMER_PERIOD_1SEC            1000
#define UI_TIMER_PERIOD_500mSEC         500
#define UI_TIMER_PERIOD_100mSEC         100
#define UI_TIMER_PERIOD_1mSEC           1
#define UI_TIMER_PERIOD_NONE            0

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
uintmax_t uiTimer = 0;
bool      bIsSeqRunning = false;

//used during timer interrupts to get next UI call
eCOMMS_LED_UI eComms_last_sequence = COMMS_LED_NO_UI;
bool bComms_led_on = false;
bool bComms_led_flashing = false;
bool bComms_led_permanent_off = false;

eHEART_LED_UI eHeart_last_sequence = HEART_LED_NO_UI;
bool bHeart_led_on = false;
bool bHeart_led_flashing = false;
bool bHeart_led_permanent_off = false;

ePOWER_LED_UI ePower_last_sequence = POWER_LED_NO_UI;
bool bPower_led_on = false;
bool bPower_led_flashing = false;
bool bPower_led_permanent_off = false;

eALERT_SOUND_UI eAlert_last_sequence = ALERT_SOUND_NO_UI;
bool bSounder_permanent_off = false;

bool bTest_mode_enabled = false;

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
extern Mailbox_Handle tUI_mailbox;
extern Queue_Handle tUI_queue;

/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//Radio setup ready
typedef enum eUser_Interface_task_state
{
  UI_TASK_ERROR,
  UI_TASK_STATE_UNKNOWN,
  UI_TASK_SETUP_NOT_STARTED,
  UI_TASK_LED_DEVICE_SETUP_STARTED,
  UI_TASK_LED_DEVICE_SETUP_COMPLETE,
  UI_TASK_SETUP_SOUNDER_DEVICE_SETUP_STARTED,
  UI_TASK_SETUP_SOUNDER_DEVICE_SETUP_COMPLETE,
  UI_TASK_READY
}eUser_Interface_task_state;

typedef enum
{
        FIELD_SERVICE_NO_FIELD_SERVICE,
        FIELD_SERVICE_OFF_FIELD_SERVICE,
        FIELD_SERVICE_DFU,
        FIELD_SERVICE_ONE_MV_CAL,
        FIELD_SERVICE_TRIANGLE_WAVE,
        FIELD_SERVICE_REBOOT,
        FIELD_SERVICE_LED_SEQ_POWER_UP_GOOD
} FIELD_SERVICE_UI;

eUser_Interface_task_state eUI_Task_State = UI_TASK_STATE_UNKNOWN;

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//typedef struct tStruct_UI_State
//{
//  bool heart_led_on;
//  bool comms_led_on;
//  bool power_led_on;
//  bool sounder_on;
//  NO_UI_ELELMENT heart_led_last_state;
//  NO_UI_ELELMENT comms_led_last_state;
//  NO_UI_ELELMENT power_led_last_state;
//  NO_UI_ELELMENT sounder_last_state;
//}tStruct_UI_State;

typedef struct tStruct_UI_Process
{
  eHEART_LED_UI heart_led_last_UI_request;
  eCOMMS_LED_UI comms_led_last_UI_request;
  ePOWER_LED_UI power_led_last_UI_request;
  eALERT_SOUND_UI sounder_last_UI_request;
  FIELD_SERVICE_UI service_last_UI_request;
  uint32_t       power_led_ui_start_time;
  uint32_t       comms_led_ui_start_time;
  uint32_t       heart_led_ui_start_time;
  uint32_t       sounder_ui_start_time;
  uint32_t       service_ui_start_time;
}tStruct_UI_Process;

typedef struct tStruct_UI_Command_Request
{
  eHEART_LED_UI heart_led_UI_request;
  eCOMMS_LED_UI comms_led_UI_request;
  ePOWER_LED_UI power_led_UI_request;
  eALERT_SOUND_UI sounder_UI_request;
  FIELD_SERVICE_UI service_UI_request;
}tStruct_UI_Command_Request;

extern Clock_Handle tUI_comms_led_timer;
extern Clock_Handle tUI_heart_led_timer;
extern Clock_Handle tUI_power_led_timer;
extern Clock_Handle tUI_sounder_timer;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//uint8_t ineedmd_do_ui_element(NO_UI_ELELMENT ui_element);

/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//uint8_t ineedmd_do_ui_element(NO_UI_ELELMENT ui_element)
//{
//  return 0;
//}
//todo: remove
ERROR_CODE eIneedmd_alert_sound(uint16_t period)
{
	ERROR_CODE eROR;
	tSounder_Request tSounder_Req;

	eROR = eSounder_Request_Params_Init(&tSounder_Req);
	if (period == 0)
	{
		tSounder_Request tSounder_Req;;
		tSounder_Req.eRequest = SOUNDER_REQ_HALT;
		eROR = eSounder_Request(&tSounder_Req);
	}
	else
	{
		tSounder_Request tSounder_Req;;
		tSounder_Req.eRequest = SOUNDER_REQ_FREQUENCY;
		tSounder_Req.uFrequency = period;
		eROR = eSounder_Request(&tSounder_Req);
	}
	return eROR;
}
/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eIneedmd_UI_task_init(void)
{
  ERROR_CODE eEC = ER_OK;
  return eEC;
}

ERROR_CODE eIneedmd_UI_Test_Mode_Enable(bool bTestMode)
{
  ERROR_CODE eEC = ER_OK;
  if(bTestMode && !bTest_mode_enabled)
  {
    vDEBUG("UI Task test mode enabled\n");
    bTest_mode_enabled = bTestMode;
    //turn off any ui sequences
	tUI_request tUI_request_params;
	eEC = eIneedmd_UI_params_init(&tUI_request_params);
	tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED | INMD_UI_ELEMENT_SOUNDER);
	tUI_request_params.eHeart_led_sequence = HEART_LED_1MV_CAL;
	tUI_request_params.eComms_led_sequence = COMMS_LED_1MV_CAL;
	tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
	tUI_request_params.eAlert_sound = ALERT_SOUND_UI_OFF;
	tUI_request_params.bIs_test_pattern = true;
	eEC =  eIneedmd_UI_request(&tUI_request_params);
    //todo:preserve current LED states
  }
  else if(!bTestMode && bTest_mode_enabled)
  {
    vDEBUG("UI Task test mode disabled\n");
    bTest_mode_enabled = bTestMode;
    //turn off any sequences
    tUI_request tUI_request_params;
	eEC = eIneedmd_UI_params_init(&tUI_request_params);
	tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED | INMD_UI_ELEMENT_SOUNDER);
	tUI_request_params.eHeart_led_sequence = HEART_LED_1MV_CAL;
	tUI_request_params.eComms_led_sequence = COMMS_LED_1MV_CAL;
	tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
	tUI_request_params.eAlert_sound = ALERT_SOUND_UI_OFF;
	eEC =  eIneedmd_UI_request(&tUI_request_params);
    //todo:re-enable previous LED states
  }
  return eEC;
}

ERROR_CODE eIneedmd_UI_params_init(tUI_request * ptUI_Request)
{
  ERROR_CODE eEC = ER_FAIL;

  ptUI_Request->bPerform_Immediatly = false;
  ptUI_Request->eAlert_sound        = ALERT_SOUND_NO_UI;
  ptUI_Request->eComms_led_sequence = COMMS_LED_NO_UI;
  ptUI_Request->eHeart_led_sequence = HEART_LED_NO_UI;
  ptUI_Request->ePower_led_sequence = POWER_LED_NO_UI;
  ptUI_Request->uiUI_element        = INMD_UI_ELEMENT_NONE;
  ptUI_Request->bIs_test_pattern        = false;
  if((ptUI_Request->bPerform_Immediatly == false)               &\
     (ptUI_Request->eAlert_sound        == ALERT_SOUND_NO_UI)   &\
     (ptUI_Request->eComms_led_sequence == COMMS_LED_NO_UI)     &\
     (ptUI_Request->eHeart_led_sequence == HEART_LED_NO_UI)     &\
     (ptUI_Request->ePower_led_sequence == POWER_LED_NO_UI)     &\
     (ptUI_Request->uiUI_element        == INMD_UI_ELEMENT_NONE)&\
     (ptUI_Request->bIs_test_pattern        == false))
  {
    eEC = ER_OK;
  }
  else
  {
    eEC = ER_FAIL;
  }

  return eEC;
}



/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
ERROR_CODE eIneedmd_UI_request(tUI_request * ptUI_Request)
{
#define DEBUG_eIneedmd_UI_request
#ifdef DEBUG_eIneedmd_UI_request
  #define  vDEBUG_INMD_UI_REQ  vDEBUG
#else
  #define vDEBUG_INMD_UI_REQ(a)
#endif
  ERROR_CODE eEC = ER_FAIL;
  tUI_request tUI_Msg;
  uint16_t uiElement = ptUI_Request->uiUI_element;
  eHEART_LED_UI eHeart_led_seq = ptUI_Request->eHeart_led_sequence;
  eCOMMS_LED_UI eComms_led_seq = ptUI_Request->eComms_led_sequence;
  ePOWER_LED_UI ePower_led_seq = ptUI_Request->ePower_led_sequence;
  eALERT_SOUND_UI eAlert_sound_seq = ptUI_Request->eAlert_sound;
  bool bIs_request_test = ptUI_Request->bIs_test_pattern;
  // Heart LED sequence request
  //
  if((uiElement & INMD_UI_ELEMENT_HEART_LED) == INMD_UI_ELEMENT_HEART_LED)
  {
    if(bHeart_led_permanent_off == false)
    {
      if(((bTest_mode_enabled == false) && (bIs_request_test == false)) ||\
          ((bTest_mode_enabled == true) && (bIs_request_test == true)))
      {
        eIneedmd_UI_params_init(&tUI_Msg);
        Clock_stop(tUI_heart_led_timer);
        switch (eHeart_led_seq)
        {
          case HEART_LED_NO_UI:
            tUI_Msg.eHeart_led_sequence = HEART_LED_NO_UI;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_HEART_LED;
            eEC = ER_OK;
            break;
          case HEART_LED_UI_OFF:
            tUI_Msg.eHeart_led_sequence = HEART_LED_UI_OFF;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_HEART_LED;
            eEC = ER_OK;
            break;
          case HEART_LED_UI_PERMANENT_OFF:
            Clock_stop(tUI_heart_led_timer);
            eIneedmd_LED_pattern(HEART_LED_OFF);
            bHeart_led_permanent_off = true;
            eEC = ER_OFF;
            break;
          case HEART_LED_LEAD_OFF_NO_DATA:
            tUI_Msg.eHeart_led_sequence = HEART_LED_LEAD_OFF_NO_DATA;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_HEART_LED;
            eEC = ER_OK;
            break;
          case HEART_LED_LEAD_ON:
            tUI_Msg.eHeart_led_sequence = HEART_LED_LEAD_ON;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_HEART_LED;
            eEC = ER_OK;
            break;
          case HEART_LED_DIGITAL_FLATLINE:
            tUI_Msg.eHeart_led_sequence = HEART_LED_DIGITAL_FLATLINE;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_HEART_LED;
            eEC = ER_OK;
            break;
          case HEART_LED_DFU:
            tUI_Msg.eHeart_led_sequence = HEART_LED_DFU;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_HEART_LED;
            eEC = ER_OK;
            break;
          case HEART_LED_TEST_PASS:
            tUI_Msg.eHeart_led_sequence = HEART_LED_TEST_PASS;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_HEART_LED;
            eEC = ER_OK;
            break;
          case HEART_LED_REBOOT:
            tUI_Msg.eHeart_led_sequence = HEART_LED_REBOOT;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_HEART_LED;
            eEC = ER_OK;
            break;
          case HEART_LED_1MV_CAL:
            tUI_Msg.eHeart_led_sequence = HEART_LED_1MV_CAL;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_HEART_LED;
            eEC = ER_OK;
            break;
          case HEART_LED_TRIANGLE:
            tUI_Msg.eHeart_led_sequence = HEART_LED_TRIANGLE;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_HEART_LED;
            eEC = ER_OK;
            break;
          default:
            eEC = ER_FAIL;
            vDEBUG_ASSERT("eIneedmd_UI_request HEART LED, unknown LED request", 0);
        }

        if(eEC == ER_OK)
        {
          Mailbox_post(tUI_mailbox, &tUI_Msg, BIOS_NO_WAIT);
        }else{/*do nothing*/}
      }
    }

  }else{/*do nothing*/}

  // Comms LED sequence request
  //
  if((uiElement & INMD_UI_ELEMENT_COMMS_LED) == INMD_UI_ELEMENT_COMMS_LED)
  {
    if(bComms_led_permanent_off == false)
    {
      if(((bTest_mode_enabled == false) && (bIs_request_test == false)) ||\
        ((bTest_mode_enabled == true) && (bIs_request_test == true)))
      {
        Clock_stop(tUI_comms_led_timer);
        eIneedmd_UI_params_init(&tUI_Msg);
        switch (eComms_led_seq)
        {
          case COMMS_LED_NO_UI:
            tUI_Msg.eComms_led_sequence = COMMS_LED_NO_UI;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_UI_OFF:
            tUI_Msg.eComms_led_sequence = COMMS_LED_UI_OFF;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_UI_PERMANENT_OFF:
            Clock_stop(tUI_comms_led_timer);
            eIneedmd_LED_pattern(COMMS_LED_OFF);
            bComms_led_permanent_off = true;
            eEC = ER_OFF;
            break;
          case COMMS_LED_BLUETOOTH_CONNECTION_SUCESSFUL:
            tUI_Msg.eComms_led_sequence = COMMS_LED_BLUETOOTH_CONNECTION_SUCESSFUL;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_BLUETOOTH_PAIRING:
            tUI_Msg.eComms_led_sequence = COMMS_LED_BLUETOOTH_PAIRING;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_BLUETOOTH_PAIRING_FAILIED:
            tUI_Msg.eComms_led_sequence = COMMS_LED_BLUETOOTH_PAIRING_FAILIED;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_USB_CONNECTION_SUCESSFUL:
            tUI_Msg.eComms_led_sequence = COMMS_LED_USB_CONNECTION_SUCESSFUL;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_USB_CONNECTION_FAILED:
            tUI_Msg.eComms_led_sequence = COMMS_LED_USB_CONNECTION_FAILED;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_DATA_TRANSFERING_FROM_DONGLE:
            tUI_Msg.eComms_led_sequence = COMMS_LED_DATA_TRANSFERING_FROM_DONGLE;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_DATA_TRANSFER_SUCESSFUL:
            tUI_Msg.eComms_led_sequence = COMMS_LED_DATA_TRANSFER_SUCESSFUL;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_DONGLE_STORAGE_WARNING:
            tUI_Msg.eComms_led_sequence = COMMS_LED_DONGLE_STORAGE_WARNING;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_ERASING_STORED_DATA:
            tUI_Msg.eComms_led_sequence = COMMS_LED_ERASING_STORED_DATA;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_ERASE_COMPLETE:
            tUI_Msg.eComms_led_sequence = COMMS_LED_ERASE_COMPLETE;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_DFU:
            tUI_Msg.eComms_led_sequence = COMMS_LED_DFU;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_TEST_PASS:
            tUI_Msg.eComms_led_sequence = COMMS_LED_TEST_PASS;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_REBOOT:
            tUI_Msg.eComms_led_sequence = COMMS_LED_REBOOT;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_1MV_CAL:
            tUI_Msg.eComms_led_sequence = COMMS_LED_1MV_CAL;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          case COMMS_LED_TRIANGLE:
            tUI_Msg.eComms_led_sequence = COMMS_LED_TRIANGLE;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
            eEC = ER_OK;
            break;
          default:
            vDEBUG_INMD_UI_REQ("INMD INMD_UI_ELEMENT_COMMS_LED SYS HALT, unknown LED request");
            while(1){};
        }
        if(eEC == ER_OK)
        {
          Mailbox_post(tUI_mailbox, &tUI_Msg, BIOS_NO_WAIT);
        }
      }
    }
  }else{/*do nothing*/}

  // Power LED sequence request
  //
  if((uiElement & INMD_UI_ELEMENT_POWER_LED) == INMD_UI_ELEMENT_POWER_LED)
  {
    if(bPower_led_permanent_off == false)
    {
      if(((bTest_mode_enabled == false) && (bIs_request_test == false)) ||\
        ((bTest_mode_enabled == true) && (bIs_request_test == true)))
      {
        Clock_stop(tUI_power_led_timer);
        eIneedmd_UI_params_init(&tUI_Msg);
        tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
        switch (ePower_led_seq)
        {
          case POWER_LED_NO_UI:
            tUI_Msg.ePower_led_sequence = POWER_LED_NO_UI;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_UI_OFF:
            tUI_Msg.ePower_led_sequence = POWER_LED_UI_OFF;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_UI_PERMANENT_OFF:
            Clock_stop(tUI_power_led_timer);
            eIneedmd_LED_pattern(POWER_LED_OFF);
            bPower_led_permanent_off = true;
            eEC = ER_OFF;
            break;
          case POWER_LED_POWER_ON_90to100:
            tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_90to100;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_POWER_ON_50to90:
            tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_50to90;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_POWER_ON_25to50:
            tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_25to50;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_POWER_ON_0to25:
            tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_0to25;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_POWER_ON_BLIP_90to100:
            tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_BLIP_90to100;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_POWER_ON_BLIP_50to90:
            tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_BLIP_50to90;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_POWER_ON_BLIP_25to50:
            tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_BLIP_25to50;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_POWER_ON_BLIP_0to25:
            tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_BLIP_0to25;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
          case POWER_LED_POWER_ON_CHARGE_90to100:
            tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_CHARGE_90to100;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_POWER_ON_CHARGE_50to90:
            tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_CHARGE_50to90;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_POWER_ON_CHARGE_25to50:
            tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_CHARGE_25to50;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_POWER_ON_CHARGE_0to25:
            tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_CHARGE_0to25;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_DFU:
            tUI_Msg.ePower_led_sequence = POWER_LED_DFU;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_TEST_PASS:
            tUI_Msg.ePower_led_sequence = POWER_LED_TEST_PASS;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          case POWER_LED_REBOOT:
            tUI_Msg.ePower_led_sequence = POWER_LED_REBOOT;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
            eEC = ER_OK;
            break;
          default:
            vDEBUG_INMD_UI_REQ("INMD INMD_UI_ELEMENT_COMMS_LED SYS HALT, unknown LED request");
            while(1){};
        }
        if(eEC == ER_OK)
        {
          Mailbox_post(tUI_mailbox, &tUI_Msg, BIOS_NO_WAIT);
        }
      }
    }
  }else{/*do nothing*/}

  //sounder
  if((uiElement & INMD_UI_ELEMENT_SOUNDER) == INMD_UI_ELEMENT_SOUNDER)
  {
    if(bSounder_permanent_off == false)
    {
      if(((bTest_mode_enabled == false) && (bIs_request_test == false)) ||\
        ((bTest_mode_enabled == true) && (bIs_request_test == true)))
      {
        Clock_stop(tUI_sounder_timer);
        eIneedmd_UI_params_init(&tUI_Msg);
        switch(eAlert_sound_seq)
        {
          case ALERT_SOUND_NO_UI:
            tUI_Msg.eAlert_sound = ALERT_SOUND_NO_UI;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_SOUNDER;
            eEC = ER_OK;
            break;
          case ALERT_SOUND_UI_OFF:
            tUI_Msg.eAlert_sound = ALERT_SOUND_UI_OFF;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_SOUNDER;
            eEC = ER_OK;
            break;
          case ALERT_SOUND_UI_PERMANENT_OFF:
            eIneedmd_alert_sound(ALERT_NOTE_OFF);
            Clock_stop(tUI_sounder_timer);
            bSounder_permanent_off = true;
            eEC = ER_OFF;
            break;
          case ALERT_SOUND_ALERT:
            tUI_Msg.eAlert_sound = ALERT_SOUND_ALERT;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_SOUNDER;
            eEC = ER_OK;
            break;
          case ALERT_SOUND_HP_ALARM:
            tUI_Msg.eAlert_sound = ALERT_SOUND_HP_ALARM;
            tUI_Msg.uiUI_element = INMD_UI_ELEMENT_SOUNDER;
            eEC = ER_OK;
            break;
          default:
            vDEBUG_INMD_UI_REQ("INMD INMD_UI_ELEMENT_SOUNDER SYS HALT, unknown Sounder request");
            while(1){};
        }
        if(eEC == ER_OK)
        {
          Mailbox_post(tUI_mailbox, &tUI_Msg, BIOS_NO_WAIT);
        }
      }
    }
  }
  else
  {
    //nothing
  }
  return eEC;

#undef vDEBUG_INMD_UI_REQ
}

void vIneedmd_UI_Comms_led_timer_INT_Service(UArg a0)
{
  eCOMMS_LED_UI next_sequence = COMMS_LED_UI_OFF;
  if(bComms_led_flashing)
  {
    next_sequence = eComms_last_sequence;
  }
  tUI_request tUI_task_msg;
  eIneedmd_UI_params_init(&tUI_task_msg);
  tUI_task_msg.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
  tUI_task_msg.eComms_led_sequence = next_sequence;
  tUI_task_msg.bIs_test_pattern = bTest_mode_enabled;
  eIneedmd_UI_request(&tUI_task_msg);
  //  eIneedmd_LED_pattern(COMMS_LED_OFF);
  return;
}

void vIneedmd_UI_Heart_led_timer_INT_Service(UArg a0)
{
  eHEART_LED_UI next_sequence = HEART_LED_UI_OFF;

  if (bHeart_led_flashing)
  {
    next_sequence = eHeart_last_sequence;
  }
  tUI_request tUI_task_msg;
  eIneedmd_UI_params_init(&tUI_task_msg);
  tUI_task_msg.uiUI_element = INMD_UI_ELEMENT_HEART_LED;
  tUI_task_msg.eHeart_led_sequence = next_sequence;
  tUI_task_msg.bIs_test_pattern = bTest_mode_enabled;
  eIneedmd_UI_request(&tUI_task_msg);
  return;
}

void vIneedmd_UI_Power_led_timer_INT_Service(UArg a0)
{
  ePOWER_LED_UI next_sequence = POWER_LED_UI_OFF;
  //check current sequence to get next sequence
  if (bPower_led_flashing)
  {
    next_sequence = ePower_last_sequence;
  }
  //post next led state
  tUI_request tUI_task_msg;
  eIneedmd_UI_params_init(&tUI_task_msg);
  tUI_task_msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
  tUI_task_msg.ePower_led_sequence = next_sequence;
  tUI_task_msg.bIs_test_pattern = bTest_mode_enabled;
  eIneedmd_UI_request(&tUI_task_msg);
  return;
}

void vIneedmd_UI_Sounder_timer_INT_Service(UArg a0)
{
  //post next led state
  tUI_request tUI_task_msg;
  eIneedmd_UI_params_init(&tUI_task_msg);
  tUI_task_msg.uiUI_element = INMD_UI_ELEMENT_SOUNDER;
  tUI_task_msg.eAlert_sound = eAlert_last_sequence;
  tUI_task_msg.bIs_test_pattern = bTest_mode_enabled;
  eIneedmd_UI_request(&tUI_task_msg);
  return;
}


void vUI_power_led_status_check_timer(void)
{

 #define fully_charges 130
 #define half_charged 110
 #define flat_charged 90

  typedef enum eCHARGE_STATE
  {
          BATTERY_LESS_25,
          BATTERY_25_50,
          BATTERY_50_90,
          BATTERY_MORE_90
  } eCHARGE_STATE;


  uint8_t present_battery_voltage = 0;
  ERROR_CODE eUSB_Connection_State = ER_NOT_CONNECTED;
  ADC_REQUEST information_request;
  eCHARGE_STATE eBattery_Charge_State;
  tUI_request tUI_request_params;

  eUSB_Connection_State = eUSB_MSCD_check_physical_connection();

  information_request.eRequest_ID = ADC_REQUEST_DEVICE_SUPPLY_VOLTAGE;
  information_request.uiDevice_supp_voltage = &present_battery_voltage;
  eADC_Request(&information_request);

  if (present_battery_voltage >= fully_charges)
  {
    eBattery_Charge_State = BATTERY_MORE_90;
  }
  else if (present_battery_voltage >= half_charged)
  {
    eBattery_Charge_State = BATTERY_50_90;
  }
  else if (present_battery_voltage >= flat_charged)
  {
    eBattery_Charge_State = BATTERY_25_50;
  }
  else
  {
    eBattery_Charge_State = BATTERY_LESS_25;
  }

  eIneedmd_UI_params_init(&tUI_request_params);
  tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_POWER_LED);

  switch (eBattery_Charge_State)
  {
    case BATTERY_MORE_90:
      if ( eUSB_Connection_State == ER_NOT_CONNECTED )
      {
          tUI_request_params.ePower_led_sequence = POWER_LED_POWER_ON_BLIP_90to100;
      }
      else
      {
        tUI_request_params.ePower_led_sequence = POWER_LED_POWER_ON_CHARGE_90to100;
      }
      break;
    case BATTERY_50_90:
      if ( eUSB_Connection_State == ER_NOT_CONNECTED )
      {
          tUI_request_params.ePower_led_sequence = POWER_LED_POWER_ON_BLIP_50to90;
      }
      else
      {
        tUI_request_params.ePower_led_sequence =  POWER_LED_POWER_ON_CHARGE_50to90;
      }
      break;
    case BATTERY_25_50:
      if ( eUSB_Connection_State == ER_NOT_CONNECTED )
      {
          tUI_request_params.ePower_led_sequence = POWER_LED_POWER_ON_BLIP_25to50 ;
      }
      else
      {
        tUI_request_params.ePower_led_sequence = POWER_LED_POWER_ON_CHARGE_25to50;
      }
      break;
    case BATTERY_LESS_25:
      if ( eUSB_Connection_State == ER_NOT_CONNECTED )
      {
          tUI_request_params.ePower_led_sequence = POWER_LED_POWER_ON_BLIP_0to25;
      }
      else
      {
        tUI_request_params.ePower_led_sequence = POWER_LED_POWER_ON_CHARGE_0to25;
      }
      break;
    default:
      tUI_request_params.ePower_led_sequence = POWER_LED_POWER_ON_BLIP_0to25;
      break;
  }
  eIneedmd_UI_request(&tUI_request_params);

  return;

}


/******************************************************************************
* name: eIneedmd_UI_process
* description:
* param description:
* return value description:
******************************************************************************/
void vIneedmd_UI_task(UArg a0, UArg a1)
{
#define DEBUG_eIneedmd_UI_process
#ifdef DEBUG_eIneedmd_UI_process
  #define  vDEBUG_INMD_UI_PROC  vDEBUG
#else
  #define vDEBUG_INMD_UI_PROC(a)
#endif

  ERROR_CODE eEC = ER_FAIL;

  tUI_request tUI_task_msg;

  uint32_t uiTimer_period = 0;
  int16_t iComms_led_flash_count = 0;
  int16_t iPower_led_flash_count = 0;
  int16_t iHeart_led_flash_count = 0;
  uint16_t uiSounder_note_count = 0;

  uint16_t uiMsg_size = sizeof(tUI_request);
  uint16_t uiMbox_size = 0;
  uiMbox_size = Mailbox_getMsgSize(tUI_mailbox);
  if(uiMsg_size != uiMbox_size)
  {
    vDEBUG("vIneedmd_UI_task SYS HALT, invalid mailbox msg size!");
    while(1){};
  }else{/*do nothing*/}

  eUI_Task_State = UI_TASK_SETUP_NOT_STARTED;

  vDEBUG("enter taskineedmdUI()");

  eEC = eIneedmd_LED_driver_setup();
  if (eEC == ER_OK)
  {
    eUI_Task_State = UI_TASK_LED_DEVICE_SETUP_COMPLETE;
  }
  else
  {
    eUI_Task_State = UI_TASK_ERROR;
  }

//  tSounder_Req.eRequest = SOUNDER_REQ_TEST;
//  eEC = eSounder_Request(&tSounder_Req);


  if(tUI_mailbox != NULL)
  {
    eUI_Task_State = UI_TASK_READY;
  }
  else
  {
    eUI_Task_State = UI_TASK_ERROR;
  }

  eIneedmd_LED_pattern(POWER_LED_OFF);
  eIneedmd_LED_pattern(COMMS_LED_OFF);
  eIneedmd_LED_pattern(POWER_LED_OFF);
//todo: actual startup checks
//  eClock_get_total_runtime(&uiCurrent_tick);
  //Main UI task loop
  //
  while(1)
  {
    if(Mailbox_pend(tUI_mailbox, &tUI_task_msg, BIOS_WAIT_FOREVER) == true)
    {
      //Power LED UI
      //
      if ((tUI_task_msg.uiUI_element & INMD_UI_ELEMENT_POWER_LED) == INMD_UI_ELEMENT_POWER_LED)
      {
        //init control variables
        uiTimer_period = UI_TIMER_PERIOD_NONE;

        switch(tUI_task_msg.ePower_led_sequence)
        {
          case POWER_LED_NO_UI:
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            bPower_led_on = false;
            bPower_led_flashing = false;
            iPower_led_flash_count = 0;
            break;
          case POWER_LED_UI_OFF:
            eIneedmd_LED_pattern(POWER_LED_OFF);
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            bPower_led_on = false;
            ePower_last_sequence = POWER_LED_UI_OFF;
            bPower_led_flashing = false;
            iPower_led_flash_count = 0;
            break;
          case POWER_LED_POWER_ON_90to100:
            eIneedmd_LED_pattern(POWER_LED_GREEN);
            uiTimer_period = UI_TIMER_PERIOD_5SEC;
            bPower_led_on = true;
            ePower_last_sequence = POWER_LED_POWER_ON_90to100;
            bPower_led_flashing = false;
            iPower_led_flash_count = 0;
            break;
          case POWER_LED_POWER_ON_50to90:
            eIneedmd_LED_pattern(POWER_LED_YELLOW);
            uiTimer_period = UI_TIMER_PERIOD_5SEC;
            bPower_led_on = true;
            ePower_last_sequence = POWER_LED_POWER_ON_50to90;
            bPower_led_flashing = false;
            iPower_led_flash_count = 0;
            break;
          case POWER_LED_POWER_ON_25to50:
            eIneedmd_LED_pattern(POWER_LED_ORANGE);
            uiTimer_period = UI_TIMER_PERIOD_5SEC;
            bPower_led_on = true;
            ePower_last_sequence = POWER_LED_POWER_ON_25to50;
            bPower_led_flashing = false;
            iPower_led_flash_count = 0;
            break;
          case POWER_LED_POWER_ON_0to25:
            eIneedmd_LED_pattern(POWER_LED_RED);
            uiTimer_period = UI_TIMER_PERIOD_5SEC;
            bPower_led_on = true;
            ePower_last_sequence = POWER_LED_POWER_ON_0to25;
            bPower_led_flashing = false;
            iPower_led_flash_count = 0;
            break;
          case POWER_LED_POWER_ON_CHARGE_90to100:
            //if entering pattern for first time, reset state flag to start pattern immediately
            if (ePower_last_sequence != POWER_LED_POWER_ON_CHARGE_90to100)
            {
              bPower_led_on = false;
            }
            bPower_led_flashing = true;
            ePower_last_sequence = POWER_LED_POWER_ON_CHARGE_90to100;
            iPower_led_flash_count = 0; //not used since pattern does not expire - keep at 0
            if (bPower_led_on == false)
            {
              eIneedmd_LED_pattern(POWER_LED_GREEN);
              uiTimer_period = UI_TIMER_PERIOD_1SEC;
              bPower_led_on = true;
            }
            else
            {
              eIneedmd_LED_pattern(POWER_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_1SEC;
              bPower_led_on = false;
            }
            break;
          case POWER_LED_POWER_ON_CHARGE_50to90:
            //if entering pattern for first time, reset state flag to start pattern immediately
            if (ePower_last_sequence != POWER_LED_POWER_ON_CHARGE_50to90)
            {
              bPower_led_on = false;
            }
            bPower_led_flashing = true;
            ePower_last_sequence = POWER_LED_POWER_ON_CHARGE_50to90;
            iPower_led_flash_count = 0;
            if (bPower_led_on == false)
            {
              eIneedmd_LED_pattern(POWER_LED_YELLOW);
              uiTimer_period = UI_TIMER_PERIOD_1SEC;
              bPower_led_on = true;
            }
            else
            {
              eIneedmd_LED_pattern(POWER_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_1SEC;
              bPower_led_on = false;
            }
            break;
          case POWER_LED_POWER_ON_CHARGE_25to50:
            //if entering pattern for first time, reset state flag to start pattern immediately
            if (ePower_last_sequence != POWER_LED_POWER_ON_CHARGE_25to50)
            {
              bPower_led_on = false;
            }
            bPower_led_flashing = true;
            ePower_last_sequence = POWER_LED_POWER_ON_CHARGE_25to50;
            iPower_led_flash_count = 0;
            if (bPower_led_on == false)
            {
              eIneedmd_LED_pattern(POWER_LED_ORANGE);
              uiTimer_period = UI_TIMER_PERIOD_1SEC;
              bPower_led_on = true;
            }
            else
            {
              eIneedmd_LED_pattern(POWER_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_1SEC;
              bPower_led_on = false;
            }
            break;
          case POWER_LED_POWER_ON_CHARGE_0to25:
            //if entering pattern for first time, reset state flag to start pattern immediately
            if (ePower_last_sequence != POWER_LED_POWER_ON_CHARGE_0to25)
            {
              bPower_led_on = false;
            }
            bPower_led_flashing = true;
            ePower_last_sequence = POWER_LED_POWER_ON_CHARGE_0to25;
            iPower_led_flash_count = 0;
            if (bPower_led_on == false)
            {
              eIneedmd_LED_pattern(POWER_LED_RED);
              uiTimer_period = UI_TIMER_PERIOD_1SEC;
              bPower_led_on = true;
            }
            else
            {
              eIneedmd_LED_pattern(POWER_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_1SEC;
              bPower_led_on = false;
            }
            break;
          case POWER_LED_POWER_ON_BLIP_90to100:
            eIneedmd_LED_pattern(POWER_LED_GREEN);
            uiTimer_period = UI_TIMER_PERIOD_1SEC;
            bPower_led_on = false;
            ePower_last_sequence =  POWER_LED_POWER_ON_BLIP_90to100;
            bPower_led_flashing = false;
            iPower_led_flash_count = 0;
            break;
          case POWER_LED_POWER_ON_BLIP_50to90:
            eIneedmd_LED_pattern(POWER_LED_YELLOW);
            uiTimer_period = UI_TIMER_PERIOD_1SEC;
            bPower_led_on = false;
            ePower_last_sequence =  POWER_LED_POWER_ON_BLIP_50to90;
            bPower_led_flashing = false;
            iPower_led_flash_count = 0;
            break;
          case POWER_LED_POWER_ON_BLIP_25to50:
            eIneedmd_LED_pattern(POWER_LED_ORANGE);
            uiTimer_period = UI_TIMER_PERIOD_1SEC;
            bPower_led_on = false;
            ePower_last_sequence =  POWER_LED_POWER_ON_BLIP_25to50;
            bPower_led_flashing = false;
            iPower_led_flash_count = 0;
            break;
          case POWER_LED_POWER_ON_BLIP_0to25:
            eIneedmd_LED_pattern(POWER_LED_RED);
            uiTimer_period = UI_TIMER_PERIOD_1SEC;
            bPower_led_on = false;
            ePower_last_sequence =  POWER_LED_POWER_ON_BLIP_0to25;
            bPower_led_flashing = false;
            iPower_led_flash_count = 0;
            break;
          case POWER_LED_DFU:
            eIneedmd_LED_pattern(POWER_LED_PINK);
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            bPower_led_on = true;
            ePower_last_sequence =  POWER_LED_DFU;
            bPower_led_flashing = false;
            iPower_led_flash_count = 0;
            break;
          case POWER_LED_TEST_PASS:
            if (ePower_last_sequence != POWER_LED_TEST_PASS)
            {
              bPower_led_on = false;
              iPower_led_flash_count = LED_TEST_PASS_DURATION;
            }
            //if pattern duration is over
            if (iPower_led_flash_count == 0)
            {
              //turn power led off - same effect as power led off request
              eIneedmd_LED_pattern(POWER_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_NONE;
              bPower_led_on = false;
              ePower_last_sequence = POWER_LED_UI_OFF;
              bPower_led_flashing = false;
              iPower_led_flash_count = 0;
            }
            else
            {
              //pattern not over, keep flashing
              iPower_led_flash_count--;
              bPower_led_flashing = true;
              ePower_last_sequence = POWER_LED_TEST_PASS;
              if (bPower_led_on == false)
              {
                eIneedmd_LED_pattern(POWER_LED_GREEN);
                uiTimer_period = UI_TIMER_PERIOD_500mSEC;;
                bPower_led_on = true;
              }
              else
              {
                eIneedmd_LED_pattern(POWER_LED_OFF);
                uiTimer_period = UI_TIMER_PERIOD_500mSEC;;
                bPower_led_on = false;
              }
            }
            break;
          case POWER_LED_REBOOT:
            eIneedmd_LED_pattern(POWER_LED_RED);
            uiTimer_period = UI_TIMER_PERIOD_1SEC;
            bPower_led_on = true;
            ePower_last_sequence =  POWER_LED_DFU;
            bPower_led_flashing = false;
            iPower_led_flash_count = 0;
            break;
          default:
            eIneedmd_LED_pattern(UI_ALL_OFF);
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            bPower_led_on = false;
            ePower_last_sequence =  POWER_LED_UI_OFF;
            bPower_led_flashing = false;

            bComms_led_on = false;
            bComms_led_flashing = false;
            eComms_last_sequence =  COMMS_LED_UI_OFF;

            bHeart_led_on = false;
            eHeart_last_sequence =  HEART_LED_UI_OFF;
            bHeart_led_flashing = false;

            iPower_led_flash_count = 0;
            break;
        }

        //Check if the timer should be stopped
        if(uiTimer_period == UI_TIMER_PERIOD_NONE)
        {
          //stop the timer
          Clock_stop(tUI_power_led_timer);
        }
        else
        {
          //set the timer to the new period and start it
          Clock_stop(tUI_power_led_timer);
          Clock_setTimeout(tUI_power_led_timer, uiTimer_period);
          Clock_start(tUI_power_led_timer);
        }
      }

      //Comm LED UI
      //
      if ((tUI_task_msg.uiUI_element & INMD_UI_ELEMENT_COMMS_LED) == INMD_UI_ELEMENT_COMMS_LED)
      {
        //init control variables
        uiTimer_period = UI_TIMER_PERIOD_NONE;

        switch(tUI_task_msg.eComms_led_sequence)
        {
          case COMMS_LED_NO_UI:
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            bComms_led_on = false;
            eComms_last_sequence = COMMS_LED_NO_UI;
            bComms_led_flashing = false;
            iComms_led_flash_count = 0;
            break;
          case COMMS_LED_UI_OFF:
            eIneedmd_LED_pattern(COMMS_LED_OFF);
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            bComms_led_on = false;
            eComms_last_sequence = COMMS_LED_UI_OFF;
            bComms_led_flashing = false;
            iComms_led_flash_count = 0;
            break;
          case COMMS_LED_BLUETOOTH_CONNECTION_SUCESSFUL:
            eIneedmd_LED_pattern(COMMS_LED_BLUE);
            uiTimer_period = UI_TIMER_PERIOD_5SEC;
            bComms_led_on = true;
            eComms_last_sequence = COMMS_LED_BLUETOOTH_CONNECTION_SUCESSFUL;
            bComms_led_flashing = false;
            iComms_led_flash_count = 0;
            break;
          case COMMS_LED_BLUETOOTH_PAIRING:
            if (eComms_last_sequence != COMMS_LED_BLUETOOTH_PAIRING)
            {
              bComms_led_on = false;
            }
            bComms_led_flashing = true;
            eComms_last_sequence = COMMS_LED_BLUETOOTH_PAIRING;
            iComms_led_flash_count = 0;
            if (bComms_led_on == false)
            {
              eIneedmd_LED_pattern(COMMS_LED_BLUE);
              uiTimer_period = UI_TIMER_PERIOD_1SEC;
              bComms_led_on = true;
            }
            else
            {
              eIneedmd_LED_pattern(COMMS_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_1SEC;
              bComms_led_on = false;
            }
            break;
          case COMMS_LED_BLUETOOTH_PAIRING_FAILIED:
            eIneedmd_LED_pattern(COMMS_LED_ORANGE);
            uiTimer_period = UI_TIMER_PERIOD_5SEC;
            bComms_led_on = true;
            eComms_last_sequence = COMMS_LED_BLUETOOTH_PAIRING_FAILIED;
            bComms_led_flashing = false;
            iComms_led_flash_count = 0;
            break;
          case COMMS_LED_USB_CONNECTION_SUCESSFUL:
            eIneedmd_LED_pattern(COMMS_LED_BLUE);
            uiTimer_period = UI_TIMER_PERIOD_5SEC;
            bComms_led_on = true;
            eComms_last_sequence = COMMS_LED_USB_CONNECTION_SUCESSFUL;
            bComms_led_flashing = false;
            iComms_led_flash_count = 0;
            break;
          case COMMS_LED_USB_CONNECTION_FAILED:
            eIneedmd_LED_pattern(COMMS_LED_ORANGE);
            uiTimer_period = UI_TIMER_PERIOD_5SEC;
            bComms_led_on = true;
            eComms_last_sequence = COMMS_LED_USB_CONNECTION_FAILED;
            bComms_led_flashing = false;
            iComms_led_flash_count = 0;
            break;
          case COMMS_LED_DATA_TRANSFERING_FROM_DONGLE:
            if (eComms_last_sequence != COMMS_LED_DATA_TRANSFERING_FROM_DONGLE)
            {
              bComms_led_on = false;
            }
            bComms_led_flashing = true;
            eComms_last_sequence = COMMS_LED_DATA_TRANSFERING_FROM_DONGLE;
            iComms_led_flash_count = 0;
            if (bComms_led_on == false)
            {
              eIneedmd_LED_pattern(COMMS_LED_PURPLE);
              uiTimer_period = UI_TIMER_PERIOD_1SEC;
              bComms_led_on = true;
            }
            else
            {
              eIneedmd_LED_pattern(COMMS_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_1SEC;
              bComms_led_on = false;
            }
            break;
          case COMMS_LED_DATA_TRANSFER_SUCESSFUL:
            eIneedmd_LED_pattern(COMMS_LED_PURPLE);
            uiTimer_period = UI_TIMER_PERIOD_5SEC;
            bComms_led_on = true;
            eComms_last_sequence = COMMS_LED_DATA_TRANSFER_SUCESSFUL;
            bComms_led_flashing = false;
            iComms_led_flash_count = 0;
            break;
          case COMMS_LED_DONGLE_STORAGE_WARNING:
            eIneedmd_LED_pattern(COMMS_LED_ORANGE);
            uiTimer_period = UI_TIMER_PERIOD_10SEC;
            bComms_led_on = true;
            eComms_last_sequence = COMMS_LED_DONGLE_STORAGE_WARNING;
            bComms_led_flashing = false;
            iComms_led_flash_count = 0;
            break;
          case COMMS_LED_ERASING_STORED_DATA:
            if (eComms_last_sequence != COMMS_LED_ERASING_STORED_DATA)
            {
              bComms_led_on = false;
            }
            bComms_led_flashing = true;
            eComms_last_sequence = COMMS_LED_ERASING_STORED_DATA;
            iComms_led_flash_count = 0;
            if (bComms_led_on == false)
            {
              eIneedmd_LED_pattern(COMMS_LED_ORANGE);
              uiTimer_period = UI_TIMER_PERIOD_2SEC;
              bComms_led_on = true;
            }
            else
            {
              eIneedmd_LED_pattern(COMMS_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_2SEC;
              bComms_led_on = false;
            }
            break;
          case COMMS_LED_ERASE_COMPLETE:
            eIneedmd_LED_pattern(COMMS_LED_GREEN);
            uiTimer_period = UI_TIMER_PERIOD_2SEC;
            bComms_led_on = true;
            eComms_last_sequence = COMMS_LED_ERASE_COMPLETE;
            bComms_led_flashing = false;
            iComms_led_flash_count = 0;
            break;
          case COMMS_LED_DFU:
            eIneedmd_LED_pattern(COMMS_LED_PINK);
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            bComms_led_on = true;
            eComms_last_sequence = COMMS_LED_DFU;
            bComms_led_flashing = false;
            iComms_led_flash_count = 0;
            break;
          case COMMS_LED_TEST_PASS:
            if (eComms_last_sequence != COMMS_LED_TEST_PASS)    //if first time running this sequence
            {
              bComms_led_on = false;
              iComms_led_flash_count = LED_TEST_PASS_DURATION;
            }
            if (iComms_led_flash_count == 0)                            //if led sequence duration expired turn off led, reset states
            {
              eIneedmd_LED_pattern(COMMS_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_NONE;
              bComms_led_on = false;
              eComms_last_sequence = COMMS_LED_UI_OFF;
              bComms_led_flashing = false;
              iComms_led_flash_count = 0;
            }
            else                                                        //else continue with led sequence
            {
              iComms_led_flash_count--;
              bComms_led_flashing = true;
              eComms_last_sequence = COMMS_LED_TEST_PASS;
              if (bComms_led_on == false)
              {
                eIneedmd_LED_pattern(COMMS_LED_GREEN);
                uiTimer_period = UI_TIMER_PERIOD_500mSEC;
                bComms_led_on = true;
              }
              else
              {
                eIneedmd_LED_pattern(COMMS_LED_OFF);
                uiTimer_period = UI_TIMER_PERIOD_500mSEC;
                bComms_led_on = false;
              }
            }
            break;
          case COMMS_LED_REBOOT:
            eIneedmd_LED_pattern(COMMS_LED_RED);
            uiTimer_period = UI_TIMER_PERIOD_1SEC;
            bComms_led_on = true;
            eComms_last_sequence = COMMS_LED_REBOOT;
            bComms_led_flashing = false;
            iComms_led_flash_count = 0;
            break;
          case COMMS_LED_1MV_CAL:
            if (eComms_last_sequence != COMMS_LED_1MV_CAL)
            {
             bComms_led_on = false;
            }
            bComms_led_flashing = true;
            eComms_last_sequence = COMMS_LED_1MV_CAL;
            iComms_led_flash_count = 0;
            if (bComms_led_on == false)
            {
             eIneedmd_LED_pattern(COMMS_LED_PURPLE);
             uiTimer_period = UI_TIMER_PERIOD_500mSEC;
             bComms_led_on = true;
            }
            else
            {
             eIneedmd_LED_pattern(COMMS_LED_OFF);
             uiTimer_period = UI_TIMER_PERIOD_500mSEC;
             bComms_led_on = false;
            }
            break;
          case COMMS_LED_TRIANGLE:
            if (eComms_last_sequence != COMMS_LED_TRIANGLE)
            {
            bComms_led_on = false;
            }
            bComms_led_flashing = true;
            eComms_last_sequence = COMMS_LED_TRIANGLE;
            iComms_led_flash_count = 0;
            if (bComms_led_on == false)
            {
            eIneedmd_LED_pattern(COMMS_LED_ORANGE);
            uiTimer_period = UI_TIMER_PERIOD_500mSEC;
            bComms_led_on = true;
            }
            else
            {
            eIneedmd_LED_pattern(COMMS_LED_OFF);
            uiTimer_period = UI_TIMER_PERIOD_500mSEC;
            bComms_led_on = false;
            }
            break;
          default:
            eIneedmd_LED_pattern(UI_ALL_OFF);
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            bPower_led_on = false;
            ePower_last_sequence =  POWER_LED_UI_OFF;
            bPower_led_flashing = false;

            bComms_led_on = false;
            bComms_led_flashing = false;
            eComms_last_sequence =  COMMS_LED_UI_OFF;

            bHeart_led_on = false;
            eHeart_last_sequence =  HEART_LED_UI_OFF;
            bHeart_led_flashing = false;
            iComms_led_flash_count = 0;
            break;
        }

        //Check if the timer should be stopped
        if(uiTimer_period == UI_TIMER_PERIOD_NONE)
        {
          //stop the timer
          Clock_stop(tUI_comms_led_timer);
        }
        else
        {
          //set the timer to the new period and start it
          Clock_stop(tUI_comms_led_timer);
          Clock_setTimeout(tUI_comms_led_timer, uiTimer_period);
          Clock_start(tUI_comms_led_timer);
        }
      }

      //Heart LED UI
      //
      if((tUI_task_msg.uiUI_element & INMD_UI_ELEMENT_HEART_LED) == INMD_UI_ELEMENT_HEART_LED)
      {
        //init control variables
        uiTimer_period = UI_TIMER_PERIOD_NONE;

        switch(tUI_task_msg.eHeart_led_sequence)
        {
          case HEART_LED_NO_UI:
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            bHeart_led_on = false;
            eHeart_last_sequence = HEART_LED_NO_UI;
            bHeart_led_flashing = false;
            iHeart_led_flash_count = 0;
            break;
          case HEART_LED_UI_OFF:
            eIneedmd_LED_pattern(HEART_LED_OFF);
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            bHeart_led_on = false;
            eHeart_last_sequence = HEART_LED_UI_OFF;
            bHeart_led_flashing = false;
            iHeart_led_flash_count = 0;
            break;
          case HEART_LED_LEAD_OFF_NO_DATA:
            //if entering pattern for first time, reset state flag to start pattern immediately
            if (eHeart_last_sequence != HEART_LED_LEAD_OFF_NO_DATA)
            {
              bHeart_led_on = false;
            }
            bHeart_led_flashing = true;
            eHeart_last_sequence = HEART_LED_LEAD_OFF_NO_DATA;
            iHeart_led_flash_count = 0;
            if (bHeart_led_on == false)
            {
              eIneedmd_LED_pattern(HEART_LED_ORANGE);
              uiTimer_period = UI_TIMER_PERIOD_2SEC;
              bHeart_led_on = true;
            }
            else
            {
              eIneedmd_LED_pattern(HEART_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_2SEC;
              bHeart_led_on = false;
            }
            break;
          case HEART_LED_LEAD_ON:
            //if entering pattern for first time, reset state flag to start pattern immediately
            if (eHeart_last_sequence != HEART_LED_LEAD_ON)
            {
              bHeart_led_on = false;
            }
            bHeart_led_flashing = true;
            eHeart_last_sequence = HEART_LED_LEAD_ON;
            iHeart_led_flash_count = 0;
            if (bHeart_led_on == false)
            {
              eIneedmd_LED_pattern(HEART_LED_GREEN);
              uiTimer_period = UI_TIMER_PERIOD_2SEC;
              bHeart_led_on = true;
            }
            else
            {
              eIneedmd_LED_pattern(HEART_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_2SEC;
              bHeart_led_on = false;
            }
            break;
          case HEART_LED_DIGITAL_FLATLINE:
            //if entering pattern for first time, reset state flag to start pattern immediately
            if (eHeart_last_sequence != HEART_LED_DIGITAL_FLATLINE)
            {
              bHeart_led_on = false;
            }
            bHeart_led_flashing = true;
            eHeart_last_sequence = HEART_LED_DIGITAL_FLATLINE;
            iHeart_led_flash_count = 0;
            if (bHeart_led_on == false)
            {
              eIneedmd_LED_pattern(HEART_LED_RED);
              uiTimer_period = UI_TIMER_PERIOD_500mSEC;
              bHeart_led_on = true;
            }
            else
            {
              eIneedmd_LED_pattern(HEART_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_500mSEC;
              bHeart_led_on = false;
            }
            break;
          case HEART_LED_DFU:
            eIneedmd_LED_pattern(HEART_LED_PINK);
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            bHeart_led_on = true;
            eHeart_last_sequence = HEART_LED_DFU;
            bHeart_led_flashing = false;
            iHeart_led_flash_count = 0;
            break;
          case HEART_LED_TEST_PASS:
            if (eHeart_last_sequence != HEART_LED_TEST_PASS)    //if first time running this sequence
            {
             bHeart_led_on = false;
             iHeart_led_flash_count = LED_TEST_PASS_DURATION;
            }
            if (iHeart_led_flash_count == 0)                            //if led sequence duration expired turn off led, reset states
            {
             eIneedmd_LED_pattern(HEART_LED_OFF);
             uiTimer_period = UI_TIMER_PERIOD_NONE;
             bHeart_led_on = false;
             eHeart_last_sequence = HEART_LED_UI_OFF;
             bHeart_led_flashing = false;
             iHeart_led_flash_count = 0;
            }
            else                                                        //else continue with led sequence
            {
             iHeart_led_flash_count--;
             bHeart_led_flashing = true;
             eHeart_last_sequence = HEART_LED_TEST_PASS;
             if (bHeart_led_on == false)
             {
               eIneedmd_LED_pattern(HEART_LED_GREEN);
               uiTimer_period = UI_TIMER_PERIOD_500mSEC;
               bHeart_led_on = true;
             }
             else
             {
               eIneedmd_LED_pattern(HEART_LED_OFF);
               uiTimer_period = UI_TIMER_PERIOD_500mSEC;
               bHeart_led_on = false;
             }
            }
            break;
          case HEART_LED_REBOOT:
            eIneedmd_LED_pattern(HEART_LED_RED);
            uiTimer_period = UI_TIMER_PERIOD_1SEC;
            bHeart_led_on = true;
            eHeart_last_sequence = HEART_LED_REBOOT;
            bHeart_led_flashing = false;
            iHeart_led_flash_count = 0;
            break;
          case HEART_LED_1MV_CAL:
            if (eHeart_last_sequence != HEART_LED_1MV_CAL)
            {
              bHeart_led_on = false;
            }
            bHeart_led_flashing = true;
            eHeart_last_sequence = HEART_LED_1MV_CAL;
            iHeart_led_flash_count = 0;
            if (bHeart_led_on == false)
            {
              eIneedmd_LED_pattern(HEART_LED_PURPLE);
              uiTimer_period = UI_TIMER_PERIOD_500mSEC;
              bHeart_led_on = true;
            }
            else
            {
              eIneedmd_LED_pattern(HEART_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_500mSEC;
              bHeart_led_on = false;
            }
            break;
          case HEART_LED_TRIANGLE:
            if (eHeart_last_sequence != HEART_LED_TRIANGLE)
            {
              bHeart_led_on = false;
            }
            bHeart_led_flashing = true;
            eHeart_last_sequence = HEART_LED_TRIANGLE;
            iHeart_led_flash_count = 0;
            if (bHeart_led_on == false)
            {
              eIneedmd_LED_pattern(HEART_LED_ORANGE);
              uiTimer_period = UI_TIMER_PERIOD_500mSEC;
              bHeart_led_on = true;
            }
            else
            {
              eIneedmd_LED_pattern(HEART_LED_OFF);
              uiTimer_period = UI_TIMER_PERIOD_500mSEC;
              bHeart_led_on = false;
            }
            break;
          default:
            eIneedmd_LED_pattern(UI_ALL_OFF);
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            bPower_led_on = false;
            ePower_last_sequence =  POWER_LED_UI_OFF;
            bPower_led_flashing = false;

            bComms_led_on = false;
            eComms_last_sequence =  COMMS_LED_UI_OFF;
            bComms_led_flashing = false;

            bHeart_led_on = false;
            eHeart_last_sequence =  HEART_LED_UI_OFF;
            bHeart_led_flashing = false;

            iHeart_led_flash_count = 0;
            break;
        }

        //Check if the timer should be stopped
        if(uiTimer_period == UI_TIMER_PERIOD_NONE)
        {
          //stop the timer
          Clock_stop(tUI_heart_led_timer);
        }
        else
        {
          //set the timer to the new period and start it
          Clock_stop(tUI_heart_led_timer);
          Clock_setTimeout(tUI_heart_led_timer, uiTimer_period);
          Clock_start(tUI_heart_led_timer);
        }
      }

      if((tUI_task_msg.uiUI_element & INMD_UI_ELEMENT_SOUNDER) == INMD_UI_ELEMENT_SOUNDER)
      {
        //eAlert_last_sequence
        //uiSounder_note_count
        //Not implemented yet
        uiTimer_period = UI_TIMER_PERIOD_NONE;
        switch(tUI_task_msg.eAlert_sound)
        {
          case ALERT_SOUND_NO_UI:
            uiSounder_note_count = 0;
            eAlert_last_sequence = ALERT_SOUND_NO_UI;
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            eIneedmd_alert_sound(ALERT_NOTE_OFF);
            break;
          case ALERT_SOUND_UI_OFF:
            uiSounder_note_count = 0;
            eAlert_last_sequence = ALERT_SOUND_UI_OFF;
            uiTimer_period = UI_TIMER_PERIOD_NONE;
            eIneedmd_alert_sound(ALERT_NOTE_OFF);
            break;
          case   ALERT_SOUND_ALERT:
            if(eAlert_last_sequence != ALERT_SOUND_ALERT)
            {
              uiSounder_note_count = 0;
            }
            if(uiSounder_note_count == 0)
            {
              eIneedmd_alert_sound(ALERT_GENERAL_MP_NOTE2);
              uiSounder_note_count++;
              eAlert_last_sequence = ALERT_SOUND_ALERT;
              uiTimer_period = ALERT_GENERAL_MP_NOTE_DURATION;
            }
            else if(uiSounder_note_count == 1)
            {
              eIneedmd_alert_sound(ALERT_GENERAL_MP_NOTE3);
              uiSounder_note_count++;
              eAlert_last_sequence = ALERT_SOUND_ALERT;
              uiTimer_period = ALERT_GENERAL_MP_NOTE_DURATION;
            }
 			else //note count == 3
			{
			  eIneedmd_alert_sound(ALERT_NOTE_OFF);
			  uiSounder_note_count = 0;
			  eAlert_last_sequence = ALERT_SOUND_ALERT;
			  uiTimer_period = ALERT_GENERAL_HP_OFF_DURATION;
			}
            break;
          case ALERT_SOUND_HP_ALARM:
            if(eAlert_last_sequence != ALERT_SOUND_HP_ALARM)
            {
              uiSounder_note_count = 0;
            }
            if(uiSounder_note_count == 0)
            {
              eIneedmd_alert_sound(ALERT_GENERAL_HP_NOTE2);
              uiSounder_note_count++;
              eAlert_last_sequence = ALERT_SOUND_HP_ALARM;
              uiTimer_period = ALERT_GENERAL_HP_NOTE_DURATION;
            }
            else if(uiSounder_note_count == 1)
            {
              eIneedmd_alert_sound(ALERT_GENERAL_HP_NOTE3);
              uiSounder_note_count++;
              eAlert_last_sequence = ALERT_SOUND_HP_ALARM;
              uiTimer_period = ALERT_GENERAL_HP_NOTE_DURATION;
            }
            else //note count == 3
            {
              eIneedmd_alert_sound(ALERT_NOTE_OFF);
              uiSounder_note_count = 0;
              eAlert_last_sequence = ALERT_SOUND_HP_ALARM;
              uiTimer_period = ALERT_GENERAL_HP_OFF_DURATION;
            }
            break;
        }
        //Check if the timer should be stopped
         if(uiTimer_period == UI_TIMER_PERIOD_NONE)
         {
           //stop the timer
           Clock_stop(tUI_sounder_timer);
         }
         else
         {
           //set the timer to the new period and start it
           Clock_stop(tUI_sounder_timer);
           Clock_setTimeout(tUI_sounder_timer, uiTimer_period);
           Clock_start(tUI_sounder_timer);
         }

      }
    }
  }//end while(1)
}

#endif //__INEEDMD_UI_C__
