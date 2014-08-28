/*
 * ineedmd_power_modes.c
 *
 *  Created on: Jul 30, 2014
 *      Author: BrianS
 */

#define debug_printf vDEBUG


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

#include "inc/tm4c1233h6pm.h"
#include "board.h"
#include "utils_inc/proj_debug.h"
#include "ineedmd_led.h"


void
shut_it_all_down(){
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
        ineedmd_led_pattern(LED_OFF);
        LEDI2CDisable();
        // power down ADC

//        if (BatMeasureADCDisable()==1)
//        {
//          vDEBUG_SHUT_DWN("..Battery measurement asleep");
//        }

//        USBPortDisable();
#undef vDEBUG_SHUT_DWN
}


void
sleep_for_tenths(int number_tenths_seconds){
//#define DEBUG_sleep_for_tenths
#ifdef DEBUG_sleep_for_tenths
  #define  vDEBUG_SLEEP_10THS  debug_printf
#else
  #define vDEBUG_SLEEP_10THS(a)
#endif

        // start timer
    //
    // Set the Timer0B load value to 10s.
    //

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    ROM_IntMasterEnable();
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (50000 * number_tenths_seconds) );

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();
    //
    // Configure the Timer0 interrupt for timer timeout.
    //
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //
    // Enable the Timer0A interrupt on the processor (NVIC).
    //
    IntEnable(INT_TIMER0A);
    //
    // clocks down the processor to REALLY slow ( 500khz) and
    //
    // go to a slow clock
    if (set_system_speed (INEEDMD_CPU_SPEED_SLOW_INTERNAL) == INEEDMD_CPU_SPEED_SLOW_INTERNAL)
    {
      vDEBUG_SLEEP_10THS("..CPU slow");
    }
    //
    // Enable Timer0(A)
    //
    TimerEnable(TIMER0_BASE, TIMER_A);

    // and deep sleep.
    ROM_SysCtlDeepSleep();

    TimerDisable(TIMER0_BASE, TIMER_A);
    IntDisable(INT_TIMER0A);
    vDEBUG_SLEEP_10THS("waking_up");
            //go to a fast clock
    if(set_system_speed(INEEDMD_CPU_SPEED_HALF_INTERNAL) == INEEDMD_CPU_SPEED_HALF_INTERNAL)
    {
      vDEBUG_SLEEP_10THS("..CPU half speed");
    }
	IntMasterEnable();
#undef vDEBUG_SLEEP_10THS
}
