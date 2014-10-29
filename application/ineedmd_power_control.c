/*
 * ineedmd_power_control.c
 *
 *  Created on: Jul 30, 2014
 *      Author: BrianS
 */
#ifndef __INEEDMD_POWER_CONTROL_C__
#define __INEEDMD_POWER_CONTROL_C__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
//#include "utils_inc/proj_debug.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/usb.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#include "utils_inc/error_codes.h"

#include "drivers_inc/ineedmd_adc.h"
#include "inc/tm4c1233h6pm.h"
#include "board.h"
#include "utils_inc/proj_debug.h"
#include "app_inc/ineedmd_power_control.h"
#include "app_inc/ineedmd_UI.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#define debug_printf vDEBUG

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
uint32_t uiCurrent_Supply_Voltage = 3500;  //todo: init to 0 when power control fully implemented
/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

ePower_Control_state eTask_state = PWR_CTRL_TASK_NONE;

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
/******************************************************************************
* name:
* description:
* param description: type - value: value description (in order from left to right)
*                    bool - true: do action when set to true
* return value description: type - value: value description
******************************************************************************/
//int example_PrivateFunction(param_1, param_2)
//{
//}

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* name: uiPower_Control_Get_Supply_Voltage
*
* description: returns a unisgned int vale of the current system supply voltage
*
* param description: none
*
* return value description: int - 0: system voltage not read yet
*                               - -1: system voltage read failure
*                               - integer: current voltage value in millivolts
******************************************************************************/
int uiPower_Control_Get_Supply_Voltage(void)
{
  int iSupply_Voltage = 0;
  ePower_Control_state eState;

  //check the current supply voltage value
  if(uiCurrent_Supply_Voltage == 0)
  {
    //supply voltage is not a valid value
    //
    //get and check the current power control task state
    eState = ePower_Control_Get_Task_State();
    if(eState == PWR_CTRL_TASK_NONE)
    {
      //The power control task has not started yet and the supply voltage has not been read yet
      //set the return value to not read yet
      iSupply_Voltage = 0;
    }
    else
    {
      //The power control task has started but there was an error reading the supply voltage
      //set the return value to error
      iSupply_Voltage = -1;
    }

  }
  else
  {
    //supply voltage is a valid value
    //
    //Set the return value to the current supply voltage value
    iSupply_Voltage = (int)uiCurrent_Supply_Voltage;
  }

  return iSupply_Voltage;
}

/******************************************************************************
* name: ePower_Control_Get_Task_State
* description: returns the current state of the power control task
* param description: type - value: value description (in order from left to right)
*                    bool - true: do action when set to true
* return value description: type - value: value description
******************************************************************************/
ePower_Control_state ePower_Control_Get_Task_State(void)
{
  return eTask_state;
}

#ifdef OLD_CODE
void shut_it_all_down()
{
//#define DEBUG_shut_it_all_down
#ifdef DEBUG_shut_it_all_down
  #define  vDEBUG_SHUT_DWN  debug_printf
#else
  #define vDEBUG_SHUT_DWN(a)
#endif
       debug_printf("going to sleep");
        // power down the radio
        if (iRadio_Power_Off()==1)
        {
          vDEBUG_SHUT_DWN("..radio asleep");
        }
        // power down the ADC
        if (EKGSPIDisable()==1)
        {
          vDEBUG_SHUT_DWN("..EKG ADC asleep");
        }
        // led's off  controller powers itself down 5ms after LEDS are off
        eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_OFF, SPEAKER_SEQ_NONE, true);
        LEDI2CDisable();
        // power down ADC

//        if (BatMeasureADCDisable()==1)
//        {
//          vDEBUG_SHUT_DWN("..Battery measurement asleep");
//        }

//        USBPortDisable();
#undef vDEBUG_SHUT_DWN
}


