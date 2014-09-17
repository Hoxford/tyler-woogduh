//*****************************************************************************
//
// main.c - aerosmith main.c file
//
// Copyright (c) 2012-2014 oodg.  All rights reserved.
//
//*****************************************************************************

//*****************************************************************************
// includes
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c1233h6pm.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_nvic.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"

#include "utils_inc/error_codes.h"
#include "board.h"
#include "ineedmd_adc.h"
#include "battery.h"
#include "ineedmd_bluetooth_radio.h"

#include "utils_inc/clock.h"
#include "utils_inc/proj_debug.h"
#include "utils_inc/file_system.h"
#include "ff.h"

#include "app_inc/ineedmd_command_protocol.h"
#include "app_inc/ineedmd_UI.h"
#include "app_inc/ineedmd_watchdog.h"
#include "app_inc/ineedmd_waveform.h"

//*****************************************************************************
// defines
//*****************************************************************************
#define WD_TIMEOUT  5  //Watch dog time out value in secons

#define iIneedmd_connection_process  iIneedMD_radio_process
/*
#define LEAD_SHORT                   0x82FF
#define LEAD_SHORT_RESET             0xA0FF
#define LEAD_SHORT_SLEEP             0xA2FF
*/

//*****************************************************************************
// variables
//*****************************************************************************

unsigned char ledState = 0;

#ifdef DEBUG
uintmax_t  uiIm_awake_timer;
bool       bDid_im_awake = false;
uintmax_t  uiIm_asleep_timer;
bool       bDid_im_asleep = false;
#endif

//*****************************************************************************
// external variables
//*****************************************************************************

//*****************************************************************************
// enums
//*****************************************************************************

//*****************************************************************************
// structures
//*****************************************************************************
sFileSys sFile_Sys;

//*****************************************************************************
// private function declarations
//*****************************************************************************
void switch_on_adc_for_lead_detection(void);
void check_for_update(void);

//*****************************************************************************
// external functions
//*****************************************************************************
//extern void PowerInitFunction(void);

