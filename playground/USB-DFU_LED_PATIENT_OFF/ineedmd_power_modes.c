/*
 * ineedmd_power_modes.c
 *
 *  Created on: Jul 30, 2014
 *      Author: BrianS
 */
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "utils_inc/proj_debug.h"

#define debug_printf vDEBUG

void
go_to_sleep(int number_second){

	debug_printf("going to sleep");
	// power down the radio
	// power down the ADC
<<<<<<< HEAD
	if (EKGSPIDisable()==1)
	{
		debug_printf("..EKG ADC asleep");
	}
	// led's off  controller powers itself down 5ms after LEDS are off
	ineedmd_led_pattern(LED_OFF);
	LEDI2CDisable();
	// power down ADC

	//if (BatMeasureADCDisable()==1)
	//{
	//	debug_printf("..Battery measurement asleep");
	//}

	USBPortDisable();
=======
	// led's off

	// power down the unused ports

	// go to a slow clock
>>>>>>> 5ce5769234c3a4927c64794d74e51524ff8b3a80

	// start timer

	// and deep sleep.
<<<<<<< HEAD
    ROM_SysCtlDeepSleep();

	    TimerDisable(TIMER0_BASE, TIMER_A);
	    IntDisable(INT_TIMER0A);
	    IntMasterDisable();

=======
	ROM_SysCtlDeepSleep();
>>>>>>> 5ce5769234c3a4927c64794d74e51524ff8b3a80
}

void
wake_up(void){

	debug_printf("waking_up");
	//go to a fast clock

	// power up the gpio

<<<<<<< HEAD
	if (RadioUARTEnable()==1)
	{
		debug_printf("..radio awake");
	}
	// power down the ADC
	if (EKGSPIEnable()==1)
	{
		debug_printf("..EKG ADC awake");
	}
	// enable the I2C bus
	LEDI2CEnable();

	USBPortEnable();

//	if (BatMeasureADCEnable()==1)
//	{
//		debug_printf("..Battery measurement asleep");
//	}
=======
	//power up the ports

	// led's om

	// ADC on

	//Radio on
>>>>>>> 5ce5769234c3a4927c64794d74e51524ff8b3a80

}
