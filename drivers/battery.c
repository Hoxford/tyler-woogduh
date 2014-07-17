//*****************************************************************************
//
// battery.c - contains the driver functions for the battery
//
// Copyright (c) notice
//
//*****************************************************************************

//*****************************************************************************
// includes
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "ineedmd_led.h"
#include "ineedmd_adc.h"

//*****************************************************************************
// defines
//*****************************************************************************

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

//*****************************************************************************
// function declarations
//*****************************************************************************

//*****************************************************************************
// functions
//*****************************************************************************

//*****************************************************************************
// name: check_battery
// description: checks the state of the battery. If the battery is low the
//  function parks the bus and clocks the processor low.
// param description: none
// return value description: none
//*****************************************************************************
void
check_battery(void){
  bool bIs_batt_low;
  //check the state of the low battery pin and if it is low the we park the bus...

  bIs_batt_low = bIs_battery_low();
  //if the input port is not high bat alarm!

//  if( GPIOPinRead(GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_LOW_BATT_INTERUPT) == INEEDMD_PORTE_RADIO_LOW_BATT_INTERUPT)
  if(bIs_batt_low == true)
  {
    // shut down the LEDs
    ineedmd_led_pattern(LED_OFF);
    //stop the conversions
    ineedmd_adc_Start_Low();
    //shut down the reference
    ineedmd_adc_Stop_Internal_Reference();
    // clocks down the processor to REALLY slow ( 30khz) and
    set_system_speed (INEEDMD_CPU_SPEED_REALLY_SLOW);

    //check batter low status
    bIs_batt_low = bIs_battery_low();

    //loop until the battery is no longer in low power
    while (bIs_batt_low == true)
    {
      wait_time(1000);
      bIs_batt_low = bIs_battery_low();
    }
  }
  //comming out we turn the processor all the way up
  set_system_speed (INEEDMD_CPU_SPEED_FULL_INTERNAL);
  //stART the conversions
  ineedmd_adc_Start_High();
  //start down the reference
  ineedmd_adc_Start_Internal_Reference();
  // clocks down the processor to REALLY slow ( 30khz) and



}
