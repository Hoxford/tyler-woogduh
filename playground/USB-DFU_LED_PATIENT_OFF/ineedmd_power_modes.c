/*
 * ineedmd_power_modes.c
 *
 *  Created on: Jul 30, 2014
 *      Author: BrianS
 */

void
go_to_sleep(int number_second){

	debug_printf("going to sleep");
	// power down the radio
	// power down the ADC
	// led's off

	// power down the unused ports

	// go to a slow clock

	// start timer

	// and deep sleep.
	ROM_SysCtlDeepSleep().
}

void
wake_up(void){

	debug_printf("waking_up")
	//go to a fast clock

	// power up the gpio

	//power up the ports

	// led's om

	// ADC on

	//Radio on

}
