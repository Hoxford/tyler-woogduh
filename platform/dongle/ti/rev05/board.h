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

/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/SPI.h>
/******************************************************************************
*public defines ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//RTOS init function mappings
#define Board_initDMA               EK_TM4C123GXL_initDMA
#define Board_initEEPROM            EK_TM4C123GXL_initEEPROM
#define Board_initGeneral           EK_TM4C123GXL_initGeneral
#define Board_initGPIO              EK_TM4C123GXL_initGPIO
#define Board_initI2C               EK_TM4C123GXL_initI2C
#define Board_initPWM               EK_TM4C123GXL_initPWM
#define Board_initSDSPI             EK_TM4C123GXL_initSDSPI
#define Board_initSPI               EK_TM4C123GXL_initSPI
#define Board_initUART              EK_TM4C123GXL_initUART
#define Board_initUSB               EK_TM4C123GXL_initUSB
#define Board_initADC               EK_TM4C123GXL_initADC
#define Board_initWatchdog          EK_TM4C123GXL_initWatchdog

//end RTOS init funciton mappings

#define Board_I2C0                  EK_TM4C123GXL_I2C0
#define Board_I2C1                  EK_TM4C123GXL_I2C3

#define Board_SDSPI0                EK_TM4C123GXL_SDSPI0

#define Board_SPI0                  EK_TM4C123GXL_SPI0
#define Board_SPI1                  EK_TM4C123GXL_SPI3
#define Board_SPI_CC3000            EK_TM4C123GXL_SPI2
#define Board_SPI_CC3100            EK_TM4C123GXL_SPI2

#define Board_USBDEVICE             EK_TM4C123GXL_USBDEVICE

#define Board_UART0                 EK_TM4C123GXL_UART0

#define Board_WATCHDOG0             EK_TM4C123GXL_WATCHDOG0
#define Board_WATCHDOG1             EK_TM4C123GXL_WATCHDOG1

#define Board_gpioCallbacks0        EK_TM4C123GXL_gpioPortBCallbacks
#define Board_gpioCallbacks1        EK_TM4C123GXL_gpioPortACallbacks
#define Board_gpioCallbacks2        EK_TM4C123GXL_gpioPortECallbacks
#define Board_gpioCallbacks3        EK_TM4C123GXL_gpioPortFCallbacks

/* Board specific I2C addresses */



//ADC A to D Converter port mappings///////////////////////

//ADC gpio mappings
  #define INEEDMD_ADC_GPIO_PORT         GPIO_PORTA_BASE

//ADC power pin defines
  #define INEEDMD_ADC_PWR_PIN           GPIO_PIN_7
  #define INEEDMD_ADC_PWR_PIN_SET       GPIO_PIN_7
  #define INEEDMD_ADC_PWR_PIN_CLEAR     0

//ADC reset pin defines
  #define INEEDMD_ADC_RESET_PIN         GPIO_PIN_6
  #define INEEDMD_ADC_RESET_PIN_SET     GPIO_PIN_6
  #define INEEDMD_ADC_RESET_PIN_CLEAR   0

//ADC data ready pin defines
  #define INEEDMD_ADC_INTERUPT_PIN      GPIO_PIN_0 //0x01

//ADC start pin defines
  #define INEEDMD_ADC_START_PIN         GPIO_PIN_1
  #define INEEDMD_ADC_START_PIN_SET     GPIO_PIN_1
  #define INEEDMD_ADC_START_PIN_CLEAR   0

//ADC chip select defines
  #define INEEDMD_ADC_nCS_PIN           GPIO_PIN_3
  #define INEEDMD_ADC_nCS_PIN_SET       GPIO_PIN_3
  #define INEEDMD_ADC_nCS_PIN_CLEAR     0

//ADC sys control mappings
  #define INEEDMD_ADC_SYSCTL_PRIPH_SSI  SYSCTL_PERIPH_SSI0
  #define INEEDMD_ADC_GPIO_SSICLK       GPIO_PA2_SSI0CLK
  #define INEEDMD_ADC_GPIO_SSITX        GPIO_PA5_SSI0TX
  #define INEEDMD_ADC_GPIO_SSIRX        GPIO_PA4_SSI0RX
  #define INEEDMD_ADC_SSICLK_PIN        GPIO_PIN_2
  #define INEEDMD_ADC_SSITX_PIN         GPIO_PIN_5
  #define INEEDMD_ADC_SSIRX_PIN         GPIO_PIN_4

//ADC spi interface mappings
  #define INEEDMD_ADC_SPI               SSI0_BASE
  #define INEEDMD_FLASH_SPI             SSI1_BASE
  #define INEEDMD_SPI_CLK               16000000

//ADC SPI system command defines
  #define ADS1198_WAKEUP    0x02    //Wake-up from standby mode. NOP command in normal mode.
  #define ADS1198_NOP       0x02
  #define ADS1198_STANDBY   0x04    //Enter standby mode
  #define ADS1198_RESET     0x06    //Reset the device
  #define ADS1198_START     0x08    //Start/restart (synchronize) conversions
  #define ADS1198_STOP      0x0A    //Stop conversion
//ADC SPI read data command defines
  #define ADS1198_RDATAC    0x10    //continuous conversion mode
  #define ADS1198_SDATAC    0x11    //stop continuous conversion
  #define ADS1198_RDATA     0x12    //read data by command

//ADC register read/write commands
//  byte 1                                      byte 2
//  RREG | [starting address]                   [number of registers to read]
//***************************************************************
  #define RREG              0x20    //Read register
  #define WREG              0x40

