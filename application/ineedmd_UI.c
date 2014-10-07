/******************************************************************************
*
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
#include "app_inc/ineedmd_UI.h"

#include "driverlib/systick.h"
#include "drivers_inc/ineedmd_led.h"

//#include <ti/drivers/I2C.h>
//#include <ti/drivers/i2c/I2CTiva.h>

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


#define INEEDMD_I2C_ADDRESS_LED_DRIVER  0x1b

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

/////////////////////////////////////////////////////////////
// SPI setup

#define INEEDMD_ADC_SPI     SSI0_BASE
#define INEEDMD_FLASH_SPI   SSI1_BASE
#define INEEDMD_SPI_CLK     16000000

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
uintmax_t uiTimer = 0;
bool      bIsSeqRunning = false;

uint8_t uiMailbox_buff[768];

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
typedef struct tStruct_UI_State
{
  bool heart_led_on;
  bool comms_led_on;
  bool power_led_on;
  bool sounder_on;
  NO_UI_ELELMENT heart_led_last_state;
  NO_UI_ELELMENT comms_led_last_state;
  NO_UI_ELELMENT power_led_last_state;
  NO_UI_ELELMENT sounder_last_state;
}tStruct_UI_State;

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


/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
uint8_t ineedmd_do_ui_element(NO_UI_ELELMENT ui_element);

/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
uint8_t ineedmd_do_ui_element(NO_UI_ELELMENT ui_element)
{
  return 0;
}

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eIneedmd_UI_task_init(void)
{
  ERROR_CODE eEC = ER_OK;
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
  ERROR_CODE eEC = ER_OK;
  bool bDid_message_post = false;
  tUI_request tUI_Msg;
  uint16_t uiElement = ptUI_Request->uiUI_element;
  bool bDo_immediatly = ptUI_Request->bPerform_Immediatly;
  eHEART_LED_UI eHeart_led_seq = ptUI_Request->eHeart_led_sequence;
  eCOMMS_LED_UI eComms_led_seq = ptUI_Request->eComms_led_sequence;
  ePOWER_LED_UI ePower_led_seq = ptUI_Request->ePower_led_sequence;

  // Heart LED sequence request
  //
  if((uiElement & INMD_UI_ELEMENT_HEART_LED) == INMD_UI_ELEMENT_HEART_LED)
  {
    switch (eHeart_led_seq)
    {
      default:
        vDEBUG_INMD_UI_REQ("INMD INMD_UI_ELEMENT_COMMS_LED SYS HALT, unknown LED request");
        while(1){};
    }
  }else{/*do nothing*/}

  // Comms LED sequence request
  //
  if((uiElement & INMD_UI_ELEMENT_COMMS_LED) == INMD_UI_ELEMENT_COMMS_LED)
  {
    switch (eComms_led_seq)
    {
      default:
        vDEBUG_INMD_UI_REQ("INMD INMD_UI_ELEMENT_COMMS_LED SYS HALT, unknown LED request");
        while(1){};
    }
  }else{/*do nothing*/}

  // Power LED sequence request
  //
  if((uiElement & INMD_UI_ELEMENT_POWER_LED) == INMD_UI_ELEMENT_POWER_LED)
  {
    eIneedmd_UI_params_init(&tUI_Msg);
    tUI_Msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;

    switch (ePower_led_seq)
    {
      case POWER_LED_NO_UI:
        break;
      case POWER_LED_UI_OFF:
        break;
      case POWER_LED_POWER_ON_90to100:
        tUI_Msg.ePower_led_sequence = POWER_LED_POWER_ON_90to100;
        break;
      case POWER_LED_POWER_ON_50to90:
        break;
      case POWER_LED_POWER_ON_25to50:
        break;
      case POWER_LED_POWER_ON_0to25:
        break;
      case POWER_LED_POWER_ON_BLIP_90to100:
        break;
      case POWER_LED_POWER_ON_BLIP_50to90:
        break;
      case POWER_LED_POWER_ON_BLIP_25to50:
        break;
      case POWER_LED_POWER_ON_BLIP_0to25:
        break;
      default:
        vDEBUG_INMD_UI_REQ("INMD INMD_UI_ELEMENT_COMMS_LED SYS HALT, unknown LED request");
        while(1){};
    }

    Mailbox_post(tUI_mailbox, &tUI_Msg, BIOS_WAIT_FOREVER);
  }else{/*do nothing*/}

  return eEC;

