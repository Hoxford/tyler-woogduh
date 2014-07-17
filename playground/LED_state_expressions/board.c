//*****************************************************************************
// Copyright (c) 2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// This file was automatically generated by the Tiva C Series PinMux Utility
// Version: 1.0.4
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/uart.h"
#include "driverlib/debug.h"

#include <inc/tm4c1233h6pm.h>


void set_system_speed (unsigned int how_fast)
{

	switch (how_fast) {
		case INEEDMD_CPU_SPEED_FULL_EXTERNAL:
			//WARNING - do not use on first board rev!!!!
			// turn on the external oscillator
			GPIOPinWrite (GPIO_PORTD_BASE, INEEDMD_PORTD_XTAL_ENABLE, INEEDMD_PORTD_XTAL_ENABLE);
			// let it stabalise
			MAP_SysCtlDelay(1000);
			//setting to run on the PLL from the external xtal and switch off the internal oscillator this gives us an 80Mhz clock
			SysCtlClockSet( SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_INT_OSC_DIS);
			//reset up the i2c bus

			break;
		case INEEDMD_CPU_SPEED_FULL_INTERNAL:
			//setting to run on the PLL from the internal clock and switch off the external xtal pads and pin this gives us an 80 Mhz clock
			SysCtlClockSet( SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_MAIN_OSC_DIS);
			// switch off the external oscillator
			GPIOPinWrite (GPIO_PORTD_BASE, INEEDMD_PORTD_XTAL_ENABLE, 0x00);
			break;
		case INEEDMD_CPU_SPEED_SLOW_INTERNAL:
			//setting to run on the  the internal OSC and switch off the external xtal pads and pin.. Setting the divider to run us at 1Mhz
			SysCtlClockSet( SYSCTL_SYSDIV_16 | SYSCTL_USE_OSC | SYSCTL_OSC_INT | SYSCTL_MAIN_OSC_DIS);
			// switch off the external oscillator
			GPIOPinWrite (GPIO_PORTD_BASE, INEEDMD_PORTD_XTAL_ENABLE, 0x00);
			break;
		case INEEDMD_CPU_SPEED_REALLY_SLOW:
			//setting to run on the  the internal OSC and switch off the external xtal pads and pin.. Setting the divider to run us at 30Khz.
			//Communication is't possible.. we are in hibernation
			SysCtlClockSet( SYSCTL_SYSDIV_1 | SYSCTL_OSC_INT30 | SYSCTL_OSC_INT | SYSCTL_MAIN_OSC_DIS);
			// switch off the external oscillator
			GPIOPinWrite (GPIO_PORTD_BASE, INEEDMD_PORTD_XTAL_ENABLE, 0x00);
			break;
		default:
			//setting the intrnal at full speed as the default.
			SysCtlClockSet( SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_MAIN_OSC_DIS);
			// switch off the external oscillator
			GPIOPinWrite (GPIO_PORTD_BASE, INEEDMD_PORTD_XTAL_ENABLE, 0x00);
			break;
	}
	//re set up the i2c bus so it's timings are about correct
    I2CMasterInitExpClk(INEEDMD_LED_I2C, SysCtlClockGet(), true);
	//re set up the UART so it's timings are about correct
    UARTConfigSetExpClk( UART1_BASE, SysCtlClockGet(), 115200, ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE ));


}