//ADC REGISTER MAP
//***************************************************************
#define ADS1198_ID_ADDRESS 0x00
#define ADS1198_IDENTIFIER	0xB6
//device ID (read only)
//#undef  ADS1198_ID
//#define ADS1198_ID                    0x00            //different name than datasheet

//ADC global settings
  #define CONFIG1           0x01
  #define CONFIG2           0x02
    #define TEST_REGISTER CONFIG2
  #define CONFIG3           0x03
  #define LOFF              0x04

// ADC channel specific settings
  #define CH1SET            0x05
  #define CH2SET            0x06
  #define CH3SET            0x07
  #define CH4SET            0x08
  #define CH5SET            0x09
  #define CH6SET            0x0A
  #define CH7SET            0x0B
  #define CH8SET            0x0C
  #define RLD_SENSP         0x0D
  #define RLD_SENSN         0x0E
  #define LOFF_SENSP        0x0F
  #define LOFF_SENSN        0x10
  #define LOFF_FLIP         0x11

// ADC lead off status, read only
  #define LOFF_STATP        0x12
  #define LOFF_STATN        0x13

// ADC GPIO and OTHER registers
  #define ADS1198_GPIO      0x14            //different name than datasheet
  #define ADS1198_PACE      0x15            //different name than datasheet
  #define CONFIG4           0x17
  #define WCT1              0x18
  #define WCT2              0x19

// ADC Register specific bit masks
//***************************************************************

// ADC CONFIG1
  #define DAISY_EN          0x40
  #define CLK_EN            0x20
  #define DR2               0x04
  #define DR1               0x02
  #define DR0               0x01

// ADC CONFIG2
  #define INT_TEST          0x10
  #define TEST_AMP          0x04
  #define TEST_FREQ1        0x02
  #define TEST_FREQ0        0x01

// ADC CONFIG3
  #define PD_REFBUF         0x80
  #define VREF_4V           0x20
  #define RLD_MEAS          0x10
  #define RLDREF_INT        0x08
  #define PD_RLD            0x04
  #define RLD_LOFF_SENS     0x02
  #define RLD_STAT          0x01

// ADC LOFF
  #define COMP_TH2          0x80
  #define COMP_TH1          0x40
  #define COMP_TH0          0x20
  #define VLEAD_OFF_EN      0x10
  #define ILEAD_OFF1        0x08
  #define ILEAD_OFF0        0x04
  #define FLEAD_OFF1        0x02
  #define FLEAD_OFF0        0x01

// ADC CHnSET
  #define PD1                                     0x80
  #define GAIN2             0x40
  #define GAIN1             0x20
  #define GAIN0             0x10
  #define MUX2              0x04
  #define MUX1              0x02
  #define MUX0              0x01

//ADC gain settings
  #define GAIN_1_X          GAIN0
  #define GAIN_2_X          GAIN1
  #define GAIN_3_X         (GAIN0 | GAIN1)
  #define GAIN_4_X          GAIN2
  #define GAIN_6_X          0x00
  #define GAIN_8_X         (GAIN2 | GAIN0)
  #define GAIN_12_X        (GAIN2 | GAIN1)

//ADC mux settings
  #define MUX_TEST         (MUX0 | MUX2)
  #define MUX_ELECTRODE     0x00
  #define MUX_SHORT        (MUX0)
  #define MUX_TEMPERATURE  (MUX0 | MUX2)
  #define MUX_ELECTRODES    0x00

// ADC RLD_SENSP
  #define RLD8P             0x80
  #define RLD7P             0x40
  #define RLD6P             0x20
  #define RLD5P             0x10
  #define RLD4P             0x08
  #define RLD3P             0x04
  #define RLD2P             0x02
  #define RLD1P             0x01
  
/* 0.3 Board Pin Definition 
  #define EKG_V6 RLD8P
  #define EKG_V5 RLD7P
  #define EKG_V4 RLD6P
  #define EKG_V3 RLD5P
  #define EKG_LA RLD4P
  #define EKG_LL RLD3P
  #define EKG_V2 RLD2P
  #define EKG_V1 RLD1P
*/

// 0.5 Board Pin Definition
  #define EKG_V1 RLD8P
  #define EKG_V2 RLD7P
  #define EKG_V3 RLD6P
  #define EKG_V4 RLD5P
  #define EKG_LL RLD4P
  #define EKG_LA RLD3P
  #define EKG_V5 RLD2P
  #define EKG_V6 RLD1P

// ADC RLD_SENSN
  #define RLD_NO_LEAD       0x00
  #define RLD8N             0x80
  #define RLD7N             0x40
  #define RLD6N             0x20
  #define RLD5N             0x10
  #define RLD4N             0x08
  #define RLD3N             0x04
  #define RLD2N             0x02
  #define RLD1N             0x01

/* 0.3 Board Pin Definition 
  #define EKG_WTC1          RLD8N
  #define EKG_WTC2          RLD7N
  #define EKG_WTC3          RLD6N
  #define EKG_WTC4          RLD5N
  #define EKG_RA1           RLD4N
  #define EKG_RA2           RLD3N
  #define EKG_WTC5          RLD2N
  #define EKG_WTC6          RLD1N
  
  */
// 0.5 Board Pin Definition
  #define EKG_WTC1 RLD8N
  #define EKG_WTC2 RLD7N
  #define EKG_WTC3 RLD6N
  #define EKG_WTC4 RLD5N
  #define EKG_RA1 RLD4N
  #define EKG_RA2 RLD3N
  #define EKG_WTC5 RLD2N
  #define EKG_WTC6 RLD1N

// ADC LOFF_SENSP
  #define LOFF8P            0x80
  #define LOFF7P            0x40
  #define LOFF6P            0x20
  #define LOFF5P            0x10
  #define LOFF4P            0x08
  #define LOFF3P            0x04
  #define LOFF2P            0x02
  #define LOFF1P            0x01

