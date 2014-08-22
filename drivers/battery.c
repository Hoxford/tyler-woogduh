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

#include "inc/hw_memmap.h"
#include "inc/tm4c1233h6pm.h"

#include "driverlib/rom_map.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"

#include "ineedmd_led.h"
#include "ineedmd_adc.h"

#include "battery.h"
#include "board.h"

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

/*
****************************************************************************
* name: check_battery
* description: checks the state of the battery. If the battery is low the
*  function parks the bus and clocks the processor low.
* param description: none
* return value description: none
*****************************************************************************
*/
void
check_battery(void)
{

  bool bIs_batt_low;

  //check the state of the low battery pin and if it is low the we park the bus...
  // There are 2 possible battery low indications..
  //          the bat_low pin from the radio
  //          The ADC that measures the battery voltage


  bIs_batt_low = bIs_battery_low();
  if(measure_battery()>BATTERY_LOW_ADC_VALUE )
  {
    bIs_batt_low = true;
    ineedmd_led_pattern(POWER_ON_BATT_GOOD);
  }
  if(measure_battery()<BATTERY_CRITICAL_ADC_VALUE )
  {
    bIs_batt_low = true;
    ineedmd_led_pattern(POWER_ON_BATT_LOW);
  }

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

    //check battery low status
    bIs_batt_low = bIs_battery_low();

    //loop until the battery is no longer in low power
    while (bIs_batt_low == true)
    {
      iHW_delay(1000);
      bIs_batt_low = bIs_battery_low();
    }
  }


}

uint32_t
measure_battery()
{
  uint32_t i;
  uint32_t pui32ADCValue;
  uint32_t average_pui32ADCValue;

  if (MAP_SysCtlClockGet()>20000000)
  {
    return 1;
  }
  average_pui32ADCValue = 0;
  for ( i = 0; i <16; i++)
  {
    ADCProcessorTrigger(BATTERY_ADC, 3);
    //
    // Wait for conversion to be completed.
    //
    while(!ADCIntStatus(BATTERY_ADC, 3, false))
    {
    }
    //
    // Clear the ADC interrupt flag.
    //
    ADCIntClear(BATTERY_ADC, 3);
    //
    // Read ADC Value.
    //
    ADCSequenceDataGet(BATTERY_ADC, 3, &pui32ADCValue);
    average_pui32ADCValue = average_pui32ADCValue+ pui32ADCValue;
  }
  average_pui32ADCValue = average_pui32ADCValue>>4;

  return average_pui32ADCValue;
}
