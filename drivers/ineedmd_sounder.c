/******************************************************************************
*
* ineedmd_sounder.c - Driver source code for the sounder alarm
* Copyright (c) notice
*
******************************************************************************/
#ifndef __INEEDMD_SOUNDER_C__
#define __INEEDMD_SOUNDER_C__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "utils_inc/proj_debug.h"
#include "utils_inc/error_codes.h"
#include "utils_inc/osal.h"
#include "drivers_inc/ineedmd_sounder.h"

#include "driverlib/gpio.h"
#include "board.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

// Define group ID mappings IE:
// RADIO interface mappings////////////////////////////////
  #define SOUNDER_EXAMPLE_DEF  1  //def description

// RADIO sub group mappings
  #define SOUNDER_EXAMPLE_DEF_AGAIN 1 //def description

// END RADIO interface mappings////////////////////////////

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
uint16_t on_time;
uint16_t off_time;

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

extern Timer_Handle tSounder_pwm_timer;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eSounder_Setup(void);
/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
/******************************************************************************
* name: eSounder_Setup
* description: sets up the sounder module
* param description: none
* return value description: ERROR_CODE - ER_OK:
*                                      - ER_FAIL:
******************************************************************************/
ERROR_CODE eSounder_Setup(void)
{
  ERROR_CODE eEC = ER_FAIL;

  //todo: abstract to board.c
  GPIO_write(EK_TM4C123GXL_PWM_OUTPUT, INEEDMD_PWM_PIN_CLEAR);

  Timer_stop(tSounder_pwm_timer);

  return eEC;
}

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

void vSounder_timer_INT_Service(UArg a0)
{
  static uint8_t timer_state = 1;

  //todo: abstract to board.c

  switch (timer_state)
  {
    case 1:
      GPIO_write(EK_TM4C123GXL_PWM_OUTPUT, INEEDMD_PWM_PIN_SET);
      Timer_stop(tSounder_pwm_timer);
      Timer_setPeriodMicroSecs(tSounder_pwm_timer, on_time);
      Timer_start(tSounder_pwm_timer);
      break;
    case 2:
      GPIO_write(EK_TM4C123GXL_PWM_OUTPUT, INEEDMD_PWM_PIN_CLEAR);
      Timer_stop(tSounder_pwm_timer);
      Timer_setPeriodMicroSecs(tSounder_pwm_timer, off_time);
      Timer_start(tSounder_pwm_timer);
      break;
    case 3:
      GPIO_write(EK_TM4C123GXL_PWM_OUTPUT, INEEDMD_PWM_PIN_SET);
      Timer_stop(tSounder_pwm_timer);
      Timer_setPeriodMicroSecs(tSounder_pwm_timer, on_time);
      Timer_start(tSounder_pwm_timer);
      break;
    case 4:
      GPIO_write(EK_TM4C123GXL_PWM_OUTPUT, INEEDMD_PWM_PIN_CLEAR);
      Timer_stop(tSounder_pwm_timer);
      Timer_setPeriodMicroSecs(tSounder_pwm_timer, off_time);
      Timer_start(tSounder_pwm_timer);
      break;
    case 5:
      GPIO_write(EK_TM4C123GXL_PWM_OUTPUT, INEEDMD_PWM_PIN_SET);
      Timer_stop(tSounder_pwm_timer);
      Timer_setPeriodMicroSecs(tSounder_pwm_timer, on_time);
      Timer_start(tSounder_pwm_timer);
      break;
    case 6:
      GPIO_write(EK_TM4C123GXL_PWM_OUTPUT, INEEDMD_PWM_PIN_CLEAR);
      Timer_stop(tSounder_pwm_timer);
      Timer_setPeriodMicroSecs(tSounder_pwm_timer, off_time);
      Timer_start(tSounder_pwm_timer);
      break;
    case 7:
      GPIO_write(EK_TM4C123GXL_PWM_OUTPUT, INEEDMD_PWM_PIN_SET);
      Timer_stop(tSounder_pwm_timer);
      Timer_setPeriodMicroSecs(tSounder_pwm_timer, off_time);
      Timer_start(tSounder_pwm_timer);
      break;
    case 8:
      GPIO_write(EK_TM4C123GXL_PWM_OUTPUT, INEEDMD_PWM_PIN_CLEAR);
      Timer_stop(tSounder_pwm_timer);
      Timer_setPeriodMicroSecs(tSounder_pwm_timer, (off_time * 2));
      Timer_start(tSounder_pwm_timer);
      timer_state = 0;
      break;
    default:
      timer_state=1;
      break;
  }

  timer_state++;
  return;
}

/******************************************************************************
* name: eSounder_Request_Params_Init
* description:
* param description: type - value: value description (in order from left to right)
*                    bool - true: do action when set to true
* return value description: type - value: value description
******************************************************************************/
ERROR_CODE eSounder_Request_Params_Init(tSounder_Request * pParams)
{
  ERROR_CODE eEC = ER_FAIL;

  pParams->eRequest = SOUNDER_REQ_NONE;
  pParams->uFrequency = 0;


  if(pParams->eRequest != SOUNDER_REQ_NONE)
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
* name: Filename_or_abreviation_funciton
* description:
* param description: type - value: value description (in order from left to right)
*                    bool - true: do action when set to true
* return value description: type - value: value description
******************************************************************************/
ERROR_CODE eSounder_Request(tSounder_Request * pRequest)
{
	//#define DEBUG_eIneedmd_SOUNDER_request


  ERROR_CODE eEC = ER_FAIL;
  switch(pRequest->eRequest)
  {
    case SOUNDER_REQ_SETUP:
      eEC = eSounder_Setup();
      break;

    case SOUNDER_REQ_TEST:

    	  on_time = 750;
    	  off_time = 1500;

          GPIO_write(EK_TM4C123GXL_PWM_OUTPUT, INEEDMD_PWM_PIN_SET);
          Timer_stop(tSounder_pwm_timer);
          Timer_setPeriodMicroSecs(tSounder_pwm_timer, 2500 );
          Timer_start(tSounder_pwm_timer);
          eEC = ER_OK;
          break;

    case SOUNDER_REQ_FREQUENCY:
    	  if (pRequest->uFrequency == 0)
    	  {
    		  eEC = ER_FAIL;
    		  break;
    	  }

    	  on_time=((pRequest->uFrequency/3)*1);
    	  off_time=((pRequest->uFrequency/3)*2);


          GPIO_write(EK_TM4C123GXL_PWM_OUTPUT, INEEDMD_PWM_PIN_SET);
          Timer_stop(tSounder_pwm_timer);
          Timer_setPeriodMicroSecs(tSounder_pwm_timer, pRequest->uFrequency );
          Timer_start(tSounder_pwm_timer);
          eEC = ER_OK;
          break;

    case SOUNDER_REQ_HALT:
      GPIO_write(EK_TM4C123GXL_PWM_OUTPUT, INEEDMD_PWM_PIN_CLEAR);
      Timer_stop(tSounder_pwm_timer);
      eEC = ER_OK;
      break;
    default:
      eEC = ER_FAIL;
  }
  return eEC;
#undef DEBUG_eIneedmd_SOUNDER_request
}

#endif //__INEEDMD_SOUNDER_C__