// ADC LOFF_SENSN
  #define LOFF8N            0x80
  #define LOFF7N            0x40
  #define LOFF6N            0x20
  #define LOFF5N            0x10
  #define LOFF4N            0x08
  #define LOFF3N            0x04
  #define LOFF2N            0x02
  #define LOFF1N            0x01

// ADC ADS1198_GPIO
  #define ADS1198_GPIOD4    0x80
  #define ADS1198_GPIOD3    0x40
  #define ADS1198_GPIOD2    0x20
  #define ADS1198_GPIOD1    0x10
  #define ADS1198_GPIOC4    0x08
  #define ADS1198_GPIOC3    0x04
  #define ADS1198_GPIOC2    0x02
  #define ADS1198_GPIOC1    0x01

// ADC PACE
  #define PACEE1            0x10
  #define PACEE0            0x08
  #define PACEO1            0x04
  #define PACEO0            0x02
  #define PD_PACE           0x01

// ADC CONFIG4
  #define SINGLE_SHOT       0x08
  #define WCT_TO_RLD        0x04
  #define PD_LOFF_COMP      0x02

// ADC WCT1
  #define aVF_CH6           0x80
  #define aVL_CH5           0x40
  #define aVR_CH7           0x20
  #define avR_CH4           0x10
  #define PD_WCTA           0x08
  #define WCTA2             0x04
  #define WCTA1             0x02
  #define WCTA0             0x01

// ADC WCT2
  #define PD_WCTC           0x80
  #define PD_WCTB           0x40
  #define WCTB2             0x20
  #define WCTB1             0x10
  #define WCTB0             0x08
  #define WCTC2             0x04
  #define WCTC1             0x02
  #define WCTC0             0x01

//ADC power defines
  #define POWER_UP_WTCA     PD_WCTA
  #define POWER_UP_WTCB     PD_WCTB
  #define POWER_UP_WTCC     PD_WCTC
  #define POWER_DOWN_WTC    0x00

//ADC GPIO   Used of powering the external reference.
#define GPIO_ADDRESS       0x14
#define ADC_GPIO4          0x80
#define ADC_GPIO3          0x40
#define ADC_GPIO2          0x20
#define ADC_GPIO1          0x10
#define ADC_GPIO4_INPUT    0x08
#define ADC_GPIO3_INPUT    0x04
#define ADC_GPIO2_INPUT    0x03
#define ADC_GPIO1_INPUT    0x01

#define EXTERNAL_REF_POWER_ADDRESS  GPIO_ADDRESS
#define EXTERNAL_REF_OFF ( ADC_GPIO4_INPUT | ADC_GPIO3_INPUT | ADC_GPIO2_INPUT | ADC_GPIO1_INPUT )
#define EXTERNAL_REF_ON  ( ADC_GPIO4 | ADC_GPIO3 | ADC_GPIO2 |  ADC_GPIO1_INPUT  )


// ADC END A to D Converter port mappings//////////////////

//EKG Lead reading mappings////////////////////////////////
//
  #define LEAD_SHORT                     0x82FF
  #define LEAD_SHORT_RESET               0xA0FF
  #define LEAD_SHORT_SLEEP               0xA2FF  //5 & 6 short
  #define LEAD_ALL_SHORTED               0x00FF

//battery port mappings
//
  #define INEEDMD_BATTERY_PORT                 GPIO_PORTE_BASE
  #define INEEDMD_BATTERY_MEASUREMENT_IN_PIN   GPIO_PIN_0
  #define INEEDMD_PORTE_RADIO_LOW_BATT_INTERUPT 0x02
  #define BATTERY_ADC                          ADC0_BASE
  #define BATTERY_ADC_CTL_CH0                  ADC_CTL_CH0
  #define BATTERY_SYSCTL_PERIPH_ADC            SYSCTL_PERIPH_ADC0
  #define BATTERY_CRITICAL_ADC_VALUE           0x00000800
  #define BATTERY_LOW_ADC_VALUE                0x00000900

//temperature monitor mapping
  #define TEMPERATURE_ADC                      ADC1_BASE
  #define TEMPERATURE_ADC_CTL                  ADC_CTL_TS
  #define TEMPERATURE_SYSCTL_PERIPH_ADC        SYSCTL_PERIPH_ADC1

//#define BATTERY_SYSCTL_PERIPH_GPIO           SYSCTL_PERIPH_GPIOE

//BT Radio mappings
//
//#define INEEDMD_RADIO_UART                     UART1_BASE
  #define INEEDMD_RADIO_UART_INDEX               EK_TM4C123GXL_UART1
  #define INEEDMD_RADIO_UART                     UART1_BASE
  #define INEEDMD_RADIO_SYSCTL_PERIPH_UART       SYSCTL_PERIPH_UART1
  #define INEEDMD_RADIO_UART_CLK                 16000000
