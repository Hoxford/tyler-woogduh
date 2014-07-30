/*
 * ineedmd_power_modes.c
 *
 *  Created on: Jul 30, 2014
 *      Author: BrianS
 */

#define debug_print(a) vDEBUG(a,__VA_ARGS__)


#include "utils_inc/proj_debug.h"



void
go_to_sleep(int number_tenths_seconds){

	debug_printf("going to sleep");
	// power down the radio
	if (RadioUARTDisable()==1)
	{
		debug_printf(""..radio asleep");
	}
	// power down the ADC
	if (EKGSPIDisable()==1)
	{
		debug_printf(""..EKG ADC asleep");
	}
	// led's off  controller powers itself down 5ms after LEDS are off
	ineedmd_led_pattern(LED_OFF);
	LEDI2CDisable();
	// power down ADC

	if (BatMeasureADCDisable()==1)
	{
		debug_printf(""..Battery measurement asleep");
	}

	USBPortDisable();

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
		debug_printf(""..CPU slow");

	}
    //
    // Enable Timer0(A)
    //
    TimerEnable(TIMER0_BASE, TIMER_A);

	// and deep sleep.
	ROM_SysCtlDeepSleep();
}

void
wake_up(void){

	debug_printf("waking_up")
	//go to a fast clock
    	    // go to a slow clock
    		if (set_system_speed (INEEDMD_CPU_SPEED_HALF_INTERNAL) == INEEDMD_CPU_SPEED_HALF_INTERNAL)
    		{
    			debug_printf(""..CPU half speed");

    		}

	if (RadioUARTEnable()==1)
	{
		debug_printf(""..radio awake");
	}
	// power down the ADC
	if (EKGSPIEnable()==1)
	{
		debug_printf(""..EKG ADC awake");
	}
	// enable the I2C bus
	LEDI2CEnable();

	USBPortEnable();

	if (BatMeasureADCEnable()==1)
	{
		debug_printf(""..Battery measurement asleep");
	}

}
