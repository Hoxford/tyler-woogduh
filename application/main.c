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
#include "app_inc/ineedmd_power_modes.h"

//*****************************************************************************
// defines
//*****************************************************************************
#define WD_TIMEOUT  5  //Watch dog time out value in secons

#define iIneedmd_connection_process  iIneedMD_radio_process

#define ALIVE_ASLEEP 1
#define ALIVE_AWAKE  2
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
//void switch_on_adc_for_lead_detection(void);
void check_for_update(void);

//*****************************************************************************
// external functions
//*****************************************************************************

/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#ifdef DEBUG
void vDEBUG_IM_ALIVE(uint8_t uiAsleep_Awake)
{
  eClock_process();
  if(uiAsleep_Awake == ALIVE_ASLEEP)
  {
    eClock_get_time(&uiIm_asleep_timer);
    uiIm_asleep_timer &= 0x00000000000FF000;
    if(uiIm_asleep_timer >= 0x0001E000)
    {
      if(bDid_im_asleep == false)
      {
        vDEBUG("I'm asleep and running");
        bDid_im_asleep = true;
      }else{/*do nothing*/}
    }
    else
    {
      bDid_im_asleep = false;
    }
  }
  else //ALIVE_AWAKE
  {
    eClock_get_time(&uiIm_awake_timer);
    uiIm_awake_timer &= 0x00000000000FF000;
    if(uiIm_awake_timer >= 0x0001E000)
    {
      if(bDid_im_awake == false)
      {
        vDEBUG("I'm awake and running");
        bDid_im_awake = true;
      }else{/*do nothing*/}
    }
    else
    {
      bDid_im_awake = false;
    }
  }
}
#else
#define vDEBUG_IM_ALIVE(n)
#endif

//*****************************************************************************
// functions
//*****************************************************************************

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
//if there is a glove connected we return a connection else we return a not connected
ERROR_CODE ineedmd_ekg_connected(void)
{
  ERROR_CODE eEC = ER_OK;
  uint32_t uiLead_status = 0;

  eEC = eIneedmd_adc_Power_status();
  if(eEC == ER_OFF)
  {
    ineedmd_adc_Power_On();
  }
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

  bool b_should_init_radio = false;
  uint32_t ubattery_voltage = 0;

  //init the debug interface
  vDEBUG_init();
  vDEBUG_MAIN("Hello World!");

  //init the board
  iBoard_init();

  // Measure the battery voltage so we can make decisions about the boot sequence..
  measure_battery(&ubattery_voltage, true);

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

  //vDEBUG_MAIN("initial batt check");
  //check_battery();

  //eBSP_Radio_Disable();mount the file system
//  iFileSys_mount(&sFile_Sys, 0, 1);

  //set up the A to D converter
  vDEBUG_MAIN("A to D setup");
  iADC_setup();

  //set up the module radio
  //vDEBUG_MAIN("Radio setup");
  //if ( ubattery_voltage > BATTERY_CRITICAL_ADC_VALUE)
  //{
    //the radio needs time to settle before the radio it ready approximately 3 second
    //iIneedMD_radio_setup();
  //}

  //init processes, there should be no hardware init done past this point
  //
  eClock_process_init();
  eIneedmd_UI_process_init();

  //if ( ubattery_voltage > BATTERY_CRITICAL_ADC_VALUE)
    //{
      //eIneedMD_radio_process_init();
      // we havent started the radio so mark it as not strated.
      //b_should_init_radio = false;
    //}

  //todo: blocking LED in the boot ... this slows the boot, but ensures that the sequence is complete.
  eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_POWER_UP_GOOD, SPEAKER_SEQ_NONE, true);


  vDEBUG_MAIN("Starting super loop");
  while(1)
  {
    //start of the sleep loop.
    //Stay in sleep if the battery is low or if we are not connected to an EKG 'glove'
    while ( \
             ((ineedmd_usb_connected() == ER_NOT_CONNECTED) & (ineedmd_ekg_connected() == ER_NOT_CONNECTED)) | \
             ( ubattery_voltage < BATTERY_CRITICAL_ADC_VALUE)\
          )
    {

      ineedmd_watchdog_pat();
      ineedmd_sleep();
      check_battery(&ubattery_voltage, true);

      b_should_init_radio = true;
      vDEBUG_IM_ALIVE(ALIVE_ASLEEP);
    }
    //end of the sleep loop

    //if we were asleep we need to power the radio back up.
    if ( b_should_init_radio == true )
    {
      eBSP_Radio_Enable();
      iRadio_Power_On();
      eBSP_Radio_Reset();
      iIneedMD_radio_setup();
      eIneedMD_radio_process_init();

      b_should_init_radio = false;

    }
    measure_battery(&ubattery_voltage, false);
    //if the battery is low and there is no other UI sequence ...  display the low battery warning...
    if ((ubattery_voltage < BATTERY_LOW_ADC_VALUE) & ( eIneedmd_present_UI_Process() == LED_SEQ_NONE))
    {
      eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_HIBERNATE_LOW, SPEAKER_SEQ_NONE, false);
    }
    ineedmd_watchdog_pat();
    iIneedMD_radio_process();
    iIneedmd_command_process();
    iIneedmd_waveform_process();
    iIneedmd_sysinfo_process();
    eClock_process();
    eIneedmd_UI_process();
//    check_for_update();
//    check_for_reset();
    vDEBUG_IM_ALIVE(ALIVE_AWAKE);
  }

  //todo debug and possible reset
   return 1;

#undef vDEBUG_MAIN
}
