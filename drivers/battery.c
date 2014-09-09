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
#include "board.h"

#include "battery.h"

#include "utils_inc/error_codes.h"


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
  //  todo: bIs_battery_low() not implemented in board rev03 anymore, add back in rev 05
#ifdef BAT_LOW_NOT_IMPLEMENTED
  bool bIs_batt_low;

  //check the state of the low battery pin and if it is low the we park the bus...
  // There are 2 possible battery low indications..
  //          the bat_low pin from the radio
  //          The ADC that measures the battery voltage

<<<<<<< HEAD

//  bIs_batt_low = bIs_battery_low();
=======
  bIs_batt_low = bIs_battery_low();
>>>>>>> cc88825822b5b852d324d7ff44cd40865625d7ee
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
#endif

  return;
}

uint32_t
measure_battery()
{
  uint32_t i;
  uint32_t pui32ADCValue;
  uint32_t average_pui32ADCValue;

  if (BatMeasureADCEnable() != ER_OK)
  {
    return 1;
  }
  if (MAP_SysCtlClockGet()<200000)
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

  BatMeasureADCDisable();

  return average_pui32ADCValue;
}

char
ineedmd_get_battery_voltage(void)
{
  uint32_t offset_battery_voltage_in_tenths;
  uint32_t battery_voltage_in_tenths;
  uint32_t mv_battery;

  //convert the ADC reading to 10uV steps
  mv_battery = measure_battery() * 162;
  //scale the 10uv steps to tenths of volts

  battery_voltage_in_tenths = mv_battery / 1000;

  //suptract the offset of 2.5V
  offset_battery_voltage_in_tenths = battery_voltage_in_tenths - 250;

  return (char) (0xff & offset_battery_voltage_in_tenths);

}

uint32_t
ineedmd_get_unit_tempoerature()
{


  uint32_t TempValueC;
  uint32_t temp_adc_return_value;

  TemperatureMeasureADCEnable();

  ADCProcessorTrigger(TEMPERATURE_ADC, 3);
  //
  // Wait for conversion to be completed.
  //
  while(!ADCIntStatus(TEMPERATURE_ADC, 3, false))
  {
  }
  //
  // Clear the ADC interrupt flag.
  //
  ADCIntClear(TEMPERATURE_ADC, 3);
  //
  // Read ADC Value.
  //
  ADCSequenceDataGet(TEMPERATURE_ADC, 3, &temp_adc_return_value);

  TemperatureMeasureADCDisable();
  TempValueC = ((1475 * 1023) - (2250 * temp_adc_return_value)) / 10230;

  return TempValueC;

}
