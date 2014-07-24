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

#ifndef __BOARD_C__
#define __BOARD_C__
//*****************************************************************************
// includes
//*****************************************************************************
#include "file.h"
#include <stdint.h>
#include <stdbool.h>
#include "utils_inc/proj_debug.h"
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

#include "inc/tm4c1233h6pm.h"
#include "board.h"


//*****************************************************************************
// defines
//*****************************************************************************
//#def EXAMPLE_DEF  value  //def description

//
//a processor loop wait timer.  The number of cycles are calculated from the frequency of the main clock.
//
void wait_time (unsigned int tenths_of_seconds)
{
  MAP_SysCtlDelay(( MAP_SysCtlClockGet() / 30  )*tenths_of_seconds );
}


//
//a 2 BYTE i2C WRITE ROUTINE.  No error checking is implemented
//
void write_2_byte_i2c (unsigned char device_id, unsigned char first_byte, unsigned char second_byte)

{

  I2CMasterSlaveAddrSet(I2C0_BASE, device_id, false);
    I2CMasterDataPut(I2C0_BASE, first_byte);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C0_BASE));
    I2CMasterDataPut(I2C0_BASE, second_byte);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(I2CMasterBusy(I2C0_BASE));

}




void set_system_speed (unsigned int how_fast)
{

  switch (how_fast) {
    case INEEDMD_CPU_SPEED_FULL_EXTERNAL:
      //WARNING - do not use on first board rev!!!!
      //turn on the external oscillator
      MAP_GPIOPinWrite (GPIO_PORTD_BASE, INEEDMD_PORTD_XTAL_ENABLE, INEEDMD_PORTD_XTAL_ENABLE);
      // let it stabalise
      MAP_SysCtlDelay(1000);
      //setting to run on the PLL from the external xtal and switch off the internal oscillator this gives us an 80Mhz clock
      SysCtlClockSet( SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_INT_OSC_DIS);
      //reset up the i2c bus

      break;

    case INEEDMD_CPU_SPEED_FULL_INTERNAL:
      //setting to run on the PLL from the internal clock and switch off the external xtal pads and pin this gives us an 80 Mhz clock
      SysCtlClockSet( SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_MAIN_OSC_DIS);
      // switch off the external oscillator
      MAP_GPIOPinWrite (GPIO_PORTD_BASE, INEEDMD_PORTD_XTAL_ENABLE, 0x00);
      break;

    case INEEDMD_CPU_SPEED_HALF_INTERNAL:
      //setting to run on the  the internal OSC and switch off the external xtal pads and pin.. Setting the divider to run us at 40Mhz
      SysCtlClockSet( SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_MAIN_OSC_DIS);
      // switch off the external oscillator
      MAP_GPIOPinWrite (GPIO_PORTD_BASE, INEEDMD_PORTD_XTAL_ENABLE, 0x00);
      break;

    case INEEDMD_CPU_SPEED_SLOW_INTERNAL:
      //setting to run on the  the internal OSC and switch off the external xtal pads and pin.. Setting the divider to run us at 2Mhz
      SysCtlClockSet( SYSCTL_SYSDIV_8 | SYSCTL_USE_OSC | SYSCTL_OSC_INT4 | SYSCTL_MAIN_OSC_DIS);
      // switch off the external oscillator
      MAP_GPIOPinWrite (GPIO_PORTD_BASE, INEEDMD_PORTD_XTAL_ENABLE, 0x00);
      break;

    case INEEDMD_CPU_SPEED_REALLY_SLOW:
      //setting to run on the  the internal OSC and switch off the external xtal pads and pin.. Setting the divider to run us at 30Khz.
      //Communication is't possible.. we are in hibernation
      SysCtlClockSet( SYSCTL_SYSDIV_1 | SYSCTL_OSC_INT30 | SYSCTL_OSC_INT | SYSCTL_MAIN_OSC_DIS);
      // switch off the external oscillator
      MAP_GPIOPinWrite (GPIO_PORTD_BASE, INEEDMD_PORTD_XTAL_ENABLE, 0x00);
      break;

    default:
      //setting the intrnal at full speed as the default.
      SysCtlClockSet( SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_INT | SYSCTL_MAIN_OSC_DIS);
      // switch off the external oscillator
      MAP_GPIOPinWrite (GPIO_PORTD_BASE, INEEDMD_PORTD_XTAL_ENABLE, 0x00);
      break;
  }



}