//Radio UART baud rates
  #define INEEDMD_RADIO_UART_BAUD_57600    57600
    #define INEEDMD_RADIO_UART_BAUD_57600d10    51840
    #define INEEDMD_RADIO_UART_BAUD_57600u10    63360
  #define INEEDMD_RADIO_UART_BAUD_76800    76800
    #define INEEDMD_RADIO_UART_BAUD_76800d10    69120
    #define INEEDMD_RADIO_UART_BAUD_76800u10    84480
  #define INEEDMD_RADIO_UART_BAUD_115200   115200
    #define INEEDMD_RADIO_UART_BAUD_115200d10   103680
    #define INEEDMD_RADIO_UART_BAUD_115200u10   126720
  #define INEEDMD_RADIO_UART_BAUD_230400   230400
    #define INEEDMD_RADIO_UART_BAUD_230400d10   207360
    #define INEEDMD_RADIO_UART_BAUD_230400u10   253440
  #define INEEDMD_RADIO_UART_BAUD_460800   460800
    #define INEEDMD_RADIO_UART_BAUD_460800d10   414720
    #define INEEDMD_RADIO_UART_BAUD_460800u10   506880
  #define INEEDMD_RADIO_UART_BAUD_921600   921600
    #define INEEDMD_RADIO_UART_BAUD_921600d10   829440
    #define INEEDMD_RADIO_UART_BAUD_921600u10   1013760
  #define INEEDMD_RADIO_UART_BAUD_1382400  1382400
    #define INEEDMD_RADIO_UART_BAUD_1382400d10  1244160
    #define INEEDMD_RADIO_UART_BAUD_1382400u10  1520640
  #define INEEDMD_RADIO_UART_BAUD_1843200  1843200
    #define INEEDMD_RADIO_UART_BAUD_1843200d10  1658880
    #define INEEDMD_RADIO_UART_BAUD_1843200u10  2027520
  #define INEEDMD_RADIO_UART_BAUD_2764800  2764800
    #define INEEDMD_RADIO_UART_BAUD_2764800d10  2488320
    #define INEEDMD_RADIO_UART_BAUD_2764800u10  3041280
  #define INEEDMD_RADIO_UART_BAUD_3686400  3686400
    #define INEEDMD_RADIO_UART_BAUD_3686400d10  3317760
    #define INEEDMD_RADIO_UART_BAUD_3686400u10  4055040
      #define INEEDMD_RADIO_UART_BAUD          INEEDMD_RADIO_UART_BAUD_1382400
      #define INEEDMD_RADIO_UART_DEFAULT_BAUD  INEEDMD_RADIO_UART_BAUD_115200
//RX Mappings
  #define INEEDMD_GPIO_RX_PORT                   GPIO_PORTC_BASE
  #define INEEDMD_GPIO_RX_PIN                    GPIO_PIN_4
  #define INEEDMD_GPIO_UARTRX                    GPIO_PC4_U1RX
  #define UDMA_CHANNEL_RADIO_RX                  UDMA_CHANNEL_UART1RX
//TX mappings
  #define INEEDMD_GPIO_TX_PORT                   GPIO_PORTC_BASE
  #define INEEDMD_GPIO_TX_PIN                    GPIO_PIN_5
  #define INEEDMD_GPIO_UARTTX                    GPIO_PC5_U1TX
  #define UDMA_CHANNEL_RADIO_TX                  UDMA_CHANNEL_UART1TX
//RTS mappings
  #define INEEDMD_RADIO_RTS_PORT                 GPIO_PORTF_BASE
  #define INEEDMD_RADIO_RTS_PIN                  GPIO_PIN_0
  #define INEEDMD_GPIO_UARTRTS                   GPIO_PF0_U1RTS
  #define INEEDMD_GPIO_RTS_LOCK                  GPIO_O_LOCK
  #define INEEDMD_GPIO_RTS_CR                    GPIO_O_CR
//CTS mappings
  #define INEEDMD_RADIO_CTS_PORT                 GPIO_PORTF_BASE
  #define INEEDMD_RADIO_CTS_PIN                  GPIO_PIN_1
  #define INEEDMD_GPIO_UARTCTS                   GPIO_PF1_U1CTS
//Radio reset mappings
  #define INEEDMD_GPIO_RST_PORT                  GPIO_PORTE_BASE
  #define INEEDMD_RADIO_RESET_PIN                GPIO_PIN_1
  #define INEEDMD_RADIO_RESET_PIN_SET            GPIO_PIN_1
  #define INEEDMD_RADIO_RESET_PIN_CLEAR          0
//Radio CD mappings
  #define INEEDMD_GPIO_CD_PORT                   GPIO_PORTE_BASE
  #define INEEDMD_RADIO_CD_PIN                   GPIO_PIN_3
  #define INEEDMD_RADIO_CD_PIN_SET               GPIO_PIN_3
  #define INEEDMD_RADIO_CD_PIN_CLEAR             0
  //Radio DTR mappings
  #define INEEDMD_GPIO_DTR_PORT                  GPIO_PORTB_BASE
  #define INEEDMD_RADIO_DTR_PIN                  GPIO_PIN_7
  #define INEEDMD_RADIO_DTR_PIN_SET              GPIO_PIN_7
  #define INEEDMD_RADIO_DTR_PIN_CLEAR            0
//Radio command mode mappings
  #define INEEDMD_GPIO_CMND_PORT                 INEEDMD_GPIO_DTR_PORT
  #define INEEDMD_RADIO_CMND_PIN                 INEEDMD_RADIO_DTR_PIN
  #define INEEDMD_RADIO_CMND_PIN_SET             INEEDMD_RADIO_DTR_PIN_SET
  #define INEEDMD_RADIO_CMND_PIN_CLEAR           INEEDMD_RADIO_DTR_PIN_CLEAR
//Radio enable mappings
  #define INEEDMD_GPIO_EN_PORT                   GPIO_PORTB_BASE
  #define INEEDMD_RADIO_ENABLE_PIN               GPIO_PIN_6
  #define INEEDMD_RADIO_ENABLE_PIN_SET           GPIO_PIN_6
  #define INEEDMD_RADIO_ENABLE_PIN_CLEAR         0
//Radio low bat mappings
  #define INEEDMD_GPIO_LOW_BAT_PORT              GPIO_PORTF_BASE
  #define INEEDMD_RADIO_LOW_BAT_PIN              GPIO_PIN_4
  #define INEEDMD_RADIO_LOW_BAT_PIN_SET          GPIO_PIN_4
  #define INEEDMD_RADIO_LOW_BAT_PIN_CLEAR        0
  #define INEEDMD_RADIO_LOW_BATT_INTERUPT_PIN    GPIO_PIN_4  //todo no longer will be on pin 1
