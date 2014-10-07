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
/******************************************************************************
*public defines ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//RTOS init function mappings
#define Board_initDMA               EK_TM4C123GXL_initDMA
#define Board_initGeneral           EK_TM4C123GXL_initGeneral
#define Board_initGPIO              EK_TM4C123GXL_initGPIO
#define Board_initI2C               EK_TM4C123GXL_initI2C
#define Board_initSDSPI             EK_TM4C123GXL_initSDSPI
#define Board_initSPI               EK_TM4C123GXL_initSPI
#define Board_initUART              EK_TM4C123GXL_initUART
#define Board_initUSB               EK_TM4C123GXL_initUSB
#define Board_initWatchdog          EK_TM4C123GXL_initWatchdog
#define Board_initWiFi              EK_TM4C123GXL_initWiFi
//end RTOS init funciton mappings

#define Board_I2C0                  EK_TM4C123GXL_I2C0
#define Board_I2C1                  EK_TM4C123GXL_I2C3
#define Board_I2C_TMP               EK_TM4C123GXL_I2C3
#define Board_I2C_NFC               EK_TM4C123GXL_I2C3
#define Board_I2C_TPL0401           EK_TM4C123GXL_I2C3

#define Board_SDSPI0                EK_TM4C123GXL_SDSPI0

#define Board_SPI0                  EK_TM4C123GXL_SPI0
#define Board_SPI1                  EK_TM4C123GXL_SPI3
#define Board_SPI_CC3000            EK_TM4C123GXL_SPI2
#define Board_SPI_CC3100            EK_TM4C123GXL_SPI2

#define Board_USBDEVICE             EK_TM4C123GXL_USBDEVICE

#define Board_UART0                 EK_TM4C123GXL_UART0

#define Board_WATCHDOG0             EK_TM4C123GXL_WATCHDOG0

#define Board_WIFI                  EK_TM4C123GXL_WIFI

#define Board_gpioCallbacks0        EK_TM4C123GXL_gpioPortFCallbacks
#define Board_gpioCallbacks1        EK_TM4C123GXL_gpioPortFCallbacks

/* Board specific I2C addresses */
#define Board_TMP006_ADDR           (0x40)
#define Board_RF430CL330_ADDR       (0x28)
#define Board_TPL0401_ADDR          (0x40)

//A to D Converter port mappings
#define INEEDMD_ADC_GPIO_PORT           GPIO_PORTA_BASE
//#define INEEDMD_PORTA_ADC_PWRDN_OUT_PIN GPIO_PIN_7 //0x80
//#define INEEDMD_PORTA_ADC_RESET_OUT_PIN GPIO_PIN_6 //0X40
//#define INEEDMD_PORTA_ADC_INTERUPT_PIN  GPIO_PIN_0 //0x01
//#define INEEDMD_PORTA_ADC_START_PIN     GPIO_PIN_1 //0x02
//#define INEEDMD_PORTA_ADC_nCS_PIN       GPIO_PIN_3 //0x08
#define INEEDMD_ADC_PWR_PIN       GPIO_PIN_7 //0x80
#define INEEDMD_ADC_RESET_PIN     GPIO_PIN_6 //0X40
#define INEEDMD_ADC_INTERUPT_PIN  GPIO_PIN_0 //0x01
#define INEEDMD_ADC_START_PIN     GPIO_PIN_1 //0x02
#define INEEDMD_ADC_nCS_PIN       GPIO_PIN_3 //0x08
//port mappings to make easier to read names...
#define INEEDMD_ADC_SPI     SSI0_BASE
#define INEEDMD_FLASH_SPI   SSI1_BASE
#define INEEDMD_SPI_CLK                 16000000

//EKG Lead reading mappings
#define LEAD_SHORT                     0x82FF
#define LEAD_SHORT_RESET               0xA0FF
#define LEAD_SHORT_SLEEP               0xA2FF  //5 & 6 short
#define LEAD_ALL_SHORTED               0x00FF

//battery port mappings
#define INEEDMD_BATTERY_PORT                 GPIO_PORTE_BASE
#define INEEDMD_BATTERY_MEASUREMENT_IN_PIN   GPIO_PIN_3
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
  #define INEEDMD_RADIO_RESET_PIN                GPIO_PIN_0
  #define INEEDMD_RADIO_RESET_PIN_SET            GPIO_PIN_0
  #define INEEDMD_RADIO_RESET_PIN_CLEAR          0