void sleep_for_tenths(int number_tenths_seconds)
{
//#define DEBUG_sleep_for_tenths
#ifdef DEBUG_sleep_for_tenths
  #define  vDEBUG_SLEEP_10THS  debug_printf
#else
  #define vDEBUG_SLEEP_10THS(a)
#endif
//#ifdef SLEEP_FOR_TENTHS_TO_BE_REMOVED
  uint16_t uiPrev_sys_speed = 0;
  uint16_t uiCurr_sys_speed = 0;

  //Get the current system speed
  eGet_system_speed(&uiPrev_sys_speed);
  // start timer
  //
  // Set the Timer0B load value to 10s.
  //
//  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
//  eMaster_int_enable();
//  ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
//  TimerLoadSet(TIMER0_BASE, TIMER_A, (50000 * number_tenths_seconds) );

  //
  // Enable processor interrupts.
  //
  eMaster_int_enable();
  //
  // Configure the Timer0 interrupt for timer timeout.
  //
//  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  //
  // Enable the Timer0A interrupt on the processor (NVIC).
  //
//  IntEnable(INT_TIMER0A);
  //
  // clocks down the processor to REALLY slow ( 500khz) and
  //
  // go to a slow clock
  uiCurr_sys_speed = uiPrev_sys_speed;
//todo: this causes problems with the led patterns and ineedmd protocl implementation making the system unresponsive
//    if (set_system_speed (INEEDMD_CPU_SPEED_SLOW_INTERNAL) == INEEDMD_CPU_SPEED_SLOW_INTERNAL)
//    {
//      vDEBUG_SLEEP_10THS("..CPU slow");
//      uiCurr_sys_speed = INEEDMD_CPU_SPEED_SLOW_INTERNAL;
//    }

  uiCurr_sys_speed = set_system_speed (INEEDMD_CPU_SPEED_HALF_INTERNAL_OSC);

  //
  // Enable Timer0(A)
  //
//  TimerEnable(TIMER0_BASE, TIMER_A);

  // and deep sleep.
  ROM_SysCtlDeepSleep();

//  TimerDisable(TIMER0_BASE, TIMER_A);
//  IntDisable(INT_TIMER0A);
  vDEBUG_SLEEP_10THS("waking_up");

  //restore the systemclock
  if(uiCurr_sys_speed != uiPrev_sys_speed)
  {
    set_system_speed(uiPrev_sys_speed);
  }

  eMaster_int_enable();
//#endif //#ifdef SLEEP_FOR_TENTHS_TO_BE_REMOVED
#undef vDEBUG_SLEEP_10THS
}

/* ***************************************************
 * sleeps for 10seconds on low power mode
 *
 * this is designed for the led4 sleep with a flash of
 * the battery state while in sleep
 *****************************************************/
