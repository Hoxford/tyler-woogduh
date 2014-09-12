/******************************************************************************
*
* ineedmd_UI.c - the user interface application code
* Copyright (c) notice
*
******************************************************************************/
#ifndef __INEEDMD_UI_C__
#define __INEEDMD_UI_C__
/******************************************************************************
* includes
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "utils_inc/error_codes.h"
#include "utils_inc/proj_debug.h"
#include "utils_inc/clock.h"
#include "app_inc/ineedmd_UI.h"
#include "ineedmd_led.h"

/******************************************************************************
* defines
******************************************************************************/

/******************************************************************************
* variables
******************************************************************************/
uintmax_t uiTimer = 0;
bool      bIsSeqRunning = false;
/******************************************************************************
* external variables
******************************************************************************/

/******************************************************************************
* enums
******************************************************************************/
INMD_UI_LED_SEQ      eUI_LED_Seq     = LED_SEQ_NONE;
INMD_UI_SPEAKER_SEQ  eUI_Speaker_Seq = SPEAKER_SEQ_NONE;
/******************************************************************************
* structures
******************************************************************************/

/******************************************************************************
* external functions
******************************************************************************/

/******************************************************************************
* private function declarations
******************************************************************************/

/******************************************************************************
* private functions
******************************************************************************/

/******************************************************************************
* public functions
******************************************************************************/
ERROR_CODE eIneedmd_UI_process_init(void)
{
  ERROR_CODE eEC = ER_OK;
  return eEC;
}

ERROR_CODE eIneedmd_UI_request(uint8_t uiUser_Interface, INMD_UI_LED_SEQ eUI_LED_Sequence, INMD_UI_SPEAKER_SEQ eUI_Spkr_Sequence, bool bDo_immediatly)
{
#define DEBUG_eIneedmd_UI_request
#ifdef DEBUG_eIneedmd_UI_request
  #define  vDEBUG_INMD_UI_REQ  vDEBUG
#else
  #define vDEBUG_INMD_UI_REQ(a)
#endif
  ERROR_CODE eEC = ER_OK;

  if((uiUser_Interface & INMD_UI_LED) == INMD_UI_LED)
  {
    switch (eUI_LED_Sequence)
    {
      case LED_SEQ_NONE:
        ineedmd_led_pattern(LED_OFF);
        bIsSeqRunning = false;
        break;
      case LED_SEQ_OFF:
        ineedmd_led_pattern(LED_OFF);
        bIsSeqRunning = true;
        break;
//      case LED_SEQ_POWER_ON_BATT_LOW:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_POWER_ON_BATT_GOOD:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_BATT_CHARGING:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_BATT_CHARGING_LOW:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_LEAD_LOOSE:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_LEAD_GOOD_UPLOADING:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_DIG_FLATLINE:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_BT_CONNECTED:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_BT_ATTEMPTING:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_BT_FAILED:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_USB_CONNECTED:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_USB_FAILED:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_DATA_TRANSFER:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_TRANSFER_DONE:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_STORAGE_WARNING:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_ERASING:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_ERASE_DONE:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_DFU_MODE:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_MV_CAL:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_TRI_WVFRM:
//        if(bDo_immediatly == true)
//        {
//          //ineedmd_led_pattern();
//        }
//        else
//        {
//          //ineedmd_led_pattern();
//          //eUI_LED_Seq = ;
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_REBOOT:
//        if(bDo_immediatly == true)
//        {
//          //ineedmd_led_pattern();
//        }
//        else
//        {
//          //ineedmd_led_pattern();
//          //eUI_LED_Seq = ;
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_HIBERNATE:
//        if(bDo_immediatly == true)
//        {
//          //ineedmd_led_pattern();
//        }
//        else
//        {
//          //ineedmd_led_pattern();
//          //eUI_LED_Seq = ;
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_LEADS_ON:
//        if(bDo_immediatly == true)
//        {
//          //ineedmd_led_pattern();
//        }
//        else
//        {
//          //ineedmd_led_pattern();
//          //eUI_LED_Seq = ;
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_MEMORY_TEST:
//        if(bDo_immediatly == true)
//        {
//          //ineedmd_led_pattern();
//        }
//        else
//        {
//          //ineedmd_led_pattern();
//          //eUI_LED_Seq = ;
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_COM_BUS_TEST:
//        if(bDo_immediatly == true)
//        {
//        }
//        else
//        {
//          //ineedmd_led_pattern();
//          //eUI_LED_Seq = ;
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
//      case LED_SEQ_CPU_CLOCK_TEST:
//        if(bDo_immediatly == true)
//        {
//          //ineedmd_led_pattern();
//        }
//        else
//        {
//          //ineedmd_led_pattern();
//          //eUI_LED_Seq = ;
//          eClock_get_total_runtime(&uiTimer);
//        }
//        break;
      case LED_SEQ_FLASH_TEST:
        if(bDo_immediatly == true)
        {
          ineedmd_led_pattern(FLASH_TEST_BLOCKING);
        }
        else
        {
          ineedmd_led_pattern(FLASH_TEST);
          eUI_LED_Seq = LED_SEQ_FLASH_TEST;
          eClock_get_total_runtime(&uiTimer);
        }
        break;
      case LED_SEQ_TEST_PASS:
        if(bDo_immediatly == true)
        {
          ineedmd_led_pattern(TEST_PASS_BLOCKING);
        }
        else
        {
          ineedmd_led_pattern(TEST_PASS);
          eUI_LED_Seq = LED_SEQ_TEST_PASS;
          eClock_get_total_runtime(&uiTimer);
        }
        break;
      case LED_SEQ_POWER_UP_GOOD:
        if(bDo_immediatly == true)
        {
          ineedmd_led_pattern(POWER_UP_GOOD_BLOCKING);
        }
        else
        {
          ineedmd_led_pattern(POWER_UP_GOOD);
          eUI_LED_Seq = LED_SEQ_POWER_UP_GOOD;
          eClock_get_total_runtime(&uiTimer);
        }
        break;
      case LED_SEQ_ACTUAL_DFU:
        break;
      default:
        vDEBUG_INMD_UI_REQ("INMD ui req SYS HALT, unknown LED request");
        while(1){};
    }
  }else{/*do nothing*/}

  if((uiUser_Interface & INMD_UI_SPEAKER) == INMD_UI_SPEAKER)
  {

  }else{/*do nothing*/}

  return eEC;

#undef vDEBUG_INMD_UI_REQ
}