//*****************************************************************************
// functions
//*****************************************************************************

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void switch_on_adc_for_lead_detection(void)
{
  //switch on the SPI port
  EKGSPIEnable();

  GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, INEEDMD_PORTA_ADC_nCS_PIN);

  //when done set the CS high
  ineedmd_adc_Start_Low();
  //power on ADC, disable continuous conversions
  ineedmd_adc_Power_On();
  //turn off continuous conversion for register read/writes
  ineedmd_adc_Stop_Continuous_Conv();

  //id = INEEDMD_ADC_Get_ID();
  ineedmd_adc_Enable_Lead_Detect();
  //increase comparator threshold for lead off detect
  uint32_t regVal = ineedmd_adc_Register_Read(LOFF);
  ineedmd_adc_Register_Write(LOFF, (regVal | ILEAD_OFF0 | ILEAD_OFF1));
  //check that we can read the device if not reset

  //start conversions
  ineedmd_adc_Start_Internal_Reference();
  ineedmd_adc_Start_High();

  return;
}


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
  uint32_t i = 200;

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
  MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);

  // Configure and enable the Timer interrupt for timer timeout
  MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  MAP_IntEnable(INT_TIMER0A);

  // Slow the system speed to conserve power
  set_system_speed (INEEDMD_CPU_SPEED_SLOW_INTERNAL);
  uiCurrent_sys_clock = MAP_SysCtlClockGet();

  //Set the timer to timeout based on the new system speed
  MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, uiCurrent_sys_clock);

  // Clear the timer set vale and enable the timer
  eBSP_Did_Timer0_Expire(true);
  MAP_TimerEnable(TIMER0_BASE, TIMER_A);


  eEC_did_timer_expire = eBSP_Did_Timer0_Expire(true);
  while(eEC_did_timer_expire == ER_FALSE)
  {
    iHW_delay(1);
    eEC_did_timer_expire = eBSP_Did_Timer0_Expire(true);
  }

  //set the system speed to what it was originally
  set_system_speed (uiPrevious_speed);

  MAP_IntDisable(INT_TIMER0A);
  MAP_TimerDisable(TIMER0_BASE, TIMER_A);

  switch_on_adc_for_lead_detection();

  uiADC_warmup = ineedmd_adc_Check_Lead_Off();
  uiADC_warmup &= 0x000000FF;
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
//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void check_for_update(void)
{
//
//  bool bUSB_plugged_in = false;
//  uint32_t ui32SysClock = MAP_SysCtlClockGet();
//  USBPortEnable();
//  MAP_SysCtlDelay(ui32SysClock);
////  sleep_for_tenths(10);
//
//  //check if there is a USB data connection attached
//  bUSB_plugged_in = bIs_usb_physical_data_conn();
//  if(bUSB_plugged_in == true)
//  {
//
//
//    volatile uint32_t ui32Loop;
//    uint16_t uiLead_status;
//  //check the state of the short on the ekg connector
//  //checks the short on the update_pin GPIO
//  //if short call the map_usbupdate() to force a USB update.
//    uiLead_status = ineedmd_adc_Check_Lead_Off();
//#define LEAD_SHORT 0x82FF
//  if(uiLead_status == LEAD_SHORT)
//  {
//      ineedmd_led_pattern(DFU_MODE);
//
//      //give the watch dog a long timeout, woof
//      ineedmd_watchdog_feed();
//      set_system_speed(INEEDMD_CPU_SPEED_FULL_EXTERNAL);
//      MAP_SysCtlDelay(ui32SysClock);
//      //proc the rom usb DFU
//      ROM_UpdateUSB(0);
//
//      //Should never get here since update is in progress
//      while(1){};
//  }
//  else
//  {
//    USBPortDisable();
//    return;
//  }
//  }
//  else
//  {
//    USBPortDisable();
//    return;
//  }
  uint32_t uiLead_status;
  bool bUSB_plugged_in = false;

  switch_on_adc_for_lead_detection();

  //turn on the USB port
  USBPortEnable();
  iHW_delay(1000);

  //Check if the firmware update jumper is in place
  uiLead_status = ineedmd_adc_Check_Lead_Off();
  if(uiLead_status == LEAD_SHORT)
  {
    vDEBUG("Update short in place!");
    //check if there is a USB data connection attached
    bUSB_plugged_in = bIs_usb_physical_data_conn(true);
    if(bUSB_plugged_in == true)
    {
      vDEBUG("USB data connection detected!");
      vDEBUG("Device going into update mode");

#define SYSTICKS_PER_SECOND 100
      ROM_FPULazyStackingEnable();
      MAP_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL  | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_INT_OSC_DIS);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
      MAP_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_4);
//      uint32_t ui32SysClock = MAP_SysCtlClockGet();
//      MAP_SysTickPeriodSet(MAP_SysCtlClockGet() / SYSTICKS_PER_SECOND);
//      MAP_SysTickIntEnable();
//      MAP_SysTickEnable();
//      eMaster_int_disable();
//      MAP_SysTickIntDisable();
      MAP_SysTickDisable();
      HWREG(NVIC_DIS0) = 0xffffffff;
      HWREG(NVIC_DIS1) = 0xffffffff;
      // 1. Enable USB PLL
      // 2. Enable USB controller
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
      MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);
      MAP_SysCtlUSBPLLEnable();
      // 3. Enable USB D+ D- pins
      // 4. Activate USB DFU
//      MAP_SysCtlDelay(ui32SysClock / 3);

      // Re-enable interrupts at NVIC level
      eMaster_int_enable();

      //set the led's to DFU mode
//      ineedmd_led_pattern(ACTUAL_DFU);
      eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_ACTUAL_DFU, SPEAKER_SEQ_NONE, true);

      set_system_speed(INEEDMD_CPU_SPEED_FULL_EXTERNAL);
      //begin the DFU usb update procedure
      ROM_UpdateUSB(0);

      //should never reach this but if so sys will reboot
      ineedmd_watchdog_doorbell();

      while(1);
    }
  }
  USBPortDisable();
}