//Radio RDY mappings
  #define INEEDMD_GPIO_RDY_PORT                  GPIO_PORTE_BASE
  #define INEEDMD_RADIO_RDY_PIN                  GPIO_PIN_3
  #define INEEDMD_RADIO_RDY_PIN_SET              GPIO_PIN_3
  #define INEEDMD_RADIO_RDY_PIN_CLEAR            0

//Radio interrupt mappings
  #define INEEDMD_RADIO_UART_INT                 INT_UART1

//External clock mappings /////////////
//
  #define INEEDMD_XTAL_PORT              GPIO_PORTD_BASE
  #define INEEDMD_XTAL_ENABLE_PIN        GPIO_PIN_6
  #define INEEDMD_XTAL_ENABLE_PIN_SET    GPIO_PIN_6
  #define INEEDMD_XTAL_ENABLE_PIN_CLEAR  0

//I2C LED controller mappings /////////
//
#define INEEDMD_LED_I2C_BASE          I2C2_BASE
#define INEEDMD_LED_I2C_INT           INT_I2C2
#define INEEDMD_LED_SYSCTL_PRIPH_I2C  SYSCTL_PERIPH_I2C2
#define INEEDMD_LED_SYSCTL_PRIPH_GPIO SYSCTL_PERIPH_GPIOE
#define INEEDMD_LED_GPIO_PORT         GPIO_PORTE_BASE
#define INEEDMD_LED_I2CSCL_PIN        GPIO_PIN_4
#define INEEDMD_LED_I2CSDA_PIN        GPIO_PIN_5
#define INEEDMD_LED_GPIO_I2CSCL       GPIO_PE4_I2C2SCL
#define INEEDMD_LED_GPIO_I2CSDA       GPIO_PE5_I2C2SDA

//LED ENABLE mappings
#define INEEDMD_GPIO_LED_EN_PORT             GPIO_PORTB_BASE
#define INEEDMD_LED_ENABLE_PIN               GPIO_PIN_5
#define INEEDMD_LED_ENABLE_PIN_SET           GPIO_PIN_5
#define INEEDMD_LED_ENABLE_PIN_CLEAR         0

//PSU keep alive mappings
#define INEEDMD_GPIO_PSU_EN_PORT             GPIO_PORTB_BASE
#define INEEDMD_PSU_ENABLE_PIN               GPIO_PIN_2
#define INEEDMD_PSU_ENABLE_PIN_SET           GPIO_PIN_2
#define INEEDMD_PSU_ENABLE_PIN_CLEAR         0

//PWM PIN mappings
#define INEEDMD_GPIO_PWM_PORT               GPIO_PORTB_BASE
#define INEEDMD_PWM_PIN                     GPIO_PIN_1
#define INEEDMD_PWM_PIN_SET                 GPIO_PIN_1
#define INEEDMD_PWM_PIN_CLEAR               0
#define INEEDMD_PWM_BLOCK                   SYSCTL_PERIPH_PWM1
#define INEEDMD_PB1_MUX_TO_PWM              GPIO_PB1_T2CCP1
#define INEEDMD_PWM_BASE                    PWM1_BASE

  
//USB mappings
//
  #define INEEDMD_USB                USB0_BASE
  #define INEEDMD_USB_SYSCTL_PERIPH  SYSCTL_PERIPH_USB0
//USB interface pin mappings
  #define INEEDMD_USB_GPIO_PORT  GPIO_PORTD_BASE
  #define INEEDMD_USBDP_PIN      GPIO_PIN_5
  #define INEEDMD_USBDM_PIN      GPIO_PIN_4
//USB detect pin mappings
  #define INEEDMD_USB_DET_GPIO_PORT  GPIO_PORTB_BASE
  #define INEEDMD_USB_DET_PIN        GPIO_PIN_4
  #define INEEDMD_USB_DET_PIN_SET    GPIO_PIN_4
  #define INEEDMD_USB_DET_PIN_CLEAR  0

//SD card mappings ////////////////////
//
  #define INEEDMD_SD_SSI_BASE           SSI3_BASE
  #define INEEDMD_SD_SPI_SYSCTL_PERIPH  SYSCTL_PERIPH_SSI3
//SD IO pin mappings
  #define INEEDMD_SD_GPIO_PORT   GPIO_PORTD_BASE
  #define INEEDMD_SD_SCK_PIN     GPIO_PIN_0
  #define INEEDMD_SD_MISO_PIN    GPIO_PIN_2//GPIO_PIN_3
  #define INEEDMD_SD_MOSI_PIN    GPIO_PIN_3//GPIO_PIN_2
  #define INEEDMD_SD_SCK_SSI     GPIO_PD0_SSI3CLK
  #define INEEDMD_SD_MISO_SSI    GPIO_PD2_SSI3RX
  #define INEEDMD_SD_MOSI_SSI    GPIO_PD3_SSI3TX
//SD chip select mappings
  #define INEEDMD_SD_CS_GPIO_PORT  GPIO_PORTD_BASE
  #define INEEDMD_SD_CS_PIN        GPIO_PIN_1
//SD TX pin
  #define INEEDMD_SD_TX_PORT     GPIO_PORTD_BASE
  #define INEEDMD_SD_TX_PIN      GPIO_PIN_2

