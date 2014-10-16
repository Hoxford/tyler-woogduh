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

#include "utils_inc/error_codes.h"
#include "app_inc/ineedmd_watchdog.h"
#include "ineedmd_led.h"
#include "app_inc/ineedmd_UI.h"
#include "ineedmd_adc.h"
#include "board.h"

#include "battery.h"

#include "utils_inc/proj_debug.h"

//*****************************************************************************
// defines
//*****************************************************************************
#define MEAS_BATT_READ_COUNT  4
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


/******************************************************************************
* name: check_battery
* description: checks the state of the battery. If the battery is low the
*  function puts the system in a sleep mode and charges the battery.
* param description: none
* return value description: none
******************************************************************************/

void
check_battery(uint32_t * uiBatt_voltage, bool show_leds)
{
#define DEBUG_check_battery
#ifdef DEBUG_check_battery
  #define  vDEBUG_CHK_BATT  vDEBUG
#else
  #define vDEBUG_CHK_BATT(a)
#endif
  //  todo: bIs_battery_low() not implemented in board rev03 anymore, add back in rev 05
  ERROR_CODE eEC = ER_NO;
  ERROR_CODE eEC_Do_Charge = ER_NO;
  bool bIs_batt_critical = false;
  // uint32_t uiBatt_voltage = 0;
  uint16_t uiPrev_sys_speed = 0;
#ifdef DEBUG
  uint16_t uiIm_charging_timer = 0;
#endif

#ifdef RADIO_BAT_LOW_PIN_NOT_IMPLEMENTED
  //check if the radio low battery pin is set
  bIs_batt_low = bIs_battery_low(); //todo: rename this funciton to be more radio specific
  if(bIs_batt_low == true)
  {
    //set the control variable to start charge
    eEC_Do_Charge = ER_YES;
  }
  else
  {
    eEC_Do_Charge = ER_NO;
  }
#endif

  if(eEC_Do_Charge != ER_YES)
  {
    //check the battery voltage via measuring directly
    //
    eEC = measure_battery(uiBatt_voltage, true);
    if(eEC == ER_OK)
    {
      if(*uiBatt_voltage >= BATTERY_LOW_ADC_VALUE )
      {
        //the battery voltage is greater then the low voltage value, sys ok
        bIs_batt_critical = false;
        if ( show_leds == true)
          {
          eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_HIBERNATE_GOOD, SPEAKER_SEQ_NONE, true);
          }

        //todo: change to UI process call ineedmd_led_pattern(POWER_ON_BATT_GOOD);
      }
      else if(*uiBatt_voltage > BATTERY_CRITICAL_ADC_VALUE)
      {
        //the battery voltage is greater then the critical voltage value, sys warning ok
        bIs_batt_critical = false;
        if ( show_leds == true)
          {
          eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_HIBERNATE_MEDIUM, SPEAKER_SEQ_NONE, true);
          }
        //todo: change to UI process call ineedmd_led_pattern(POWER_ON_BATT_LOW);
      }
      else
      {
        //the battery is at or below the critical voltage value, sys warning critical!
        bIs_batt_critical = true;

        // always flash the red LED
        if ( (show_leds == true) | (show_leds == false) )
          {
          eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_HIBERNATE_LOW, SPEAKER_SEQ_NONE, true);
          }
        //todo: led pattern needs to be critical voltage value
        //todo: change to UI process call ineedmd_led_pattern(POWER_ON_BATT_LOW);
      }

      if(bIs_batt_critical == true)
      {
        eEC_Do_Charge = ER_YES;
      }
      else
      {
        eEC_Do_Charge = ER_NO;
      }
    }
    else
    {
#ifdef DEBUG
      if(eEC == ER_NOT_ENABLED)
        vDEBUG_CHK_BATT("Chk Batt, adc to meas batt not en");
      else if(eEC == ER_SPEED)
        vDEBUG_CHK_BATT("Chk Batt, sys speed to low");
#endif
    }
  }else{/*do nothing*/}

  /*
  //  battery voltage is ppassed up and dealt with at a higher level
  //check the error code to determine if the system needs to charge
  if(eEC_Do_Charge == ER_YES)
  {
    vDEBUG_CHK_BATT("Chk Batt, batt critical going into wait for charge state");

    //Get the current system speed
    eGet_system_speed(&uiPrev_sys_speed);

    //Turn the system "off"
    //
    //stop the conversions
    ineedmd_adc_Start_Low();
    //shut down the reference
    ineedmd_adc_Stop_Internal_Reference();

    //turn off the radio
    iRadio_Power_Off();


    // clocks down the processor to REALLY slow
    set_system_speed (INEEDMD_CPU_SPEED_REALLY_SLOW);

    //loop until the battery is no longer in critical low power
    while (uiBatt_voltage < BATTERY_LOW_ADC_VALUE)
    {
      iHW_delay(1000);
      uiBatt_voltage = 0;
      measure_battery(&uiBatt_voltage, true);
      ineedmd_watchdog_feed();
#ifdef DEBUG
      uiIm_charging_timer++;
      if(uiIm_charging_timer == 60)
      {
        vDEBUG_CHK_BATT("Chk Batt, batt charging");
        uiIm_charging_timer = 0;
      }
#endif //#ifdef DEBUG
    }

    //turn the system back "on"
    //
    //set system speed to previous value
    set_system_speed(uiPrev_sys_speed);

    //turn the radio back on
    iRadio_Power_On();

    vDEBUG_CHK_BATT("Chk Batt, batt charged resuming operation");
  }
  */

  return;