void
PowerInitFunction(void)
{
  HWREG(SYSCTL_PBORCTL_R)=SYSCTL_PBORCTL_BOR0;
}

void
GPIOEnable(void)
{
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

}

void
GPIODisable(void)
{
    //
    // Enable Peripheral Clocks
    // These have to be switched on in order... who knew!
    //
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_GPIOA);
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_GPIOB);
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_GPIOC);
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_GPIOD);
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_GPIOE);
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_GPIOF);

}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void
BatMeasureADCEnable(void)
{

    //uint32_t uiData;

    //Enable the ADC Clock
    MAP_SysCtlPeripheralEnable(BATTERY_SYSCTL_PERIPH_ADC);

    MAP_GPIOPinTypeADC(INEEDMD_BATTERY_PORT, INEEDMD_BATTERY_MEASUREMENT_IN_PIN);

//    MAP_SysCtlADCSpeedSet(SYSCTL_ADCSPEED_1MSPS); //(p215 ROM-LM guide)
//    MAP_ADCSequenceDisable(BATTERY_ADC, 3);

//    ROM_ADCSequenceConfigure(BATTERY_ADC, 3, ADC_TRIGGER_PROCESSOR, 0);

    MAP_ADCSequenceDisable(BATTERY_ADC, 3);
    MAP_ADCSequenceConfigure(BATTERY_ADC, 3, ADC_TRIGGER_PROCESSOR, 0);
    MAP_ADCSequenceStepConfigure(BATTERY_ADC, 3, 0, BATTERY_ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END );
    MAP_ADCIntClear(BATTERY_ADC, 3);

    MAP_ADCSequenceEnable(BATTERY_ADC, 3);

    //MAP_ADCSequenceDataGet(BATTERY_ADC, 3, &uiData);


}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void
BatMeasureADCDisable(void)
{
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
   //Enable the ADC Clock
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_ADC0);
    //let is stabalise with a majik delay
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void
BatMeasureADCRead(void)
{

}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
bool
bIs_battery_low(void)
{
  uint8_t uiLow_batt;
  bool bIs_batt_low = false;



  uiLow_batt = GPIOPinRead(GPIO_PORTE_BASE, INEEDMD_RADIO_LOW_BATT_INTERUPT_PIN);

  if(uiLow_batt == INEEDMD_RADIO_LOW_BATT_INTERUPT_PIN)
  {
    bIs_batt_low = true;
  }

  return bIs_batt_low;
}

void
EKGSPIEnable(void)
{
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
  //set the nCS pin as a GPIO
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN);
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
  //MAP_GPIOPinConfigure(GPIO_PA3_SSI0FSS);
  //MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_3);
  //
  // Enable pin PA4 for SSI0 SSI0RX
  //
  MAP_GPIOPinConfigure(GPIO_PA4_SSI0RX);
  MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_4);
  //set SSI for 1MHz clock and 8 bit data, master mode
  SSIConfigSetExpClk(INEEDMD_ADC_SPI, MAP_SysCtlClockGet(), SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, 1000000, 8);
  SSIEnable(INEEDMD_ADC_SPI);
//  while(!SysCtlPeripheralReady(INEEDMD_ADC_SPI));

  //when done set the CS high the ADC needs the CS pin high to work properly
   MAP_GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, INEEDMD_PORTA_ADC_nCS_PIN);
}




void
EKGSPIDisable(void)
{
//  while(!SysCtlPeripheralReady(INEEDMD_ADC_SPI));
  SSIDisable(INEEDMD_ADC_SPI);
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_SSI0);
    //no need for a majik delay as we are configuring the GPIO pins as well...
}