//Default CPU speed
#define INEEDMD_CPU_SPEED_DEFAULT       INEEDMD_CPU_SPEED_HALF_INTERNAL
//#define INEEDMD_CPU_SPEED_NOT_SET           0x00
//#define INEEDMD_CPU_SPEED_FULL_EXTERNAL     0x07
//#define INEEDMD_CPU_SPEED_HALF_EXTERNAL     0x05
//#define INEEDMD_CPU_SPEED_QUARTER_EXTERNAL  0x06
//#define INEEDMD_CPU_SPEED_FULL_INTERNAL     0x01
//#define INEEDMD_CPU_SPEED_HALF_INTERNAL     0x02
//#define INEEDMD_CPU_SPEED_HALF_INTERNAL_OSC 0x08
//#define INEEDMD_CPU_SPEED_SLOW_INTERNAL     0x03
//#define INEEDMD_CPU_SPEED_REALLY_SLOW       0x04

//define the WTC channel A input for the RA electronde as the channel 3 negative input
#define WTC_A_CHANNEL 0x05
//define the WTC channel B input for the LA electronde as the channel 3 Positive input
#define WTC_B_CHANNEL 0x20
//define the WTC channel C input for the LL electronde as the channel 4 Positive input
#define WTC_C_CHANNEL 0x06


/******************************************************************************
*public variables /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
/* GPIO_Callbacks structure for GPIO interrupts */
extern const GPIO_Callbacks EK_TM4C123GXL_gpioPortACallbacks;
extern const GPIO_Callbacks EK_TM4C123GXL_gpioPortBCallbacks;
extern const GPIO_Callbacks EK_TM4C123GXL_gpioPortECallbacks;
extern const GPIO_Callbacks EK_TM4C123GXL_gpioPortFCallbacks;

/******************************************************************************
*public enums /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
/*!
 *  @def    EK_TM4C123GXL_GPIOName
 *  @brief  Enum of GPIO names on the EK_TM4C123GXL dev board
 */
typedef enum EK_TM4C123GXL_GPIOName
{
  EK_TM4C123GXL_DEBUG = 0,
  EK_TM4C123GXL_RADIO_POWER,
  EK_TM4C123GXL_RADIO_CMND_MODE,
//  EK_TM4C123GXL_RADIO_DTR,
  EK_TM4C123GXL_RADIO_LOW_BATT,
  EK_TM4C123GXL_RADIO_CD,
  EK_TM4C123GXL_RADIO_RDY,
  EK_TM4C123GXL_RADIO_RESET,
  EK_TM4C123GXL_XTAL_ENABLE,
  EK_TM4C123GXL_ADC_POWER,
  EK_TM4C123GXL_ADC_RESET,
  EK_TM4C123GXL_ADC_INTERUPT,
  EK_TM4C123GXL_ADC_START,
  EK_TM4C123GXL_ADC_nCS,
  EK_TM4C123GXL_USB_DETECT,
  EK_TM4C123GXL_LED_ENABLE,
  EK_TM4C123GXL_PSU_ENABLE,
  EK_TM4C123GXL_PWM_OUTPUT,
  EK_TM4C123GXL_VBAT_INPUT,
  EK_TM4C123GXL_GPIOCOUNT,
} EK_TM4C123GXL_GPIOName;

/*!
 *  @def    EK_TM4C123GXL_I2CName
 *  @brief  Enum of I2C names on the EK_TM4C123GXL dev board
 */
typedef enum EK_TM4C123GXL_I2CName {
    EK_TM4C123GXL_I2C_LED = 0,

    EK_TM4C123GXL_I2CCOUNT
} EK_TM4C123GXL_I2CName;

/*!
 *  @def    EK_TM4C123GXL_SDSPIName
 *  @brief  Enum of SDSPI names on the EK_TM4C123GXL dev board
 */
typedef enum EK_TM4C123GXL_SDSPIName {
    EK_TM4C123GXL_SDSPI0 = 0,

    EK_TM4C123GXL_SDSPICOUNT
} EK_TM4C123GXL_SDSPIName;

/*!
 *  @def    EK_TM4C123GXL_SPIName
 *  @brief  Enum of SPI names on the EK_TM4C123GXL dev board
 */
typedef enum EK_TM4C123GXL_SPIName {
  EK_TM4C123GXL_SPI_ADC = 0,
//    EK_TM4C123GXL_SPI0 = 0,
//    EK_TM4C123GXL_SPI2,
//    EK_TM4C123GXL_SPI3,

    EK_TM4C123GXL_SPICOUNT
} EK_TM4C123GXL_SPIName;

/*!
 *  @def    EK_TM4C123GXL_UARTName
 *  @brief  Enum of UARTs on the EK_TM4C123GXL dev board
 */
typedef enum EK_TM4C123GXL_UARTName {
    EK_TM4C123GXL_UART1 = 0,
    EK_TM4C123GXL_UART3,
    EK_TM4C123GXL_UARTCOUNT
} EK_TM4C123GXL_UARTName;

/*!
 *  @def    EK_TM4C123GXL_USBMode
 *  @brief  Enum of USB setup function on the EK_TM4C123GXL dev board
 */
typedef enum EK_TM4C123GXL_USBMode {
    EK_TM4C123GXL_USBDEVICE,
    EK_TM4C123GXL_USBHOST
} EK_TM4C123GXL_USBMode;

/*!
 *  @def    EK_TM4C123GXL_WatchdogName
 *  @brief  Enum of Watchdogs on the EK_TM4C123GXL dev board
 */
typedef enum EK_TM4C123GXL_WatchdogName {
    EK_TM4C123GXL_WATCHDOG0 = 0,
    EK_TM4C123GXL_WATCHDOG1,
    EK_TM4C123GXL_WATCHDOGCOUNT
} EK_TM4C123GXL_WatchdogName;