/******************************************************************************
* name: eIneedmd_UI_process
* description:
* param description:
* return value description:
******************************************************************************/
ERROR_CODE eIneedmd_UI_process(void)
{
#define DEBUG_eIneedmd_UI_process
#ifdef DEBUG_eIneedmd_UI_process
  #define  vDEBUG_INMD_UI_PROC  vDEBUG
#else
  #define vDEBUG_INMD_UI_PROC(a)
#endif
  ERROR_CODE eEC = ER_OK;
  uintmax_t uiCurrent_tick = 0;
  uintmax_t uiTick_diff = 0;

  eClock_get_total_runtime(&uiCurrent_tick);

  switch(eUI_LED_Seq)
  {
    case LED_SEQ_NONE:
      ineedmd_led_pattern(LED_OFF);
      bIsSeqRunning = false;
      break;
    case LED_SEQ_OFF:
      ineedmd_led_pattern(LED_OFF);
      bIsSeqRunning = true;
      break;
//    case LED_SEQ_POWER_ON_BATT_LOW:
//      break;
//    case LED_SEQ_POWER_ON_BATT_GOOD:
//      break;
//    case LED_SEQ_BATT_CHARGING:
//      break;
//    case LED_SEQ_BATT_CHARGING_LOW:
//      break;
//    case LED_SEQ_LEAD_LOOSE:
//      break;
//    case LED_SEQ_LEAD_GOOD_UPLOADING:
//      break;
//    case LED_SEQ_DIG_FLATLINE:
//      break;
//    case LED_SEQ_BT_CONNECTED:
//      break;
//    case LED_SEQ_BT_ATTEMPTING:
//      break;
//    case LED_SEQ_BT_FAILED:
//      break;
//    case LED_SEQ_USB_CONNECTED:
//      break;
//    case LED_SEQ_USB_FAILED:
//      break;
//    case LED_SEQ_DATA_TRANSFER:
//      break;
//    case LED_SEQ_TRANSFER_DONE:
//      break;
//    case LED_SEQ_STORAGE_WARNING:
//      break;
//    case LED_SEQ_ERASING:
//      break;
//    case LED_SEQ_ERASE_DONE:
//      break;
//    case LED_SEQ_DFU_MODE:
//      break;
//    case LED_SEQ_MV_CAL:
//      break;
//    case LED_SEQ_TRI_WVFRM:
//      break;
//    case LED_SEQ_REBOOT:
//      break;
//    case LED_SEQ_HIBERNATE:
//      break;
//    case LED_SEQ_LEADS_ON:
//      break;
//    case LED_SEQ_MEMORY_TEST:
//      break;
//    case LED_SEQ_COM_BUS_TEST:
//      break;
//    case LED_SEQ_CPU_CLOCK_TEST:
//      break;
    case LED_SEQ_FLASH_TEST:
      uiTick_diff = uiCurrent_tick - uiTimer;
      if(uiTick_diff >= 1000)
      {
        ineedmd_led_pattern(TEST_PASS_OFF);
        eUI_LED_Seq = LED_SEQ_NONE;
      }
      else
      {
        ineedmd_led_pattern(TEST_PASS_ON);
      }
      break;
    case LED_SEQ_TEST_PASS:
      uiTick_diff = uiCurrent_tick - uiTimer;
      if(uiTick_diff >= 100)
      {
        ineedmd_led_pattern(TEST_PASS_OFF);
        eUI_LED_Seq = LED_SEQ_NONE;
      }
      else
      {
        ineedmd_led_pattern(TEST_PASS_ON);
      }

      break;
    case LED_SEQ_POWER_UP_GOOD:
      uiTick_diff = uiCurrent_tick - uiTimer;
      if(uiTick_diff >= 500)
      {
        ineedmd_led_pattern(POWER_UP_GOOD_OFF);
        eUI_LED_Seq = LED_SEQ_NONE;
      }
      else if(uiTick_diff >= 400)
      {
         ineedmd_led_pattern(POWER_UP_GOOD_ON);
      }
      else if(uiTick_diff >= 300)
      {
        ineedmd_led_pattern(POWER_UP_GOOD_OFF);
      }
      else if(uiTick_diff >= 200)
      {
        ineedmd_led_pattern(POWER_UP_GOOD_ON);
      }
      else if(uiTick_diff >= 100)
      {
        ineedmd_led_pattern(POWER_UP_GOOD_OFF);
      }
      else
      {
        ineedmd_led_pattern(POWER_UP_GOOD_ON);
      }

      break;
    case LED_SEQ_ACTUAL_DFU:
      break;
    default:
      vDEBUG_INMD_UI_PROC("INMD ui proc SYS HALT, unknown LED sequence");
      while(1){};
      break;
  }

  return eEC;
}

#endif //__INEEDMD_UI_C__
