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
#include "board.h"
#include "ineedmd_adc.h"
#include "battery.h"
#include "ineedmd_bluetooth_radio.h"
#include "ineedmd_led.h"

//*****************************************************************************
// defines
//*****************************************************************************
#define WD_TIMEOUT  5  //Watch dog time out value in secons
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

//*****************************************************************************
// external functions
//*****************************************************************************
//extern void PowerInitFunction(void);
//*****************************************************************************
// function declarations
//*****************************************************************************
int iInit_wd(int iWD_timeout);
int iPet_the_dog( void );

//*****************************************************************************
// functions
//*****************************************************************************
//*****************************************************************************
// name: iInit_wd
// description: initalize the watch dog
// param description: int iWD_timeout - timeout value in seconds for the watchdog
// return value description: 1 if successful
//*****************************************************************************
int iInit_wd(int iWD_timeout)
{
  return 1;
}

//*****************************************************************************
// name: iPet_the_dog
// description: resets the watch dog timer to the WDLOAD value
// param description: none
// return value description: 1 if successful
//*****************************************************************************
int iPet_the_dog( void )
{
    return 1;
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
  iInit_wd(WD_TIMEOUT);
  iBoard_init();

//  ineedmd_adc_Start_Low();
//  //power on ADC, disable continuous conversions
//  ineedmd_adc_Power_On();
//  //turn off continuous conversion for register read/writes
//  ineedmd_adc_Stop_Continuous_Conv();
//
//  ineedmd_adc_Enable_Lead_Detect();
//
//  //increase comparator threshold for lead off detect
//  uint32_t regVal = ineedmd_adc_Register_Read(LOFF);
//  ineedmd_adc_Register_Write(LOFF, (regVal | ILEAD_OFF0 | ILEAD_OFF1));
//
//  //start conversions
//  ineedmd_adc_Start_Internal_Reference();
//  ineedmd_adc_Start_High();
  iADC_setup();

  //power on the radio
  bluetooth_setup();

  while(1)
  {
    iPet_the_dog();

#ifdef DEBUG
    led_test();
#endif //DEBUG

    check_battery();
//
//    check_for_update();
  }
  return 1;
}