#undef vDEBUG_INMD_UI_REQ
}

void vIneedmd_UI_Comms_led_timer_INT_Service(UArg a0)
{
  return;
}

void vIneedmd_UI_Heart_led_timer_INT_Service(UArg a0)
{
  return;
}

void vIneedmd_UI_Power_led_timer_INT_Service(UArg a0)
{
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
//  I2C_Handle         tI2C_handle;
//  I2C_Params         tI2C_params;
  tStruct_UI_Process          tUI_Process;
  tStruct_UI_Command_Request  tUI_requested_feature;
  tUI_request tUI_task_msg;

  eUI_Task_State = UI_TASK_SETUP_NOT_STARTED;

  vDEBUG("enter taskineedmdUI()\n");

  eEC = eIneedmd_LED_driver_setup();
  if (eEC == ER_OK)
  {
    eUI_Task_State = UI_TASK_LED_DEVICE_SETUP_COMPLETE;
  }
  else
  {
    eUI_Task_State = UI_TASK_ERROR;
  }

  if(tUI_mailbox != NULL)
  {
    eUI_Task_State = UI_TASK_READY;
  }
  else
  {
    eUI_Task_State = UI_TASK_ERROR;
  }
//  eClock_get_total_runtime(&uiCurrent_tick);

  eIneedmd_UI_params_init(&tUI_task_msg);
  tUI_task_msg.uiUI_element = INMD_UI_ELEMENT_POWER_LED;
  tUI_task_msg.ePower_led_sequence = POWER_LED_POWER_ON_90to100;
  eIneedmd_UI_request(&tUI_task_msg);
  eIneedmd_UI_params_init(&tUI_task_msg);

  //Main UI task loop
  //
  while(1)
  {
    if(Mailbox_pend(tUI_mailbox, &tUI_task_msg, BIOS_WAIT_FOREVER) == true)
    {
//      if (tUI_requested_feature.heart_led_UI_request != HEART_LED_NO_UI )
//      {
//        tUI_Process.heart_led_last_UI_request = tUI_requested_feature.heart_led_UI_request;
//        tUI_Process.heart_led_ui_start_time = SysTickValueGet();
//        tUI_requested_feature.heart_led_UI_request = HEART_LED_NO_UI;
//      }
//
//      if (tUI_requested_feature.comms_led_UI_request != COMMS_LED_NO_UI )
//      {
//        tUI_Process.comms_led_last_UI_request = tUI_requested_feature.comms_led_UI_request;
//        tUI_Process.comms_led_ui_start_time = SysTickValueGet();
//        tUI_requested_feature.comms_led_UI_request = COMMS_LED_NO_UI;
//      }

//      if (tUI_requested_feature.power_led_UI_request != POWER_LED_NO_UI )
      if (tUI_task_msg.uiUI_element == INMD_UI_ELEMENT_POWER_LED)
      {
        switch(tUI_task_msg.ePower_led_sequence)
        {
          case POWER_LED_NO_UI:
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
            eIneedmd_LED_pattern(POWER_ON_BATT_GOOD);
            tUI_Process.comms_led_last_UI_request = COMMS_LED_NO_UI;
            break;
          case POWER_LED_UI_OFF:
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
            tUI_Process.power_led_last_UI_request = POWER_LED_NO_UI;
            break;
          case POWER_LED_POWER_ON_90to100:
            eIneedmd_LED_pattern(POWER_ON_BATT_GOOD);
            break;
          case POWER_LED_POWER_ON_50to90:
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_ORANGE );
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_ORANGE );
            break;
          case POWER_LED_POWER_ON_25to50:
//            uiTick_diff = uiCurrent_tick - tUI_Process.comms_led_ui_start_time;
//            if(uiTick_diff >= 2000)
//            {
//                tUI_Process.comms_led_ui_start_time = uiCurrent_tick;
//            }
//            else if(uiTick_diff >= 1000)
//            {
//                        ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//                        ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//            }
//            else
//            {
//                        ineedmd_do_ui_element( tI2C_handle, COMMS_LED_ORANGE );
//                        ineedmd_do_ui_element( tI2C_handle, HEART_LED_ORANGE );
//            }
            break;
          case POWER_LED_POWER_ON_0to25:
//            uiTick_diff = uiCurrent_tick - tUI_Process.comms_led_ui_start_time;
//            if(uiTick_diff >= 2000)
//            {
//                tUI_Process.comms_led_ui_start_time = uiCurrent_tick;
//            }
//            else if(uiTick_diff >= 1000)
//            {
//                        ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//                        ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//            }
//            else
//            {
//                        ineedmd_do_ui_element( tI2C_handle, COMMS_LED_RED );
//                        ineedmd_do_ui_element( tI2C_handle, COMMS_LED_RED );
//            }
            break;
          case POWER_LED_POWER_ON_BLIP_90to100:
//            uiTick_diff = uiCurrent_tick - tUI_Process.power_led_ui_start_time;
//            if(uiTick_diff >= 500)
//            {
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//              tUI_Process.power_led_last_UI_request = POWER_LED_NO_UI;
//            }
//            else
//            {
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_GREEN );
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_GREEN );
//            }
            break;
          case POWER_LED_POWER_ON_BLIP_50to90:
//            uiTick_diff = uiCurrent_tick - tUI_Process.power_led_ui_start_time;
//            if(uiTick_diff >= 1500)
//            {
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//              tUI_Process.power_led_last_UI_request = POWER_LED_NO_UI;
//            }
//            else if(uiTick_diff >= 1000)
//            {
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_ORANGE );
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_ORANGE );
//            }
//            else if(uiTick_diff >= 500)
//            {
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//            }
//            else
//            {
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_ORANGE );
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_ORANGE );
//            }
            break;
          case POWER_LED_POWER_ON_BLIP_25to50:
