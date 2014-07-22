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

#ifndef __BOARD_H__
#define __BOARD_H__




#define INEEDMD_PORTA_ADC_PWRDN_OUT_PIN 0x80
#define INEEDMD_PORTA_ADC_RESET_OUT_PIN 0X40
#define INEEDMD_PORTA_ADC_INTERUPT_PIN 0x01
#define INEEDMD_PORTA_ADC_START_PIN 0x02
#define INEEDMD_PORTA_ADC_nCS_PIN 0x08

#define INEEDMD_PORTE_BATTERY_MEASUREMENT_IN_PIN 0x08
#define INEEDMD_PORTE_RADIO_REST  0x01
#define INEEDMD_PORTE_RADIO_LOW_BATT_INTERUPT 0x02
#define INEEDMD_PORTE_RADIO_ENABLE 0x04

#define INEEDMD_PORTD_XTAL_ENABLE 0x20


//port mappings to make easier to read names...
#define INEEDMD_ADC_SPI SSI0_BASE
#define INEEDMD_FLASH_SPI SSI1_BASE
#define INEEDMD_LED_I2C I2C0_BASE
#define INEEDMD_RADIO_UART UART1_BASE
#define INEEDMD_USB USB0_BASE

//subsystem block number - just used to make some coding functions easer.
#define INEEDMD_CPU 0x00
#define INEEDMD_ADC 0x01
#define INEEDMD_FLASH 0x02
#define INEEDMD_LED 0x03
#define INEEDMD_RADIO 0x04
#define INEED_USB 0x05

//subsystem block number - just used to make some coding functions easer.
#define INEEDMD_CPU_SPEED_FULL_EXTERNAL 0x00
#define INEEDMD_CPU_SPEED_FULL_INTERNAL 0x01
#define INEEDMD_CPU_SPEED_SLOW_INTERNAL 0x02
#define INEEDMD_CPU_SPEED_REALLY_SLOW 0x03



void Set_Timer0_Sleep();
void PowerInitFunction(void);
void PortFunctionInit(void);
void ConfigureSleep(void);
void wait_time (unsigned int);
void write_2_byte_i2c (unsigned char, unsigned char, unsigned char);
void set_system_speed (unsigned int);

void GPIOEnable(void);
void BatMeasureADCEnable(void);
void EKGSPIEnable(void);
void RadioUARTEnable(void);
void RadioUARTDisable(void);
int iRadio_Power_On(void);
int iRadio_interface_enable(void);
void LEDI2CEnable(void);
void XTALControlPin(void);
void USBPortEnable(void);

void GPIODisable(void);
void BatMeasureADCDisable(void);
bool bIs_battery_low(void);
void EKGSPIDisable(void);

void LEDI2CDisable(void);
void USBPortDisnable(void);
int iBoard_init(void);

#endif //  __BOARD_H__