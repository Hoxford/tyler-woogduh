//*****************************************************************************
//
// ineedmd_waveform.c - waveform application for the patient EKG data processing
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __INEEDMD_WAVEFORM_C__
#define __INEEDMD_WAVEFORM_C__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils_inc/error_codes.h"
#include "board.h"
#include "drivers_inc/ineedmd_bluetooth_radio.h"
#include "drivers_inc/ineedmd_adc.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "app_inc/ineedmd_waveform.h"
#include "utils_inc/proj_debug.h"
#include "utils_inc/osal.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
bool bTest_Mode = false;

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
eEKG_Task_state eCurrent_EKG_Task_state = EKG_TASK_IDLE;
/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eIneedmd_EKG_set_task_state(eEKG_Task_state eState);

/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

ERROR_CODE eIneedmd_EKG_set_task_state(eEKG_Task_state eState)
{
  ERROR_CODE eEC = ER_FAIL;
  eCurrent_EKG_Task_state = eState;

  if(eCurrent_EKG_Task_state == eState)
  {
    eEC = ER_OK;
  }
  else
  {
    eEC = ER_FAIL;
  }

  return eEC;
}

#ifdef NOT_NOW
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iIneedmd_waveform_enable_TestSignal(void)
{

  set_system_speed (INEEDMD_CPU_SPEED_FULL_INTERNAL);
  bTest_Mode = true;
  return 1;
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iIneedmd_waveform_disable_TestSignal(void)
{
  set_system_speed (INEEDMD_CPU_SPEED_DEFAULT);
  bTest_Mode = false;
  return 1;
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
bool iIneedmd_is_test_running(void)
{
  return bTest_Mode;
}

/* ineedmd_measurement_ramp
 * Sends a set of measurement ramps... full scale..
 *
 * This function simply outputs a set of saw tooth measurement packets to test the display.
 */
void ineedmd_measurement_ramp(void)
{
#define PACKET_LENGTH 0x18

  uint16_t uiSys_Tick_value = 0;//MAP_SysTickValueGet();
  int16_t signed_systick
  unsigned char highbyte = 0;
  unsigned char lowbyte = 0;
  char test_packet[PACKET_LENGTH];

  eBSP_Get_Current_ms(&uiSys_Tick_value);
  signed_systick= uiSys_Tick_value-32767;
  lowbyte = (unsigned char)(0xff & ( signed_systick ));
  highbyte = (unsigned char)(0xff & ( signed_systicks >> 8 ));


  test_packet[0x00] = 0x9C;
  test_packet[0x01] = 0x04;
  test_packet[0x02] = PACKET_LENGTH;
  test_packet[PACKET_LENGTH-1] = 0xC9;


//  for (highbyte= 0; highbyte < 0xFF; highbyte++)
//  {
//    for (lowbyte= 0; lowbyte < 0xFF; lowbyte++)
//    {
      //RA measurements
      test_packet[0x03] = highbyte;
      test_packet[0x04] = lowbyte;
      //LA measurements
      test_packet[0x05] = highbyte;
      test_packet[0x06] = lowbyte;
      //RL measurements
      test_packet[0x07] = highbyte;
      test_packet[0x08] = lowbyte;
      //LL measurements
      test_packet[0x09] = highbyte;
      test_packet[0x0A] = lowbyte;
      //V1 measurements
      test_packet[0x0B] = highbyte;
      test_packet[0x0C] = lowbyte;
      //V2 measurements
      test_packet[0x0D] = highbyte;
      test_packet[0x0E] = lowbyte;
      //V3 measurements
      test_packet[0x0F] = highbyte;
      test_packet[0x10] = lowbyte;
      //V4 measurements
      test_packet[0x11] = highbyte;
      test_packet[0x12] = lowbyte;
      //V5 measurements
      test_packet[0x13] = highbyte;
      test_packet[0x14] = lowbyte;
      //V6 measurements
      test_packet[0x15] = highbyte;
      test_packet[0x16] = lowbyte;

      ineedmd_radio_send_frame((uint8_t *)test_packet, PACKET_LENGTH);

//    }
//  }
}
#endif //#ifdef NOT_NOW

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

eEKG_Task_state eIneedmd_EKG_get_task_state(void)
{
  return eCurrent_EKG_Task_state;
}

ERROR_CODE eIneedmd_EKG_request_param_init(tINMD_EKG_req * ptRequest)
{
  ERROR_CODE eEC = ER_FAIL;

  //verify that the pointer to use is valid
  if(ptRequest == NULL)
  {
    //pointer is invalid
    eEC = ER_PARAM;
  }
  else
  {
    //pointer is valid
    //
    //Init params
    ptRequest->eReq_ID = EKG_REQUEST_NONE;
    ptRequest->vEKG_read_callback = NULL;


    //Check if params were properly set
    if((ptRequest->eReq_ID == EKG_REQUEST_NONE)  &\
       (ptRequest->vEKG_read_callback == NULL))
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

ERROR_CODE eIneedmd_EKG_request(tINMD_EKG_req * ptRequest)
{
  ERROR_CODE eEC = ER_FAIL;

  if(ptRequest == NULL)
  {
    eEC = ER_PARAM;
  }
  else
  {
    //check if the request ID is valid
    if((ptRequest->eReq_ID >= EKG_REQUEST_LIMIT) |\
       (ptRequest->eReq_ID == EKG_REQUEST_NONE))
    {
      vDEBUG("eIneedmd_EKG_request request invalid");
      eEC = ER_INVALID;
    }
    else
    {
      switch(ptRequest->eReq_ID)
      {
        case EKG_REQUEST_EKG_HALT:
        {
          //todo do EKG halt
          if(ptRequest->vEKG_read_callback != NULL)
          {
            ptRequest->vEKG_read_callback(0,false);
          }
          eEC = ER_OK;
          break;
        }
        case EKG_REQUEST_EKG_SHUTDOWN:
        {
          //todo do EKG shut down
          if(ptRequest->vEKG_read_callback != NULL)
          {
            ptRequest->vEKG_read_callback(0,false);
          }
          eEC = ER_OK;
          break;
        }
        case EKG_REQUEST_SHORTING_BAR:
        case EKG_REQUEST_DFU:
        case EKG_REQUEST_SHIPPING_HOLD:
        case EKG_REQUEST_TEST_PATTERN:
        case EKG_REQUEST_EKG_MONITOR:
        default:
          vDEBUG("eIneedmd_EKG_request request not implemented yet");
          eEC = ER_REQUEST;
          break;
      }
    }
  }
  return eEC;
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
void vIneedmd_waveform_task(UArg a0, UArg a1)
{
  ERROR_CODE eEC = ER_FAIL;

  eEC = eADC_setup();
  if(eEC == ER_FAIL)
  {
    vDEBUG("vIneedmd_waveform_task SYS HALT, ADC setup failure!");
    while(1){};
  }else{/*do nothing*/}

  while(1)
  {
    Task_sleep(1000);
#ifdef NOT_NOW
    if(bTest_Mode == true)
    {
      bDid_sys_tick = bWaveform_did_timer_tick();

      if(bDid_sys_tick == true)
      {
        ineedmd_measurement_ramp();
      }
    }
#endif
  }
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/

#endif //__INEEDMD_WAVEFORM_C__
