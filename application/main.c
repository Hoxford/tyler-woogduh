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

#include "board.h"
#include "ineedmd_adc.h"
#include "battery.h"
#include "ineedmd_bluetooth_radio.h"
#include "ineedmd_led.h"
#include "app_inc/ineedmd_command_protocol.h"
#include "app_inc/ineedmd_watchdog.h"
#include "app_inc/ineedmd_waveform.h"
#include "utils_inc/proj_debug.h"
#include "utils_inc/file_system.h"
#include "utils_inc/error_codes.h"
#include "ff.h"

//*****************************************************************************
// defines
//*****************************************************************************
#define WD_TIMEOUT  5  //Watch dog time out value in secons

#define iIneedmd_connection_process  iIneedMD_radio_process
//*****************************************************************************
// variables
//*****************************************************************************

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
// external functions
//*****************************************************************************
//extern void PowerInitFunction(void);
//*****************************************************************************
// function declarations
//*****************************************************************************
void check_for_update(void);

//*****************************************************************************
// functions
//*****************************************************************************

void check_for_update(void)
{
  uint32_t ui32Loop;
  bool bUSB_plugged_in = false;
  USBPortEnable();
  iHW_delay(1000);

  //check if there is a USB data connection attached
  bUSB_plugged_in = bIs_usb_physical_data_conn();
  if(bUSB_plugged_in == true)
  {
    vDEBUG("Device going into update mode");

#define SYSTICKS_PER_SECOND 100
    ROM_FPULazyStackingEnable();
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL  | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_INT_OSC_DIS);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    MAP_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_4);
    uint32_t ui32SysClock = MAP_SysCtlClockGet();
    MAP_SysTickPeriodSet(MAP_SysCtlClockGet() / SYSTICKS_PER_SECOND);
    MAP_SysTickIntEnable();
    MAP_SysTickEnable();
    MAP_IntMasterDisable();
    MAP_SysTickIntDisable();
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
    MAP_SysCtlDelay(ui32SysClock / 3);

    // Re-enable interrupts at NVIC level
    MAP_IntMasterEnable();

    //set the led's to DFU mode
    ineedmd_led_pattern(DFU_MODE);

    //begin the DFU usb update procedure
    ROM_UpdateUSB(0);

    //should never reach this but if so sys will reboot
    ineedmd_watchdog_doorbell();

    while(1);
  }
  USBPortDisable();
}

//*****************************************************************************
// name: main
// description: main function start point for aerosmith
// param description: none
// return value description: int
//*****************************************************************************
int
main(void)
{
  iBoard_init();

  //init the debug interface
  vDEBUG_init();
  vDEBUG("Hello World!");

  ineedmd_led_pattern(LED_OFF);

  //set up the watchdog
  ineedmd_watchdog_setup();
  vDEBUG("Watchdog setup");

  //give the watch dog a long timer to allow inital setup to take place
  ineedmd_watchdog_feed();

  //if during system reboot and the usb cable is plugged into a data connection the device will go into DFU
  vDEBUG("checking for update");
  check_for_update();

//  check_battery();

  //mount the file system
//  iFileSys_mount(&sFile_Sys, 0, 1);

  iADC_setup();

  //set up the module radio
  iIneedMD_radio_setup();

  vDEBUG("Starting super loop");
  while(1)
  {
    ineedmd_watchdog_pat();

    iIneedMD_radio_process();

    iIneedmd_command_process();

    iIneedmd_waveform_process();

//    led_test();
//    check_battery();
//    check_for_update();
  }
  return 1;
}