void check_for_reset(void)
{
  //uint32_t uiLead_status = ineedmd_adc_Check_Lead_Off();
  if(ineedmd_adc_Check_Lead_Off() == LEAD_SHORT_RESET)
  {
    vDEBUG("Reset short in place!");
    vDEBUG("Device going into reset");
//    ineedmd_led_pattern(REBOOT);
    eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_REBOOT, SPEAKER_SEQ_NONE, true);
    //should never reach this but if so sys will reboot
    ineedmd_watchdog_doorbell();
    while(1);
  }
}

//check for the glove status
ERROR_CODE ineedmd_ekg_connected(void)
{
  //if there is a glove connected we return a connection else we return a not connected
  uint32_t uiLead_status = 0;

  uiLead_status = ineedmd_adc_Check_Lead_Off();
  if(uiLead_status == LEAD_ALL_SHORTED)
  {
    return ER_CONNECTED;
  }
  else
  {
    return ER_NOT_CONNECTED;
  }
}

//*****************************************************************************
// name: main
// description: main function start point for aerosmith
// param description: none
// return value description: int
//*****************************************************************************
int main(void)
{
#define DEBUG_main
#ifdef DEBUG_main
  #define  vDEBUG_MAIN  vDEBUG
#else
  #define vDEBUG_MAIN(a)
#endif

  //init the debug interface
  vDEBUG_init();
  vDEBUG_MAIN("Hello World!");

  //init the board
  iBoard_init();

//  ineedmd_led_pattern(LED_OFF);
  eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_OFF, SPEAKER_SEQ_NONE, true);

  //Put the system into low power mode if the shipping jumper is present
  hold_until_short_removed();

  //set up the watchdog
  vDEBUG_MAIN("Watchdog setup");
  ineedmd_watchdog_setup();
  //give the watch dog a long timer to allow inital setup to take place
  ineedmd_watchdog_feed();

  //if during system reboot and the usb cable is plugged into a data connection the device will go into DFU
  vDEBUG_MAIN("checking for update");
  check_for_update();

  vDEBUG_MAIN("initial batt check");
  check_battery();

  //mount the file system
//  iFileSys_mount(&sFile_Sys, 0, 1);

  //set up the A to D converter
  vDEBUG_MAIN("A to D setup");
  iADC_setup();

  //set up the module radio
  vDEBUG_MAIN("Radio setup");
  iIneedMD_radio_setup();

  //init processes, there should be no hardware init done past this point
  //
  eClock_process_init();
  eIneedmd_UI_process_init();
  eIneedMD_radio_process_init();

  eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_POWER_UP_GOOD, SPEAKER_SEQ_NONE, false);

  vDEBUG_MAIN("Starting super loop");
  while(1)
  {
    while ((ineedmd_usb_connected() == ER_NOT_CONNECTED) & \
           (ineedmd_ekg_connected() == ER_NOT_CONNECTED))
    {
      ineedmd_watchdog_pat();
      ineedmd_sleep();
      check_battery();
#ifdef DEBUG
      eClock_process();
      eClock_get_time(&uiIm_asleep_timer);
      uiIm_asleep_timer &= 0x00000000000FF000;
      if(uiIm_asleep_timer >= 0x0001E000)
      {
        if(bDid_im_asleep == false)
        {
          vDEBUG_MAIN("I'm asleep and running");
          bDid_im_asleep = true;
        }else{/*do nothing*/}
      }
      else
      {
        bDid_im_asleep = false;
      }
#endif
    }
    ineedmd_watchdog_pat();
    iIneedMD_radio_process();
    iIneedmd_command_process();
    iIneedmd_waveform_process();
    eClock_process();
    eIneedmd_UI_process();
    check_battery();
//    check_for_update();
//    check_for_reset();
#ifdef DEBUG
    eClock_get_time(&uiIm_awake_timer);
    uiIm_awake_timer &= 0x00000000000FF000;
    if(uiIm_awake_timer >= 0x0001E000)
    {
      if(bDid_im_awake == false)
      {
        vDEBUG_MAIN("I'm awake and running");
        bDid_im_awake = true;
      }else{/*do nothing*/}
    }
    else
    {
      bDid_im_awake = false;
    }
#endif
  }

  //todo debug and possible reset
   return 1;

#undef vDEBUG_MAIN
}