//            uiTick_diff = uiCurrent_tick - tUI_Process.power_led_ui_start_time;
//            if(uiTick_diff >= 500)
//            {
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//              tUI_Process.power_led_last_UI_request = POWER_LED_NO_UI;
//            }
//            else
//            {
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_ORANGE );
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_ORANGE );
//            }
            break;
          case POWER_LED_POWER_ON_BLIP_0to25:
//            uiTick_diff = uiCurrent_tick - tUI_Process.power_led_ui_start_time;
//            if(uiTick_diff >= 500)
//            {
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//              tUI_Process.power_led_last_UI_request = POWER_LED_NO_UI;
//            }
//            else
//            {
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_RED );
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_RED );
//            }
            break;
          default:
//            ineedmd_do_ui_element( tI2C_handle, UI_ALL_OFF );
            break;
        }
      }

//      if (tUI_requested_feature.sounder_UI_request != ALERT_SOUND_NO_UI )
//      {
//        tUI_Process.sounder_last_UI_request = tUI_requested_feature.sounder_UI_request;
//        tUI_Process.sounder_ui_start_time = SysTickValueGet();
//        tUI_requested_feature.sounder_UI_request = ALERT_SOUND_NO_UI;
//      }
//
//      if (tUI_requested_feature.service_UI_request != FIELD_SERVICE_NO_FIELD_SERVICE )
//      {
//        tUI_Process.service_last_UI_request = tUI_requested_feature.service_UI_request;
//        tUI_Process.service_ui_start_time = SysTickValueGet();
//        tUI_requested_feature.service_UI_request = FIELD_SERVICE_NO_FIELD_SERVICE;
//      }

      if (tUI_Process.service_last_UI_request != FIELD_SERVICE_NO_FIELD_SERVICE)
      {
//        //clear all other ui
//        tUI_Process.sounder_last_UI_request=ALERT_SOUND_NO_UI;
//        tUI_Process.power_led_last_UI_request=POWER_LED_NO_UI;
//        tUI_Process.comms_led_last_UI_request=COMMS_LED_NO_UI;
//        tUI_Process.heart_led_last_UI_request=HEART_LED_NO_UI;
//        switch (tUI_Process.service_last_UI_request)
//        {
//          case FIELD_SERVICE_OFF_FIELD_SERVICE:
//            ineedmd_do_ui_element( tI2C_handle, UI_ALL_OFF );
//
//            break;
//          case FIELD_SERVICE_DFU:
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_PURPLE );
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_PURPLE );
//            break;
//          case FIELD_SERVICE_ONE_MV_CAL:
//            uiTick_diff = uiCurrent_tick - tUI_Process.service_ui_start_time;
//            if(uiTick_diff >= 1000)
//            {
//              tUI_Process.service_ui_start_time = uiCurrent_tick;
//            }
//            else if(uiTick_diff >= 500)
//            {
//              ineedmd_do_ui_element( tI2C_handle, UI_ALL_OFF );
//              tUI_Process.service_last_UI_request = FIELD_SERVICE_NO_FIELD_SERVICE;
//            }
//            else
//            {
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_PURPLE );
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_PURPLE );
//            }
//          case FIELD_SERVICE_TRIANGLE_WAVE:
//            uiTick_diff = uiCurrent_tick - tUI_Process.service_ui_start_time;
//            if(uiTick_diff >= 1000)
//            {
//              tUI_Process.service_ui_start_time = uiCurrent_tick;
//            }
//            else if(uiTick_diff >= 500)
//            {
//              ineedmd_do_ui_element( tI2C_handle, UI_ALL_OFF );
//            }
//            else
//            {
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_ORANGE );
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_ORANGE );
//            }
//            break;
//          case FIELD_SERVICE_REBOOT:
//            uiTick_diff = uiCurrent_tick - tUI_Process.service_ui_start_time;
//            if(uiTick_diff >= 1000)
//            {
//              ineedmd_do_ui_element( tI2C_handle, UI_ALL_OFF );
//              tUI_Process.service_last_UI_request = FIELD_SERVICE_NO_FIELD_SERVICE;
//            }
//            else
//            {
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_RED );
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_RED );
//            }
//            break;
//          case FIELD_SERVICE_LED_SEQ_POWER_UP_GOOD:
//            uiTick_diff = uiCurrent_tick - tUI_Process.service_ui_start_time;
//            if(uiTick_diff >= 2500)
//            {
//              ineedmd_do_ui_element( tI2C_handle, UI_ALL_OFF );
//              tUI_Process.service_last_UI_request = FIELD_SERVICE_NO_FIELD_SERVICE;
//            }
//            else if(uiTick_diff >= 2000)
//            {
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_GREEN );
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_GREEN );
//            }
//            else if(uiTick_diff >= 1500)
//            {
//              ineedmd_do_ui_element( tI2C_handle, UI_ALL_OFF );
//            }
//            else if(uiTick_diff >= 1000)
//            {
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_GREEN );
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_GREEN );
//            }
//            else if(uiTick_diff >= 500)
//            {
//              ineedmd_do_ui_element( tI2C_handle, UI_ALL_OFF );
//            }
//            else
//            {
//              ineedmd_do_ui_element( tI2C_handle, HEART_LED_GREEN );
//              ineedmd_do_ui_element( tI2C_handle, COMMS_LED_GREEN );
//            }
//
//            break;
//          default:
//            ineedmd_do_ui_element( tI2C_handle, UI_ALL_OFF );
//            break;
//        }
      }
      else
      {
//        switch(tUI_Process.heart_led_last_UI_request)
//        {
//        case HEART_LED_UI_OFF:
//          ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//          tUI_Process.heart_led_last_UI_request = HEART_LED_NO_UI;
//          break;
//        case LEAD_OFF_NO_DATA:
//          uiTick_diff = uiCurrent_tick - tUI_Process.heart_led_ui_start_time;
//          if(uiTick_diff >= 10000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//            tUI_Process.heart_led_last_UI_request = HEART_LED_NO_UI;
//          }
//          else if(uiTick_diff >= 8000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_ORANGE );
//          }
//          else if(uiTick_diff >= 6000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//          }
//          else if(uiTick_diff >= 4000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_ORANGE );
//          }
//          else if(uiTick_diff >= 2000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_ORANGE );
//          }
//          break;
//        case LEAD_ON:
//          uiTick_diff = uiCurrent_tick - tUI_Process.heart_led_ui_start_time;
//          if(uiTick_diff >= 10000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//            tUI_Process.heart_led_last_UI_request = HEART_LED_NO_UI;
//          }
//          else if(uiTick_diff >= 8000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_GREEN );
//          }
//          else if(uiTick_diff >= 6000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//          }
//          else if(uiTick_diff >= 4000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_GREEN );
//          }
//          else if(uiTick_diff >= 2000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_GREEN );
//          }
//          break;
//        case DIGITAL_FLATLINE:
//          uiTick_diff = uiCurrent_tick -  tUI_Process.heart_led_ui_start_time;
//          if(uiTick_diff >= 1000)
//          {
//            tUI_Process.heart_led_ui_start_time = uiCurrent_tick;
//          }
//          else if(uiTick_diff >= 500)
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_RED );
//          }
//          break;
//        case HEART_LED_NO_UI:
//          ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//          tUI_Process.heart_led_last_UI_request = HEART_LED_NO_UI;
//          break;
//        default:
//          ineedmd_do_ui_element( tI2C_handle, UI_ALL_OFF );
//
//          break;
      } //end of the heart LED section


      switch (tUI_Process.comms_led_last_UI_request)
      {
//        case COMMS_LED_NO_UI:
//          ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//          tUI_Process.comms_led_last_UI_request = COMMS_LED_NO_UI;
//          break;
//        case COMMS_LED_UI_OFF:
//          ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//          tUI_Process.comms_led_last_UI_request = COMMS_LED_NO_UI;
//          break;
//        case BLOOTHOOTH_CONNECTION_SUCESSFUL:
//          uiTick_diff = uiCurrent_tick - tUI_Process.comms_led_ui_start_time;
//          if(uiTick_diff >= 5000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//            tUI_Process.comms_led_last_UI_request = COMMS_LED_NO_UI;
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_BLUE );
//          }
//          break;
//        case BLUETOOTH_PAIRING:
//          uiTick_diff = uiCurrent_tick -  tUI_Process.comms_led_ui_start_time;
//          if(uiTick_diff >= 2000)
//          {
//            tUI_Process.comms_led_ui_start_time = uiCurrent_tick;
//          }
//          else if(uiTick_diff >= 1000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, HEART_LED_OFF );
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_BLUE );
//          }
//          break;
//        case BLUETOOTH_PAIRING_FAILIED:
//          uiTick_diff = uiCurrent_tick - tUI_Process.comms_led_ui_start_time;
//          if(uiTick_diff >= 5000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//            tUI_Process.comms_led_last_UI_request = COMMS_LED_NO_UI;
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_ORANGE );
//          }
//          break;
//        case USB_CONNECTION_SUCESSFUL:
//          uiTick_diff = uiCurrent_tick - tUI_Process.comms_led_ui_start_time;
//          if(uiTick_diff >= 5000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//            tUI_Process.comms_led_last_UI_request = COMMS_LED_NO_UI;
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_BLUE );
//          }
//          break;
//        case USB_CONNECTION_FAILED:
//          uiTick_diff = uiCurrent_tick - tUI_Process.comms_led_ui_start_time;
//          if(uiTick_diff >= 5000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//            tUI_Process.comms_led_last_UI_request = COMMS_LED_NO_UI;
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_ORANGE );
//          }
//          break;
//        case DATA_TANSFERING_FROM_DONGLE:
//          uiTick_diff = uiCurrent_tick - tUI_Process.comms_led_ui_start_time;
//          if(uiTick_diff >= 10000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//            tUI_Process.comms_led_last_UI_request = COMMS_LED_NO_UI;
//          }
//          else if(uiTick_diff >= 8000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_PURPLE );
//          }
//          else if(uiTick_diff >= 6000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//          }
//          else if(uiTick_diff >= 4000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_PURPLE );
//          }
//          else if(uiTick_diff >= 2000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_PURPLE );
//          }
//          break;
//        case DATA_TRANSFER_SUCESSFUL:
//          uiTick_diff = uiCurrent_tick - tUI_Process.comms_led_ui_start_time;
//          if(uiTick_diff >= 5000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//            tUI_Process.comms_led_last_UI_request = COMMS_LED_NO_UI;
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_PURPLE );
//          }
//          break;
//        case DONGLE_STORAGE_WARNING:
//          uiTick_diff = uiCurrent_tick - tUI_Process.comms_led_ui_start_time;
//          if(uiTick_diff >= 10000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//            tUI_Process.comms_led_last_UI_request = COMMS_LED_NO_UI;
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_ORANGE );
//          }
//          break;
//        case ERASING_STORED_DATA:
//          uiTick_diff = uiCurrent_tick - tUI_Process.comms_led_ui_start_time;
//          if(uiTick_diff >= 6000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//            tUI_Process.comms_led_last_UI_request = COMMS_LED_NO_UI;
//          }
//          else if(uiTick_diff >= 4000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_ORANGE );
//          }
//          else if(uiTick_diff >= 2000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_ORANGE );
//          }
//              break;
//        case ERASE_COMPLETE:
//          uiTick_diff = uiCurrent_tick - tUI_Process.comms_led_ui_start_time;
//          if(uiTick_diff >= 2000)
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_OFF );
//            tUI_Process.comms_led_last_UI_request = COMMS_LED_NO_UI;
//          }
//          else
//          {
//            ineedmd_do_ui_element( tI2C_handle, COMMS_LED_GREEN );
//          }
//          break;
//        default:
//          ineedmd_do_ui_element( tI2C_handle, UI_ALL_OFF );
//          break;
      } //end of the comms led section