//*****************************************************************************
// name: RadioUARTEnable
// description: configures and enables the usart for the BT radio
// param description: none
// return value description: none
//*****************************************************************************
void
RadioUARTEnable(void)
{
  //TODO: abstract all the direct references to the processor I/O
  //
    //RADIO_CONFIG
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
  //No mjik delay as I am doing the GPIO first.
    //
    // Enable pin PE0 for GPIOOutput - this is the reet for the radio.
    //
    MAP_GPIOPinTypeGPIOOutput(INEEDMD_RADIO_PORT, INEEDMD_RADIO_RESET_PIN);
     //
    // Enable pin PE1 for GPIOInput
    // Not mapping this to an interupt yet as the IC is not yet written
    MAP_GPIOPinTypeGPIOInput(INEEDMD_RADIO_PORT, INEEDMD_RADIO_LOW_BATT_INTERUPT_PIN);
    // Enable pin PE2 for GPIOOutput
    //
    MAP_GPIOPinTypeGPIOOutput(INEEDMD_RADIO_PORT, INEEDMD_RADIO_ENABLE_PIN);

    //de-exert,sets it low, reset pin, aka run mode
    MAP_GPIOPinWrite(INEEDMD_RADIO_PORT, INEEDMD_RADIO_ENABLE_PIN, 0x00);
    //
    // Enable pin PF0 for UART1 U1RTS
    // First open the lock and select the bits we want to modify in the GPIO commit register.
    //
    HWREG(INEEDMD_RADIO_SERIAL_PORT + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(INEEDMD_RADIO_SERIAL_PORT + GPIO_O_CR) = 0x1;
    //
    // Now modify the configuration of the pins that we unlocked.
    //
    MAP_GPIOPinConfigure(GPIO_PF0_U1RTS);
    MAP_GPIOPinTypeUART(INEEDMD_RADIO_SERIAL_PORT, GPIO_PIN_0);
    //
    // Enable pin PF1 for UART1 U1CTS
    //
    MAP_GPIOPinConfigure(GPIO_PF1_U1CTS);
    MAP_GPIOPinTypeUART(INEEDMD_RADIO_SERIAL_PORT, GPIO_PIN_1);
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
  //re set up the UART so it's timings are about correct
    UARTConfigSetExpClk( INEEDMD_RADIO_UART, MAP_SysCtlClockGet(), 115200, ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE ));
    UARTEnable(INEEDMD_RADIO_UART);
//  while(!SysCtlPeripheralReady(INEEDMD_RADIO_UART));

}




//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void
RadioUARTDisable(void)
{
//  while(!SysCtlPeripheralReady(INEEDMD_RADIO_UART));
    UARTDisable(INEEDMD_RADIO_UART);
  MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_UART1);
}

//*****************************************************************************
// name: iRadioPowerOn
// description: sets the gpio pin to the radio high to turn on the radio
// param description: none
// return value description: 1 if success
//*****************************************************************************
int
iRadio_Power_On(void)
{
  GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_RADIO_ENABLE_PIN, INEEDMD_RADIO_ENABLE_PIN);
  return 1;
}

//*****************************************************************************
// name: iRadio_interface_enable
// description: enables the serial interface to the external radio interface
// param description: none
// return value description: 1 if success
//*****************************************************************************
int iRadio_interface_enable(void)
{
  //set flow control
  UARTFlowControlSet(INEEDMD_RADIO_UART, (UART_FLOWCONTROL_TX | UART_FLOWCONTROL_RX));

  // Enables the communication FIFO
  UARTFIFOEnable(INEEDMD_RADIO_UART);

  //perform a delay
  iHW_delay(1);

  //
  // Configure the UART for 115,200, 8-N-1 operation.
  // This function uses SysCtlClockGet() to get the system clock
  // frequency.
  UARTConfigSetExpClk( UART1_BASE, MAP_SysCtlClockGet(), 115200, ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE ));
  //And the Radio UART
  UARTEnable(INEEDMD_RADIO_UART);

  return 1;
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void
SDCardSPIInit(void)
{
    //
    //SPI 1 is used for the FLASH
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
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
    SSIEnable(INEEDMD_FLASH_SPI);
    SSIConfigSetExpClk(INEEDMD_FLASH_SPI, MAP_SysCtlClockGet(), SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, 1000000, 8);
    SSIEnable(INEEDMD_FLASH_SPI);
    //  while(!SysCtlPeripheralReady(INEEDMD_FLASH_SPI));

}