/*!
 *  @def    EK_TM4C123GXL_WiFiName
 *  @brief  Enum of WiFi names on the EK_TM4C123GXL dev board
 */
typedef enum EK_TM4C123GXL_WiFiName {
    EK_TM4C123GXL_WIFI = 0,

    EK_TM4C123GXL_WIFICOUNT
} EK_TM4C123GXL_WiFiName;

typedef enum SYSTEM_SPEED_INDEX
{
  INEEDMD_CPU_SPEED_NOT_SET = 0x00,
  INEEDMD_CPU_SPEED_FULL_EXTERNAL,
  INEEDMD_CPU_SPEED_HALF_EXTERNAL,
  INEEDMD_CPU_SPEED_QUARTER_EXTERNAL,
  INEEDMD_CPU_SPEED_FULL_INTERNAL,
  INEEDMD_CPU_SPEED_HALF_INTERNAL,
  INEEDMD_CPU_SPEED_HALF_INTERNAL_OSC,
  INEEDMD_CPU_SPEED_SLOW_INTERNAL,
  INEEDMD_CPU_SPEED_REALLY_SLOW,
  INEEDMD_CPU_SPEED_INDEX_COUNT
}eSYSTEM_SPEED_INDEX;

/******************************************************************************
*public structures ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
/*!
 *  @brief  Initialize board specific DMA settings
 *
 *  This function creates a hwi in case the DMA controller creates an error
 *  interrrupt, enables the DMA and supplies it with a uDMA control table.
 */
extern void EK_TM4C123GXL_initDMA(void);

/*!
 *  @brief  Initialize board specific EEPROM settings
 *
 */
extern void EK_TM4C123GXL_initEEPROM(void);

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings. This include
 *     - Flash wait states based on the process
 *     - Disable clock source to watchdog module
 *     - Enable clock sources for peripherals
 */
extern void EK_TM4C123GXL_initGeneral(void);

/*!
 *  @brief  Initialize board specific GPIO settings
 *
 *  This function initializes the board specific GPIO settings and
 *  then calls the GPIO_init API to initialize the GPIO module.
 *
 *  The GPIOs controlled by the GPIO module are determined by the GPIO_config
 *  variable.
 */
extern void EK_TM4C123GXL_initGPIO(void);

/*!
 *  @brief  Initialize board specific I2C settings
 *
 *  This function initializes the board specific I2C settings and then calls
 *  the I2C_init API to initialize the I2C module.
 *
 *  The I2C peripherals controlled by the I2C module are determined by the
 *  I2C_config variable.
 */
extern void EK_TM4C123GXL_initI2C(void);

/*!
 *  @brief  Initialize board specific PWM settings
 *
 *  This function initializes the board specific PWM settings and then calls
 *  the PWM_init API to initialize the PWM module.
 *
 *  The PWM peripherals controlled by the I2C module are determined by the
 *  PWM_config variable.
 */
extern void EK_TM4C123GXL_initPWM(void);

/*!
 *  @brief  Initialize board specific SDSPI settings
 *
 *  This function initializes the board specific SDSPI settings and then calls
 *  the SDSPI_init API to initialize the SDSPI module.
 *
 *  The SDSPI peripherals controlled by the SDSPI module are determined by the
 *  SDSPI_config variable.
 */
extern void EK_TM4C123GXL_initSDSPI(void);

/*!
 *  @brief  Initialize board specific SPI settings
 *
 *  This function initializes the board specific SPI settings and then calls
 *  the SPI_init API to initialize the SPI module.
 *
 *  The SPI peripherals controlled by the SPI module are determined by the
 *  SPI_config variable.
 */
extern void EK_TM4C123GXL_initSPI(void);

/*!
 *  @brief  Initialize board specific UART settings
 *
 *  This function initializes the board specific UART settings and then calls
 *  the UART_init API to initialize the UART module.
 *
 *  The UART peripherals controlled by the UART module are determined by the
 *  UART_config variable.
 */
extern void EK_TM4C123GXL_initUART(void);

/*!
 *  @brief  Initialize board specific USB settings
 *
 *  This function initializes the board specific USB settings and pins based on
 *  the USB mode of operation.
 *
 *  @param      usbMode    USB mode of operation
 */
extern void EK_TM4C123GXL_initUSB(EK_TM4C123GXL_USBMode usbMode);

/*!
 *  @brief  Initialize board specific ADC settings
 *
 *  This function initializes the board specific ADC settings and pins
 *
 */
extern void EK_TM4C123GXL_initADC(void);

/*!
 *  @brief  Initialize board specific Watchdog settings
 *
 *  This function initializes the board specific Watchdog settings and then
 *  calls the Watchdog_init API to initialize the Watchdog module.
 *
 *  The Watchdog peripherals controlled by the Watchdog module are determined
 *  by the Watchdog_config variable.
 */
extern void EK_TM4C123GXL_initWatchdog(void);

/*!
 *  @brief  Initialize board specific WiFi settings
 *
 *  This function initializes the board specific WiFi settings and then calls
 *  the WiFi_init API to initialize the WiFi module.
 *
 *  The hardware resources controlled by the WiFi module are determined by the
 *  WiFi_config variable.
 */
extern void EK_TM4C123GXL_initWiFi(void);

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
void        vBSP_Power_System_Down(void);
void        vBSP_System_reset(void);
ERROR_CODE  set_system_speed (eSYSTEM_SPEED_INDEX eHow_Fast);
ERROR_CODE  eGet_system_speed(uint16_t * uiSys_speed);
void        Set_Timer0_Sleep();
ERROR_CODE  eBSP_Timer0_Int_Serivce(uint32_t uiInt_Status);
ERROR_CODE  eBSP_Did_Timer0_Expire(bool bClear_status);
void        vSystick_int_service(void);
ERROR_CODE  eBSP_Get_Current_ms_count(uintmax_t * uiCurrent_ms_count);
ERROR_CODE  eBSP_Get_Current_ms(uint16_t * uiCurrent_ms);