void ineedmd_sleep(void)
{
  ERROR_CODE eEC = ER_OK;
  ERROR_CODE eEC_did_timer_expire = ER_FALSE;
  uint32_t uiCurrent_sys_clock = 0;
  uint32_t uiADC_warmup = 0;
  uint32_t i = 0;

  uint16_t uiPrevious_speed = 0;

  //get the current system speed index
  eEC = eGet_system_speed(&uiPrevious_speed);
  if(eEC == ER_NOT_SET)
  {
    uiPrevious_speed = set_system_speed (INEEDMD_CPU_SPEED_DEFAULT);
  }else{/* do nothing */}

  //shut down the LED's
  eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_OFF, SPEAKER_SEQ_NONE, true);

  //disable the spi port
  EKGSPIDisable();

  //power down the ADC
  ineedmd_adc_Power_Off();

  //turn the radio off
  iRadio_Power_Off();

  // Set a timer to wake the processor out of sleep at a specified interval
  //
  //Enable the timer
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
//  SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER0);

  //Config the timer to be a single shot
  MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

  // Configure and enable the Timer interrupt for timer timeout
  MAP_IntEnable(INT_TIMER0A);
  MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  // Slow the system speed to conserve power
  set_system_speed (INEEDMD_CPU_SPEED_SLOW_INTERNAL);
  uiCurrent_sys_clock = MAP_SysCtlClockGet();

  //Set the timer to timeout based on the new system speed
  MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, uiCurrent_sys_clock);

  // Clear the timer set vale and enable the timer
  eEC_did_timer_expire = eBSP_Did_Timer0_Expire(true);
  MAP_TimerEnable(TIMER0_BASE, TIMER_A);


  eEC_did_timer_expire = eBSP_Did_Timer0_Expire(true);
  i = 0;
  while(eEC_did_timer_expire == ER_FALSE)
  {
    i += iHW_delay(1);
    if(i >= 1000)
    {
      break;
    }
    eEC_did_timer_expire = eBSP_Did_Timer0_Expire(true);
  }

  //set the system speed to what it was originally
  set_system_speed (uiPrevious_speed);

  MAP_IntDisable(INT_TIMER0A);
  MAP_TimerDisable(TIMER0_BASE, TIMER_A);

  switch_on_adc_for_lead_detection();

  uiADC_warmup = ineedmd_adc_Check_Lead_Off();
  uiADC_warmup &= 0x000000FF;
  i = 200;
  while(uiADC_warmup != 0x000000FF)
  {
    ineedmd_adc_Power_Off();
    switch_on_adc_for_lead_detection();
    iHW_delay(i);
    i+= 100;
    uiADC_warmup = ineedmd_adc_Check_Lead_Off();
    uiADC_warmup &= 0x000000FF;
  }

  //power the radio back on
  iRadio_Power_On();

  //re-enable master interrupts
  eMaster_int_enable();
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
void hold_until_short_removed(void)
{
  ERROR_CODE eEC = ER_OK;
  uint32_t uiLead_check = 0;
  uint16_t uiPrevious_speed = 0;

  eEC = eGet_system_speed(&uiPrevious_speed);
  if(eEC == ER_NOT_SET)
  {
    uiPrevious_speed = INEEDMD_CPU_SPEED_FULL_INTERNAL;
    set_system_speed (uiPrevious_speed);
  }else{/*nothing*/}

  switch_on_adc_for_lead_detection();

  uiLead_check = ineedmd_adc_Check_Lead_Off();

  while(uiLead_check == LEAD_SHORT_SLEEP)
  {
      //disable the spi port
    EKGSPIDisable();
    //power down the ADC
    ineedmd_adc_Power_Off();

    //
    // Set the Timer0B load value to 10s.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    eMaster_int_enable();
    MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, 5000000 );

    //
    // Enable processor interrupts.
    //todo: redundant?
    eMaster_int_enable();
    //
    // Configure the Timer0 interrupt for timer timeout.
    //
    MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //
    // Enable the Timer0A interrupt on the processor (NVIC).
    //
    MAP_IntEnable(INT_TIMER0A);
    //
    // clocks down the processor to REALLY slow ( 500khz) and
    //
    set_system_speed (INEEDMD_CPU_SPEED_SLOW_INTERNAL);
    //
    // Enable Timer0(A)
    //
    MAP_TimerEnable(TIMER0_BASE, TIMER_A);

    MAP_SysCtlSleep();

    //comming out we turn the processor all the way up
    set_system_speed (INEEDMD_CPU_SPEED_FULL_INTERNAL);

    MAP_TimerDisable(TIMER0_BASE, TIMER_A);
    MAP_IntDisable(INT_TIMER0A);
    eMaster_int_disable();

    switch_on_adc_for_lead_detection();
    iHW_delay(100);

    uiLead_check = ineedmd_adc_Check_Lead_Off();
  }

  //re-enable master interrupts
  eMaster_int_enable();

  //set the system speed to what it was originally
  set_system_speed (uiPrevious_speed);
}
#endif //#ifdef OLD_CODE

#endif //#define __INEEDMD_POWER_CONTROL_C__