//Radio command mode mappings
  #define INEEDMD_GPIO_CMND_PORT                 GPIO_PORTE_BASE
  #define INEEDMD_RADIO_CMND_PIN                 GPIO_PIN_1
  #define INEEDMD_RADIO_CMND_PIN_SET             GPIO_PIN_1
  #define INEEDMD_RADIO_CMND_PIN_CLEAR           0
//Radio enable mappings
  #define INEEDMD_GPIO_EN_PORT                   GPIO_PORTE_BASE
  #define INEEDMD_RADIO_ENABLE_PIN               GPIO_PIN_2
  #define INEEDMD_RADIO_ENABLE_PIN_SET           GPIO_PIN_2
  #define INEEDMD_RADIO_ENABLE_PIN_CLEAR         0
//Radio low batt mappings
//  #define INEEDMD_RADIO_LOW_BATT_INTERUPT_PIN    GPIO_PIN_1  //todo no longer will be on pin 1
//Radio interrupt mappings
  #define INEEDMD_RADIO_UART_INT                 INT_UART1

//External clock mappings
  #define INEEDMD_XTAL_PORT              GPIO_PORTD_BASE
  #define INEEDMD_XTAL_ENABLE_PIN        GPIO_PIN_5
  #define INEEDMD_XTAL_ENABLE_PIN_SET    GPIO_PIN_5
  #define INEEDMD_XTAL_ENABLE_PIN_CLEAR  0

//I2C LED controller mappings
#define INEEDMD_LED_I2C_BASE          I2C0_BASE
#define INEEDMD_LED_I2C_INT           INT_I2C0
#define INEEDMD_LED_SYSCTL_PRIPH_I2C  SYSCTL_PERIPH_I2C0
#define INEEDMD_LED_GPIO_PORT         GPIO_PORTB_BASE
#define INEEDMD_LED_I2CSCL_PIN        GPIO_PIN_2
#define INEEDMD_LED_I2CSDA_PIN        GPIO_PIN_3
#define INEEDMD_LED_GPIO_I2CSCL       GPIO_PB2_I2C0SCL
#define INEEDMD_LED_GPIO_I2CSDA       GPIO_PB3_I2C0SDA

//USB mappings
#define INEEDMD_USB         USB0_BASE

//subsystem block number - just used to make some coding functions easer.
#define INEEDMD_CPU    0x00
#define INEEDMD_ADC    0x01
#define INEEDMD_FLASH  0x02
#define INEEDMD_LED    0x03
#define INEEDMD_RADIO  0x04
#define INEED_USB      0x05

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

/* LEDs on EK_TM4C123GXL are active high. */
#define EK_TM4C123GXL_LED_OFF (0)
#define EK_TM4C123GXL_LED_ON  (~0)