#undef vDEBUG_CHK_BATT
}

/******************************************************************************
* name: measure_battery
* description: Measures the system battery. It enables the ADC to the battery
*   then measures the battery. After measuring the battery the ACD is shut
*   down to save power.
* param description:
*   uint32_t * - pointer to the variable to store the batt voltage
* return value description:
*   ERROR_CODE - error code return value for error compensation
*     ER_OK - measure battery successful
*     ER_SPEED - system speed to low to measure the battery
*     ER_NOT_ENABLED - ADC to measure the battery failed to init
******************************************************************************/
ERROR_CODE measure_battery(uint32_t * uiBatt_voltage, bool bForce_measurement)
{
  ERROR_CODE eEC = ER_FAIL;
  uint16_t uiPrevious_sys_speed = 0;
  uint16_t uiCurr_sys_speed = 0;
  uint16_t i = 0;
  uint32_t ui32ADCValue = 0;
  uint32_t ui32ADCValue_Cumulative = 0;
  uint32_t ui32ADCValue_Average = 0;

  //check the current system speed
  //must be high enough to run the adc
  eGet_system_speed(&uiCurr_sys_speed);
  if((uiCurr_sys_speed == INEEDMD_CPU_SPEED_NOT_SET) | \
     (uiCurr_sys_speed == INEEDMD_CPU_SPEED_SLOW_INTERNAL) | \
     (uiCurr_sys_speed == INEEDMD_CPU_SPEED_REALLY_SLOW))
  {
    //if the measurement is forced set the system speed to allow ADC conversions
    if(bForce_measurement == true)
    {
      uiPrevious_sys_speed = uiCurr_sys_speed;
      set_system_speed(INEEDMD_CPU_SPEED_HALF_INTERNAL);
      uiCurr_sys_speed = INEEDMD_CPU_SPEED_HALF_INTERNAL;
      eEC = ER_OK;
    }
    else
    {
      //the system is not running fast enough to run the ADC
      eEC = ER_SPEED;
    }
  }
  else
  {
    uiPrevious_sys_speed = uiCurr_sys_speed;
    eEC = ER_OK;
  }

  if(eEC == ER_OK)
  {
    //enable and check the battery measurement ADC
    eEC = BatMeasureADCEnable();
    if (eEC != ER_OK)
    {
      //the battery measurement ADC failed enabling
      eEC = ER_NOT_ENABLED;
    }
    else
    {
      eEC = ER_OK;
    }
  }

  if(eEC == ER_OK)
  {
    ui32ADCValue = 0;
    ui32ADCValue_Cumulative = 0;
    ui32ADCValue_Average = 0;

    //begin measuring the battery voltage
    for ( i = 0; i <MEAS_BATT_READ_COUNT; i++)
    {
      eEC = eBSP_ADCMeasureBatt(&ui32ADCValue);
      if(eEC == ER_OK)
      {
        ui32ADCValue_Cumulative = ui32ADCValue_Cumulative + ui32ADCValue;
      }
      else
      {
        eEC = ER_FAIL;
        break;
      }
    }

    //check if the battery average voltage over time was successfully measured
    if(eEC == ER_OK)
    {
      ui32ADCValue_Average = ui32ADCValue_Cumulative / MEAS_BATT_READ_COUNT;

      *uiBatt_voltage = ui32ADCValue_Average;
    }
    else
    {
      //the battery voltage over time was not successfully measured, set the
      //measuring voltage variable to 0
      *uiBatt_voltage = 0;
    }

    //disable the battery measuring ADC
    BatMeasureADCDisable();

    if(uiPrevious_sys_speed !=  uiCurr_sys_speed)
    {
      set_system_speed(uiPrevious_sys_speed);
    }else{/*do nothing*/}
  }

  return eEC;
}

char
ineedmd_get_battery_voltage(void)
{
  uint32_t offset_battery_voltage_in_tenths;
  uint32_t battery_voltage_in_tenths;
  uint32_t mv_battery;

  //convert the ADC reading to 10uV steps
  measure_battery(&mv_battery, true);
  mv_battery = mv_battery * 162;
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
//  uint16_t uiPrevious_sys_speed = 0;
//  uint16_t uiCurr_sys_speed = 0;


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