void
PortFunctionInit(void)
{

    volatile uint32_t ui32Loop;


	//
    // Enable Peripheral Clocks 
    // These have to be switched on in order... who knew!
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //magic delay to let the blocks stabalise...  the delay is 3000 clock cycles as long as there are no interupts..  or about 1/10th of a seconds.
    // The perpherals are mostly clocked on sub clocks and so the change need to propergate through their blocks before going furthere
    // we dont have timers up yet so just burning CPU clock cycles
    MAP_SysCtlDelay(1000);




    //
    //Setup for the ADC to allow processor to measure the battery voltage.  This is a back up for the radio voltage monitor.
    //It is a feature that would allow us to divorce ourselves from the radio module we are using.
    //

    //Enable the ADC Clock
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    //let is stabalise with a majik delay
    MAP_SysCtlDelay(1000);
    // Enable pin PE3 for ADC AIN0
    //
    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, INEEDMD_PORTE_BATTERY_MEASUREMENT_IN_PIN);
	// Enable pin PA7 for GPIOOutput



    //
    // Configuring the SPI port and pins to talk to the analog front end
    // SPI0_BASE is mapped to INEEDMD_ADC_SPI
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    //no need for a majik delay as we are configuring the GPIO pins as well...
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_PWRDN_OUT_PIN);
    // Enable pin PA6 for GPIOOutput
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_RESET_OUT_PIN);
    // Enable pin PA0 for GPIOInput
    MAP_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_INTERUPT_PIN);
    // Enable pin PA1 for GPIOOutput
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_START_PIN);
    // Enable pin PA2 for SSI0 SSI0CLK
    //
    MAP_GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2);
    //
    // Enable pin PA5 for SSI0 SSI0TX
    //
    MAP_GPIOPinConfigure(GPIO_PA5_SSI0TX);
    MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5);
    //
    // Enable pin PA3 for SSI0 SSI0FSS
    //
    MAP_GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_3);
    //
    // Enable pin PA4 for SSI0 SSI0RX
    //
    MAP_GPIOPinConfigure(GPIO_PA4_SSI0RX);
    MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_4);




	//
    //RADIO_CONFIG
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	//No mjik delay as I am doing the GPIO first.
    //
    // Enable pin PE0 for GPIOOutput - this is the reet for the radio.
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_REST);
     //
    // Enable pin PE1 for GPIOInput
    // Not mapping this to an interupt yet as the IC is not yet written
    MAP_GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_LOW_BATT_INTERUPT);
    // Enable pin PE2 for GPIOOutput
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_ENABLE);
    //
    // Enable pin PF0 for UART1 U1RTS
    // First open the lock and select the bits we want to modify in the GPIO commit register.
    //
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;
    //
    // Now modify the configuration of the pins that we unlocked.
    //
    MAP_GPIOPinConfigure(GPIO_PF0_U1RTS);
    MAP_GPIOPinTypeUART(GPIO_PORTF_BASE, GPIO_PIN_0);
    //
    // Enable pin PF1 for UART1 U1CTS
    //
    MAP_GPIOPinConfigure(GPIO_PF1_U1CTS);
    MAP_GPIOPinTypeUART(GPIO_PORTF_BASE, GPIO_PIN_1);
    //
    // Enable pin PC5 for UART1 U1TX and PC4 for U1RX
    //
    MAP_GPIOPinConfigure(GPIO_PC5_U1TX);
    MAP_GPIOPinConfigure(GPIO_PC4_U1RX);
    //
    // Enable pin PC5 for UART1 U1TX
    //
    MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_5);
    //
    // Enable pin PC4 for UART1 U1RX
    //
    MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4);



    //
    //SPI 1 is used for the FLASH
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
    MAP_SysCtlDelay(1000);
    //
    // Enable pin PD2 for SSI1 SSI1RX
    //
    MAP_GPIOPinConfigure(GPIO_PD2_SSI1RX);
    MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_2);
    //
    // Enable pin PD3 for SSI1 SSI1TX
    //
    MAP_GPIOPinConfigure(GPIO_PD3_SSI1TX);
    MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_3);
    //
    // Enable pin PD0 for SSI1 SSI1CLK
    //
    MAP_GPIOPinConfigure(GPIO_PD0_SSI1CLK);
    MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0);
    //
    // Enable pin PD1 for SSI1 SSI1FSS
    //
    MAP_GPIOPinConfigure(GPIO_PD1_SSI1FSS);
    MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_1);





    //
    // Enable pin PB2 for I2C0 I2C0SCL
    //
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	// majik delay as there are things that need to settle.  Because the I2C is very low speed block this is twice as long as the other delays
	MAP_SysCtlDelay(2000);
	//
	// I2C Clk is on portB pin 2
	//
    MAP_GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    MAP_GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	//
	// I2C SDA is on portB pin 3
	//
    MAP_GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    MAP_GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.  For this example we will use a data rate of 100kbps.
    //

    //
    // Set the clocking to run directly from the external crystal/oscillator.
    // TODO: the crystal got connected to the wrong pins.  This will  need to be changed later in life  - external xtal allows better chance of USB support
    // crystal on your board.


    set_system_speed (INEEDMD_CPU_SPEED_FULL_INTERNAL);


    // USB block need the processor at full speed to complete the initiaisation.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
    //
    // Enable pin PD4 for USB0 USB0DM
    //
    MAP_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4);
    //
    // Enable pin PD5 for USB0 USB0DP
    //
    MAP_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5);


}