/******************************************************************************
*public variables /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
/* GPIO_Callbacks structure for GPIO interrupts */
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
  EK_TM4C123GXL_RADIO_RESET,
  EK_TM4C123GXL_XTAL_ENABLE,
  EK_TM4C123GXL_ADC_POWER,
  EK_TM4C123GXL_ADC_RESET,
  EK_TM4C123GXL_ADC_INTERUPT,
  EK_TM4C123GXL_ADC_START,
  EK_TM4C123GXL_ADC_nCS,
  EK_TM4C123GXL_GPIOCOUNT

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
    EK_TM4C123GXL_UART0 = 0,
    EK_TM4C123GXL_UART1,
    EK_TM4C123GXL_UART5,
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
void        wait_time (unsigned int);
void        write_2_byte_i2c (unsigned char, unsigned char, unsigned char);
ERROR_CODE  set_system_speed (eSYSTEM_SPEED_INDEX eHow_Fast);
ERROR_CODE  eGet_system_speed(uint16_t * uiSys_speed);
void        Set_Timer0_Sleep();
ERROR_CODE  eBSP_Timer0_Int_Serivce(uint32_t uiInt_Status);
ERROR_CODE  eBSP_Did_Timer0_Expire(bool bClear_status);
void        vSystick_int_service(void);
ERROR_CODE  eBSP_Get_Current_ms_count(uintmax_t * uiCurrent_ms_count);
ERROR_CODE  eBSP_Get_Current_ms(uint16_t * uiCurrent_ms);
bool        bWaveform_did_timer_tick(void);
void        PowerInitFunction(void);
void        GPIOEnable(void);
void        GPIODisable(void);
ERROR_CODE  BatMeasureADCEnable(void);
int         BatMeasureADCDisable(void);
ERROR_CODE  eBSP_ADCMeasureBatt(uint32_t * puiBatt_voltage);
ERROR_CODE  TemperatureMeasureADCEnable(void);
ERROR_CODE  TemperatureMeasureADCDisable(void);
bool        bIs_battery_low(void);
int         EKGSPIEnable(void);
int         EKGSPIDisable(void);
ERROR_CODE  eBSP_Set_radio_uart_baud(uint32_t uiBaud_rate_to_set);
ERROR_CODE  eBSP_Get_radio_uart_baud(uint32_t * uiBaud_rate_to_get);
ERROR_CODE  eBSP_RadioUARTEnable(void);
int         RadioUARTDisable(void);
int         iRadio_Power_On(void);
int         iRadio_Power_Off(void);
ERROR_CODE  eBSP_Radio_Power_Cycle(void);
ERROR_CODE  eBSP_Radio_Enable(void);
ERROR_CODE  eBSP_Radio_Disable(void);
ERROR_CODE  eBSP_Radio_Disable(void);
ERROR_CODE  eBSP_Radio_Reset(void);
ERROR_CODE  eSet_radio_to_cmnd_mode(void);  //sets the uP radio command mode pin
ERROR_CODE  eIs_radio_in_cmnd_mode(void);
ERROR_CODE  eSet_radio_to_data_mode(void);  //clears the uP radio command mode pin
ERROR_CODE  eUsing_radio_uart_dma(void);
ERROR_CODE  eRadio_interface_enable(void);
int         iRadio_gpio_set(uint16_t uiMask);
int         iRadio_gpio_clear(uint16_t uiMask);
int         iRadio_gpio_read(uint16_t uiMask);
int         iRadio_gpio_config(uint32_t uiRadio_Pin_Port, uint8_t uiPIN_Out_Mask);
int         iRadio_send_char(char * byte);
int         iRadio_send_string(char *cSend_string, uint16_t uiBuff_size);
ERROR_CODE  eRadio_DMA_send_string(char *cSend_string, uint16_t uiBuff_size);
int         iRadio_send_frame(uint8_t *cSend_frame, uint16_t uiFrame_size);
ERROR_CODE  eRadio_rcv_string(char *cRcv_string, uint16_t uiBuff_size);
ERROR_CODE  iRadio_rcv_char(char *cRcv_char);
int         iRadio_rcv_byte(uint8_t *uiRcv_byte);
ERROR_CODE  eRadio_clear_rcv_buffer(void);
ERROR_CODE  eRcv_dma_radio_cmnd_frame(char * cRcv_buff, uint16_t uiMax_buff_size);
ERROR_CODE  eRcv_dma_radio_boot_frame(char * cRcv_buff, uint16_t uiMax_buff_size);
ERROR_CODE  eIs_UART_using_DMA(void);
int         iRadio_interface_int_enable(void);
int         iRadio_interface_int_disable(void);
void        vRadio_interface_int_service(uint32_t uiInt_id);
void        vRadio_UARTTx_int_service(uint32_t ui32Status);
void        vRadio_interface_DMA_int_service(uint32_t ui32DMA_int_status);
void        vRadio_interface_int_service_timeout(uint16_t uiInt_id);
bool        bRadio_is_data(void);
ERROR_CODE  eGet_Radio_CTS_status(void);  //returns the radio UART cts status
ERROR_CODE  eGet_Radio_CTS_INT_status(void);  //returns the radio UART CTS interrupt status
void        LEDI2CEnable(void);
void        XTALControlPin(void);
void        USBPortEnable(void);
void        USBPortDisable(void);
void        vUSBServiceInt(uint32_t uiUSB_int_flags);
bool        bIs_usb_physical_data_conn(bool bClear_Status);
ERROR_CODE  ineedmd_usb_connected(void);
ERROR_CODE  eBSP_Systick_Init(void);
void        PortFunctionInit(void);
void        ConfigureSleep(void);
void        ConfigureDeepSleep(void);
void        LEDI2CDisable(void);
ERROR_CODE  eBSP_LEDI2C_clock_set(void);
void USBPortDisable(void);
ERROR_CODE eMaster_int_enable(void);
ERROR_CODE eMaster_int_disable(void);
int iHW_delay(uint32_t uiDelay);
ERROR_CODE eBSP_Board_init(void);  //Does board hardware initalization

#endif //  __BOARD_H__