void        PowerInitFunction(void);
void        GPIOEnable(void);
void        GPIODisable(void);
ERROR_CODE  BatMeasureADCEnable(void);
int         BatMeasureADCDisable(void);
ERROR_CODE  eBSP_ADCMeasureBatt(uint32_t * puiBatt_voltage);
ERROR_CODE  eBSP_TemperatureMeasureADCEnable(void);
ERROR_CODE  eBSP_TemperatureMeasureADCDisable(void);
bool        bIs_battery_low(void);

//Radio bsp api functions
ERROR_CODE  eBSP_Set_radio_uart_baud(uint32_t uiBaud_rate_to_set);
ERROR_CODE  eBSP_Get_radio_uart_baud(uint32_t * uiBaud_rate_to_get);
ERROR_CODE  eBSP_Set_radio_uart_to_blocking(void);
ERROR_CODE  eBSP_Set_radio_uart_to_callback(void);
ERROR_CODE  eBSP_RadioUARTEnable(void);
ERROR_CODE  eBSP_Radio_Power_On(void);
ERROR_CODE  eBSP_Radio_Power_Off(void);
ERROR_CODE  eBSP_Radio_Power_Cycle(void);
ERROR_CODE  eBSP_Radio_Enable(void);
ERROR_CODE  eBSP_Radio_Disable(void);
ERROR_CODE  eBSP_Radio_Disable(void);
ERROR_CODE  eBSP_Radio_Reset(void);
ERROR_CODE  eBSP_Set_radio_to_cmnd_mode(void);  //sets the uP radio command mode pin
ERROR_CODE  eIs_radio_in_cmnd_mode(void);
ERROR_CODE  eBSP_Set_radio_to_data_mode(void);  //clears the uP radio command mode pin
ERROR_CODE  eUsing_radio_uart_dma(void);
ERROR_CODE  eBSP_Radio_send_byte(uint8_t * uiSend_Byte);
ERROR_CODE  eBSP_Radio_send_frame(uint8_t *cSend_frame, uint16_t uiFrame_len);
ERROR_CODE  eBSP_Radio_send_frame_halt(void);
ERROR_CODE  eBSP_Radio_rcv_string(char *cRcv_string, uint16_t uiBuff_size);
ERROR_CODE  eBSP_Radio_rcv_char(char *cRcv_char);
ERROR_CODE  eBSP_Radio_rcv_frame(uint8_t * cRcv_frame, uint32_t uiFrame_len);
ERROR_CODE  eBSP_Radio_rcv_frame_halt(void);
ERROR_CODE  eBSP_Radio_clear_rcv_buffer(void);
ERROR_CODE  eBSP_Get_Radio_CTS_status(void);  //returns the radio UART cts status
ERROR_CODE  eBSP_Get_Radio_CD_status(void);
ERROR_CODE  eBSP_Get_Radio_low_bat_status(void);

void        LEDI2CEnable(void);
void        XTALControlPin(void);

//USB bsp api functions
void        USBPortEnable(void);
void        USBPortDisable(void);
void        vUSBServiceInt(uint32_t uiUSB_int_flags);
bool        bIs_usb_physical_data_conn(bool bClear_Status);
ERROR_CODE  eBSP_USB_is_physical_connection(void);
void        vBSP_USB_DFU(void);

//ADC bsp api functions
ERROR_CODE  eBSP_ADC_Hard_Reset       (void);    //gpio toggle to the adc reset pin
ERROR_CODE  eBSP_ADC_Reset            (bool bReset);  //gpio set or clear the reset pin
ERROR_CODE  eBSP_ADC_Start            (bool bStart);  //gpio set or clear of the adc start pin
ERROR_CODE  eBSP_ADC_Select           (bool bSelect);//gpio set or clear the chip select pin
ERROR_CODE  eBSP_ADC_Power            (bool bPower);  //gpio set or clear of the adc power pin
ERROR_CODE  eBSP_ADC_Data_command     (uint8_t uiCommand);
ERROR_CODE  eBSP_ADC_Data_frame_read        (uint8_t *pData, uint16_t uiFrameLength);
ERROR_CODE  eBSP_ADC_Data_single_read        (uint8_t *pData);
ERROR_CODE  eBSP_ADC_Data_ADS_read    (uint8_t * pData, uint16_t uiNum_Bytes);
ERROR_CODE  eBSP_ADC_Register_Read    (uint8_t address, uint8_t * uiRX_Data);
ERROR_CODE  eBSP_ADC_Register_Write   (uint8_t address, uint8_t value);
ERROR_CODE  eBSP_ADC_Register_Clear   (uint8_t address, uint8_t value);
ERROR_CODE  eBSP_ADC_SPI_Enable       (void);    //enabling the SPI interface to the ADC
ERROR_CODE  eBSP_ADC_temperature_init (void);
ERROR_CODE  eBSP_ADC_temperature_read (uint32_t * pData);
ERROR_CODE  eBSP_ADC_batt_volt_init   (void);
ERROR_CODE  eBSP_ADC_batt_volt_read   (uint32_t * pData);

ERROR_CODE eBSP_Master_int_enable(void);
ERROR_CODE eBSP_Master_int_disable(void);
int iHW_delay(uint32_t uiDelay);
ERROR_CODE eBSP_debugger_detect(void);  //Does board hardware initalization
ERROR_CODE eBSP_Board_init(void);  //Does board hardware initalization

#endif //  __BOARD_H__