#ifdef UI_DEBUG

      vDEBUG("exit UI function - this is an OF point\n");
#endif
      Task_exit();
    }else{/*do nothing*/}//end mailbox
  }//end while(1)
//  switch(eUI_LED_Seq)
//  {
//    case LED_SEQ_NONE:
//      ineedmd_led_pattern(LED_OFF);
//      bIsSeqRunning = false;
//      break;
//    case LED_SEQ_OFF:
//      ineedmd_led_pattern(LED_OFF);
//      bIsSeqRunning = true;
//      break;
//    case LED_SEQ_POWER_ON_BATT_LOW:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 2000)
//      {
//        uiTimer = uiCurrent_tick;
//      }
//      else if(uiTick_diff >= 1000)
//      {
//        ineedmd_led_pattern(POWER_ON_BATT_LOW_OFF);
//      }
//      else
//      {
//        ineedmd_led_pattern(POWER_ON_BATT_LOW_ON);
//      }
//      break;
//    case LED_SEQ_POWER_ON_BATT_GOOD:
//      bIsSeqRunning = true;
//      ineedmd_led_pattern(POWER_ON_BATT_GOOD_ON);
//      break;
//    case LED_SEQ_BATT_CHARGING:
//      bIsSeqRunning = true;
//      ineedmd_led_pattern(BATT_CHARGING_ON);
//      break;
//    case LED_SEQ_BATT_CHARGING_LOW:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 2000)
//      {
//        uiTimer = uiCurrent_tick;
//      }
//        else if(uiTick_diff >= 1000)
//      {
//          ineedmd_led_pattern(BATT_CHARGING_LOW_OFF);
//      }
//      else
//      {
//        ineedmd_led_pattern(BATT_CHARGING_LOW_ON);
//      }
//      break;
//
//    case LED_SEQ_LEAD_LOOSE:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 4000)
//      {
//        uiTimer = uiCurrent_tick;
//      }
//        else if(uiTick_diff >= 2000)
//      {
//          ineedmd_led_pattern(LEAD_LOOSE_OFF);
//      }
//      else
//      {
//        ineedmd_led_pattern(LEAD_LOOSE_ON);
//      }
//      break;
//
//    case LED_SEQ_LEAD_GOOD_UPLOADING:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 4000)
//      {
//        uiTimer = uiCurrent_tick;
//      }
//        else if(uiTick_diff >= 2000)
//      {
//          ineedmd_led_pattern(LEAD_GOOD_UPLOADING_OFF);
//      }
//      else
//      {
//        ineedmd_led_pattern(LEAD_GOOD_UPLOADING_ON);
//      }
//      break;
//
//    case LED_SEQ_DIG_FLATLINE:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 1000)
//      {
//        uiTimer = uiCurrent_tick;
//      }
//        else if(uiTick_diff >= 500)
//      {
//          ineedmd_led_pattern(DIG_FLATLINE_OFF);
//      }
//      else
//      {
//        ineedmd_led_pattern(DIG_FLATLINE_ON);
//      }
//      break;
//
//    case LED_SEQ_BT_CONNECTED:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 5000)
//      {
//        ineedmd_led_pattern(BT_CONNECTED_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(BT_CONNECTED_ON);
//      }
//      break;
//
//    case LED_SEQ_BT_ATTEMPTING:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 10000)
//      {
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else if(uiTick_diff >= 9000)
//        {
//            ineedmd_led_pattern(BT_ATTEMPTING_OFF);
//        }
//      else if(uiTick_diff >= 8000)
//        {
//            ineedmd_led_pattern(BT_ATTEMPTING_ON);
//        }
//      else if(uiTick_diff >= 7000)
//        {
//            ineedmd_led_pattern(BT_ATTEMPTING_OFF);
//        }
//      else if(uiTick_diff >= 6000)
//        {
//            ineedmd_led_pattern(BT_ATTEMPTING_ON);
//        }
//      else if(uiTick_diff >= 5000)
//        {
//            ineedmd_led_pattern(BT_ATTEMPTING_OFF);
//        }
//      else if(uiTick_diff >= 4000)
//        {
//            ineedmd_led_pattern(BT_ATTEMPTING_ON);
//        }
//      else if(uiTick_diff >= 3000)
//        {
//            ineedmd_led_pattern(BT_ATTEMPTING_OFF);
//        }
//      else if(uiTick_diff >= 2000)
//        {
//            ineedmd_led_pattern(BT_ATTEMPTING_ON);
//        }
//      else if(uiTick_diff >= 1000)
//        {
//            ineedmd_led_pattern(BT_ATTEMPTING_OFF);
//        }
//      else
//      {
//        ineedmd_led_pattern(BT_ATTEMPTING_ON);
//      }
//      break;
//
//    case LED_SEQ_BT_FAILED:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 5000)
//      {
//        ineedmd_led_pattern(BT_FAILED_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(BT_FAILED_ON);
//      }
//      break;
//
//    case LED_SEQ_USB_CONNECTED:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 5000)
//      {
//        ineedmd_led_pattern(USB_CONNECTED_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(USB_CONNECTED_ON);
//      }
//      break;
//
//    case LED_SEQ_USB_FAILED:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 5000)
//      {
//        ineedmd_led_pattern(USB_FAILED_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(USB_FAILED_ON);
//      }
//      break;
//
//    case LED_SEQ_DATA_TRANSFER:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 5000)
//      {
//        ineedmd_led_pattern(DATA_TRANSFER_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else if(uiTick_diff >= 4000)
//      {
//        ineedmd_led_pattern(DATA_TRANSFER_ON);
//      }
//      else if(uiTick_diff >= 3000)
//      {
//        ineedmd_led_pattern(DATA_TRANSFER_OFF);
//      }
//      else if(uiTick_diff >= 2000)
//      {
//        ineedmd_led_pattern(DATA_TRANSFER_ON);
//      }
//      else if(uiTick_diff >= 1000)
//      {
//        ineedmd_led_pattern(DATA_TRANSFER_OFF);
//      }
//      else
//      {
//        ineedmd_led_pattern(DATA_TRANSFER_ON);
//      }
//      break;
//
//    case LED_SEQ_TRANSFER_DONE:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 5000)
//      {
//        ineedmd_led_pattern(TRANSFER_DONE_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(TRANSFER_DONE_ON);
//      }
//      break;
//    case LED_SEQ_STORAGE_WARNING:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 10000)
//      {
//        ineedmd_led_pattern(STORAGE_WARNING_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(STORAGE_WARNING_ON);
//      }
//      break;
//
//    case LED_SEQ_ERASING:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 4000)
//      {
//        uiTimer = uiCurrent_tick;
//      }
//      else if(uiTick_diff >= 2000)
//      {
//        ineedmd_led_pattern(ERASING_OFF);
//      }
//      else
//      {
//        ineedmd_led_pattern(ERASING_ON);
//      }
//      break;
//
//    case LED_SEQ_ERASE_DONE:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 2000)
//      {
//        ineedmd_led_pattern(ERASE_DONE_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(ERASE_DONE_ON);
//      }
//      break;
//
//    case LED_SEQ_DFU_MODE:
//      ineedmd_led_pattern(DFU_MODE_ON);
//      break;
//
//    case LED_SEQ_MV_CAL:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 1000)
//      {
//        uiTimer = uiCurrent_tick;
//      }
//      else if(uiTick_diff >= 500)
//      {
//        ineedmd_led_pattern(MV_CAL_OFF);
//      }
//      else
//      {
//        ineedmd_led_pattern(MV_CAL_ON);
//      }
//      break;
//
//    case LED_SEQ_TRI_WVFRM:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 1000)
//      {
//        uiTimer = uiCurrent_tick;
//      }
//      else if(uiTick_diff >= 500)
//      {
//        ineedmd_led_pattern(TRI_WVFRM_OFF);
//      }
//      else
//      {
//        ineedmd_led_pattern(TRI_WVFRM_ON);
//      }
//      break;
//
//    case LED_SEQ_REBOOT:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 1000)
//      {
//        ineedmd_led_pattern(REBOOT_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(REBOOT_ON);
//      }
//      break;
//    case LED_SEQ_HIBERNATE_GOOD:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 500)
//      {
//        ineedmd_led_pattern(HIBERNATE_GOOD_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(HIBERNATE_GOOD_ON);
//      }
//      break;
//    case LED_SEQ_HIBERNATE_MEDIUM:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 500)
//      {
//        ineedmd_led_pattern(HIBERNATE_MEDIUM_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(HIBERNATE_MEDIUM_ON);
//      }
//      break;
//    case LED_SEQ_HIBERNATE_LOW:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 500)
//      {
//        ineedmd_led_pattern(HIBERNATE_LOW_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(HIBERNATE_LOW_ON);
//      }
//      break;
//
////      break;
////    case LED_SEQ_LEADS_ON:
////      break;
////    case LED_SEQ_MEMORY_TEST:
////      break;
////    case LED_SEQ_COM_BUS_TEST:
////      break;
////    case LED_SEQ_CPU_CLOCK_TEST:
////      break;
//    case LED_SEQ_FLASH_TEST:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 1000)
//      {
//        ineedmd_led_pattern(TEST_PASS_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(TEST_PASS_ON);
//      }
//      break;
//    case LED_SEQ_TEST_PASS:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 100)
//      {
//        ineedmd_led_pattern(TEST_PASS_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else
//      {
//        ineedmd_led_pattern(TEST_PASS_ON);
//      }
//
//      break;
//    case LED_SEQ_POWER_UP_GOOD:
//      uiTick_diff = uiCurrent_tick - uiTimer;
//      if(uiTick_diff >= 2500)
//      {
//        ineedmd_led_pattern(POWER_UP_GOOD_OFF);
//        eUI_LED_Seq = LED_SEQ_NONE;
//      }
//      else if(uiTick_diff >= 2000)
//      {
//         ineedmd_led_pattern(POWER_UP_GOOD_ON);
//      }
//      else if(uiTick_diff >= 1500)
//      {
//        ineedmd_led_pattern(POWER_UP_GOOD_OFF);
//      }
//      else if(uiTick_diff >= 1000)
//      {
//        ineedmd_led_pattern(POWER_UP_GOOD_ON);
//      }
//      else if(uiTick_diff >= 500)
//      {
//        ineedmd_led_pattern(POWER_UP_GOOD_OFF);
//      }
//      else
//      {
//        ineedmd_led_pattern(POWER_UP_GOOD_ON);
//      }
//
//      break;
//    case LED_SEQ_ACTUAL_DFU:
//      break;
//
//    default:
//      vDEBUG_INMD_UI_PROC("INMD ui proc SYS HALT, unknown LED sequence");
//      while(1){};
//      break;
//  }

}

#endif //__INEEDMD_UI_C__