void
SDCardSPIDisable(void)
{
  //  while(!SysCtlPeripheralReady(INEEDMD_FLASH_SPI));
  SSIDisable(INEEDMD_FLASH_SPI);
    //
    //SPI 1 is used for the FLASH
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_SSI1);

}

void
LEDI2CEnable(void)
{
  //
  // Enable pin PB2 for I2C0 I2C0SCL
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
  // majik delay as there are things that need to settle.  Because the I2C is very low speed block this is twice as long as the other delays
  MAP_SysCtlDelay(200);
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
    I2CMasterInitExpClk(INEEDMD_LED_I2C, MAP_SysCtlClockGet(), true);
  I2CMasterEnable(INEEDMD_LED_I2C);
    //  while(!SysCtlPeripheralReady(INEEDMD_LED_I2C));

}

void
LEDI2CDisable(void)
{
    //  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));
    I2CMasterDisable(INEEDMD_LED_I2C);
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C0);

}

void
XTALControlPin(void)
{
    //
    // set up the crystal control pin...
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, INEEDMD_PORTD_XTAL_ENABLE);
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void
USBPortEnable(void)
{
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

    //  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_USB0));

}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void
USBPortDisable(void)
{
    //  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_USB0));
    // USB block need the processor at full speed to complete the initiaisation.
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_USB0);
}

//*****************************************************************************
// name: iHW_delay
// description: performs a blocking hardware based delay. the delay is in 100ms
//  "chunks"
// param description:  uint32_t number of 100ms cycles to delay
// return value description: the number of cycles delayed
//*****************************************************************************
int
iHW_delay(uint32_t uiDelay)
{
  int i;
  for(i = 0; i < uiDelay; i++)
  {
    MAP_SysCtlDelay( MAP_SysCtlClockGet() / 30  );
  }
  return i;
}

#if 0
todo: delete me?
//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void
PortFunctionInit(void)
{

  GPIOEnable();
    BatMeasureADCEnable();
    EKGSPIEnable();
    RadioUARTEnable();
    LEDI2CEnable();
    XTALControlPin();
    USBPortEnable();



    //
    // Set the clocking to run directly from the external crystal/oscillator.
    // TODO: the crystal got connected to the wrong pins.  This will  need to be changed later in life  - external xtal allows better chance of USB support
    // crystal on your board.

    set_system_speed (INEEDMD_CPU_SPEED_FULL_INTERNAL);
}
#endif
//*****************************************************************************
// name: iBoard_init
// description: calls the low level board driver initalization functions
// param description: none
// return value description: 1 if success
//*****************************************************************************
int
iBoard_init(void)
{

  //Set up a colock to 40Mhz off the PLL.  This is fat enough to allow for things to set up well, but not too fast that we have big temporal problems.
  SysCtlClockSet( SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_MAIN_OSC_DIS);

  // set the brown out interupt and power down voltage
  PowerInitFunction();

  // switch on the GPIO
  GPIOEnable();

  //start the SPI bus to the capture ADC
  EKGSPIEnable();

  //start the radio UART
  RadioUARTEnable();

  //and the ASC enable to measure the battery
  BatMeasureADCEnable();

  //start the LED driver so that we can flash and dance
  LEDI2CEnable();

  //start the pin that allows us to shut down the external oscillator... savng some power
  XTALControlPin();

  //setup the USB port.  This can't be used in this clock mode.
  USBPortEnable();

  return 1;
}
#endif //__BOARD_C__
