/*
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== EK_TM4C123GXL.c ========
 *  This file is responsible for setting up the board specific items for the
 *  EK_TM4C123GXL board.
 *
 *  The following defines are used to determine which TI-RTOS peripheral drivers
 *  to include:
 *     TI_DRIVERS_GPIO_INCLUDED
 *     TI_DRIVERS_I2C_INCLUDED
 *     TI_DRIVERS_SDSPI_INCLUDED
 *     TI_DRIVERS_SPI_INCLUDED
 *     TI_DRIVERS_UART_INCLUDED
 *     TI_DRIVERS_WATCHDOG_INCLUDED
 *     TI_DRIVERS_WIFI_INCLUDED
 *  These defines are created when a useModule is done on the driver in the
 *  application's .cfg file. The actual #define is in the application
 *  generated header file that is brought in via the xdc/cfg/global.h.
 *  For example the following in the .cfg file
 *     var GPIO = xdc.useModule('ti.drivers.GPIO');
 *  Generates the following
 *     #define TI_DRIVERS_GPIO_INCLUDED 1
 *  If there is no useModule of ti.drivers.GPIO, the constant is set to 0.
 *
 *  Note: a useModule is generated in the .cfg file via the graphical
 *  configuration tool when the "Add xxx to my configuration" is checked
 *  or "Use xxx" is selected.
 */

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_ints.h>
#include <inc/hw_gpio.h>

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/i2c.h>
#include <driverlib/ssi.h>
#include <driverlib/udma.h>
#include <driverlib/pin_map.h>
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>

#include "utils_inc/proj_debug.h"
#include "utils_inc/error_codes.h"
#include "board.h"
//#include "EK_TM4C123GXL.h"

#define WD_PAT40  0x2625A00  //about 1 seconds at 40Mhz
#define WD_PAT80  0x4C4B400  //about 1 seconds at 80Mhz

#if defined(ccs)
#pragma DATA_ALIGN(EK_TM4C123GXL_DMAControlTable, 1024)
#elif defined(ewarm)
#pragma data_alignment=1024
#elif defined(gcc)
__attribute__ ((aligned (1024)))
#endif
static tDMAControlTable EK_TM4C123GXL_DMAControlTable[32];
static bool DMA_initialized = false;

extern void vUSB_hardware_int_callback(void);

/* Hwi_Struct used in the initDMA Hwi_construct call */
static Hwi_Struct hwiStruct;

/*
 *  ======== EK_TM4C123GXL_errorDMAHwi ========
 */
static Void EK_TM4C123GXL_errorDMAHwi(UArg arg)
{
    System_printf("DMA error code: %d\n", uDMAErrorStatusGet());
    uDMAErrorStatusClear();
    System_abort("DMA error!!");
}

/*
 *  ======== EK_TM4C123GXL_initDMA ========
 */
void EK_TM4C123GXL_initDMA(void)
{
    Error_Block eb;
    Hwi_Params  hwiParams;

    if(!DMA_initialized){

        Error_init(&eb);

        Hwi_Params_init(&hwiParams);
        Hwi_construct(&(hwiStruct), INT_UDMAERR, EK_TM4C123GXL_errorDMAHwi,
                      &hwiParams, &eb);
        if (Error_check(&eb)) {
            System_abort("Couldn't create DMA error hwi");
        }

        SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
        uDMAEnable();
        uDMAControlBaseSet(EK_TM4C123GXL_DMAControlTable);

        DMA_initialized = true;
    }
}

/*
 *  ======== EK_TM4C123GXL_initGeneral ========
 */
void EK_TM4C123GXL_initGeneral(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

#ifdef TI_DRIVERS_GPIO_INCLUDED
#include <ti/drivers/GPIO.h>

/* Callback functions for the GPIO interrupt example. */
void gpioButtonFxn0(void);
void gpioButtonFxn1(void);

/* GPIO configuration structure */
const GPIO_HWAttrs gpioHWAttrs[EK_TM4C123GXL_GPIOCOUNT] = {
        {DEBUG_GPIO_PORT_1,      DEBUG_GPIO_PIN_1,         GPIO_OUTPUT},
        {INEEDMD_GPIO_EN_PORT,   INEEDMD_RADIO_ENABLE_PIN, GPIO_OUTPUT},
        {INEEDMD_GPIO_CMND_PORT, INEEDMD_RADIO_CMND_PIN,   GPIO_OUTPUT},
        {INEEDMD_GPIO_RST_PORT,  INEEDMD_RADIO_RESET_PIN,  GPIO_OUTPUT},
        {INEEDMD_XTAL_PORT,      INEEDMD_XTAL_ENABLE_PIN,  GPIO_OUTPUT},
        {INEEDMD_ADC_GPIO_PORT,  INEEDMD_ADC_PWR_PIN,      GPIO_OUTPUT},
        {INEEDMD_ADC_GPIO_PORT,  INEEDMD_ADC_RESET_PIN,    GPIO_OUTPUT},
        {INEEDMD_ADC_GPIO_PORT,  INEEDMD_ADC_INTERUPT_PIN, GPIO_INPUT},
        {INEEDMD_ADC_GPIO_PORT,  INEEDMD_ADC_START_PIN,    GPIO_OUTPUT},
        {INEEDMD_ADC_GPIO_PORT,  INEEDMD_ADC_nCS_PIN,      GPIO_OUTPUT},
        {GPIO_PORTB_BASE,  GPIO_PIN_4,      GPIO_INPUT},
};

/* Memory for the GPIO module to construct a Hwi */
Hwi_Struct callbackHwi;

/* GPIO callback structure to set callbacks for GPIO interrupts */
const GPIO_Callbacks EK_TM4C123GXL_gpioPortBCallbacks = {
    GPIO_PORTB_BASE, INT_GPIOB, &callbackHwi,
    {NULL, NULL, NULL, NULL, vUSB_hardware_int_callback, NULL, NULL, NULL}
};

/* GPIO config structure, must be called in order of the gpio name */
const GPIO_Config GPIO_config[] = {
    {&gpioHWAttrs[EK_TM4C123GXL_DEBUG]},
    {&gpioHWAttrs[EK_TM4C123GXL_RADIO_POWER]},
    {&gpioHWAttrs[EK_TM4C123GXL_RADIO_CMND_MODE]},
    {&gpioHWAttrs[EK_TM4C123GXL_RADIO_RESET]},
    {&gpioHWAttrs[EK_TM4C123GXL_XTAL_ENABLE]},
    {&gpioHWAttrs[EK_TM4C123GXL_ADC_POWER]},
    {&gpioHWAttrs[EK_TM4C123GXL_ADC_RESET]},
    {&gpioHWAttrs[EK_TM4C123GXL_ADC_INTERUPT]},
    {&gpioHWAttrs[EK_TM4C123GXL_ADC_START]},
    {&gpioHWAttrs[EK_TM4C123GXL_ADC_nCS]},
    {&gpioHWAttrs[EK_TM4C123GXL_USB_DETECT]},
    {NULL},
};

/*
 *  ======== EK_TM4C123GXL_initGPIO ========
 */
void EK_TM4C123GXL_initGPIO(void)
{
//    /* Setup the LED GPIO pins used */
//    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1); /* EK_TM4C123GXL_LED_RED */
//    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2); /* EK_TM4C123GXL_LED_GREEN */
//    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3); /* EK_TM4C123GXL_LED_BLUE */
//
//    /* Setup the button GPIO pins used */
//    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);  /* EK_TM4C123GXL_GPIO_SW1 */
//    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
//
//    /* PF0 requires unlocking before configuration */
//    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;
//    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);  /* EK_TM4C123GXL_GPIO_SW2 */
//    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
//    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_M;

  //Init debug gpio pin/pins
  //
  MAP_GPIOPinTypeGPIOOutput(DEBUG_GPIO_PORT_1, DEBUG_GPIO_PIN_1);  //debug pin

  //Init radio power enable pin
  //
  MAP_GPIOPinTypeGPIOOutput(INEEDMD_GPIO_EN_PORT, INEEDMD_RADIO_ENABLE_PIN);  //radio power enable pin

  //Init radio command mode pin
  //
  MAP_GPIOPinTypeGPIOOutput(INEEDMD_GPIO_CMND_PORT, INEEDMD_RADIO_CMND_PIN);  //radio command mode pin

  //Init radio reset pin
  //
  MAP_GPIOPinTypeGPIOOutput(INEEDMD_GPIO_RST_PORT, INEEDMD_RADIO_RESET_PIN);  //radio reset pin

  //Init external clock enable pin
  //
  MAP_GPIOPinTypeGPIOOutput(INEEDMD_XTAL_PORT, INEEDMD_XTAL_ENABLE_PIN);  //external clock enable pin

  //Init ADC power on pin
  //
  MAP_GPIOPinTypeGPIOOutput(INEEDMD_ADC_GPIO_PORT, INEEDMD_ADC_PWR_PIN);

  //Init ADC reset pin
  //
  MAP_GPIOPinTypeGPIOOutput(INEEDMD_ADC_GPIO_PORT, INEEDMD_ADC_RESET_PIN);

  // Init ADC interrupt pin
  //
  MAP_GPIOPinTypeGPIOInput(INEEDMD_ADC_GPIO_PORT, INEEDMD_ADC_INTERUPT_PIN);

  // Init ADC start pin
  //
  MAP_GPIOPinTypeGPIOOutput(INEEDMD_ADC_GPIO_PORT, INEEDMD_ADC_START_PIN);

  //Init ADC nCS pin
  //
  MAP_GPIOPinTypeGPIOOutput(INEEDMD_ADC_GPIO_PORT, INEEDMD_ADC_nCS_PIN);

  //Init USB unplug detect pin
  //
  HWREG(GPIO_PORTB_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  HWREG(GPIO_PORTB_BASE + GPIO_O_CR) |= GPIO_PIN_4;
  MAP_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_4);
  MAP_GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD);
  HWREG(GPIO_PORTB_BASE + GPIO_O_LOCK) = GPIO_LOCK_M;

  /* Once GPIO_init is called, GPIO_config cannot be changed */
  GPIO_init();

}
#endif /* TI_DRIVERS_GPIO_INCLUDED */

#if TI_DRIVERS_I2C_INCLUDED
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CTiva.h>

/* I2C objects */
I2CTiva_Object i2cTivaObjects[EK_TM4C123GXL_I2CCOUNT];

/* I2C configuration structure, describing which pins are to be used */
const I2CTiva_HWAttrs i2cTivaHWAttrs[EK_TM4C123GXL_I2CCOUNT] = {
//  {INEEDMD_I2C_BASE_ADDRESS, INEEDMD_I2C_INTERUPT_ADDRESS},
  {INEEDMD_LED_I2C_BASE, INEEDMD_LED_I2C_INT},
};

const I2C_Config I2C_config[] = {
    {&I2CTiva_fxnTable, &i2cTivaObjects[0], &i2cTivaHWAttrs[0]},
    {NULL, NULL, NULL}
};

/*
 *  ======== EK_TM4C123GXL_initI2C ========
 */
void EK_TM4C123GXL_initI2C(void)
{
  //Enable the peripheral funcitonality for the GPIO port
  //
  MAP_SysCtlPeripheralEnable(INEEDMD_LED_SYSCTL_PRIPH_GPIO);

  // Enable the I2C peripheral
  //
  MAP_SysCtlPeripheralEnable(INEEDMD_LED_SYSCTL_PRIPH_I2C);

  // Configure the alternate function for the I2C SCL pin and tie it to the I2C
  MAP_GPIOPinTypeI2CSCL(INEEDMD_LED_GPIO_PORT, INEEDMD_LED_I2CSCL_PIN);
  MAP_GPIOPinConfigure(INEEDMD_LED_GPIO_I2CSCL);

  //Configure the alternate function for the I2C SDA pin and tie it to the I2C
  MAP_GPIOPinTypeI2C(INEEDMD_LED_GPIO_PORT, INEEDMD_LED_I2CSDA_PIN);
  MAP_GPIOPinConfigure(INEEDMD_LED_GPIO_I2CSDA);

  I2C_init();

  return;
}
#endif /* TI_DRIVERS_I2C_INCLUDED */

#if TI_DRIVERS_SDSPI_INCLUDED
#include <ti/drivers/SDSPI.h>
#include <ti/drivers/sdspi/SDSPITiva.h>

/* SDSPI objects */
SDSPITiva_Object sdspiTivaobjects[EK_TM4C123GXL_SDSPICOUNT];

/* SDSPI configuration structure, describing which pins are to be used */
const SDSPITiva_HWAttrs sdspiTivaHWattrs[EK_TM4C123GXL_SDSPICOUNT] = {
    {
        INEEDMD_SD_SSI_BASE,          /* SPI base address */

//        GPIO_PORTB_BASE,    /* The GPIO port used for the SPI pins */
//        GPIO_PIN_4,         /* SCK */
//        GPIO_PIN_6,         /* MISO */
//        GPIO_PIN_7,         /* MOSI */
         GPIO_PORTD_BASE,    /* The GPIO port used for the SPI pins */
         GPIO_PIN_0,         /* SCK */
         GPIO_PIN_3,         /* MISO */
         GPIO_PIN_2,         /* MOSI */

//        GPIO_PORTA_BASE,    /* Chip select port */
//        GPIO_PIN_5,         /* Chip select pin */
         GPIO_PORTD_BASE,    /* Chip select port */
         GPIO_PIN_1,         /* Chip select pin */

//        GPIO_PORTB_BASE,    /* GPIO TX port */
//        GPIO_PIN_7,         /* GPIO TX pin */
         GPIO_PORTD_BASE,    /* GPIO TX port */
         GPIO_PIN_2,         /* GPIO TX pin */
    }
};

const SDSPI_Config SDSPI_config[] = {
    {&SDSPITiva_fxnTable, &sdspiTivaobjects[0], &sdspiTivaHWattrs[0]},
    {NULL, NULL, NULL}
};

/*
 *  ======== EK_TM4C123GXL_initSDSPI ========
 */
void EK_TM4C123GXL_initSDSPI(void)
{
  /* Enable the peripherals used by the SD Card */
  MAP_SysCtlPeripheralEnable(INEEDMD_SD_SPI_SYSCTL_PERIPH);

  /* Configure pad settings */
  GPIOPadConfigSet(INEEDMD_SD_GPIO_PORT,
      INEEDMD_SD_SCK_PIN | INEEDMD_SD_MOSI_PIN,
            GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

  GPIOPadConfigSet(INEEDMD_SD_GPIO_PORT,
      INEEDMD_SD_MISO_PIN,
          GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);

  GPIOPadConfigSet(INEEDMD_SD_CS_GPIO_PORT,
      INEEDMD_SD_CS_PIN,
          GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

  GPIOPinConfigure(INEEDMD_SD_SCK_SSI);
  GPIOPinConfigure(INEEDMD_SD_MISO_SSI);
  GPIOPinConfigure(INEEDMD_SD_MOSI_SSI);

//  /*
//   * These GPIOs are connected to PB6 and PB7 and need to be brought into a
//   * GPIO input state so they don't interfere with SPI communications.
//   */
//  GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0);
//  GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_1);

  SDSPI_init();
}
#endif /* TI_DRIVERS_SDSPI_INCLUDED */

#if TI_DRIVERS_SPI_INCLUDED
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPITivaDMA.h>

/* SPI objects */
SPITivaDMA_Object spiTivaDMAobjects[EK_TM4C123GXL_SPICOUNT];
#if defined(ccs)
#pragma DATA_ALIGN(spiTivaDMAscratchBuf, 32)
#elif defined(ewarm)
#pragma data_alignment=32
#elif defined(gcc)
__attribute__ ((aligned (32)))
#endif
uint32_t spiTivaDMAscratchBuf[EK_TM4C123GXL_SPICOUNT];

/* SPI configuration structure, describing which pins are to be used */
const SPITivaDMA_HWAttrs spiTivaDMAHWAttrs[EK_TM4C123GXL_SPICOUNT] = {
    {
        SSI0_BASE,
        INT_SSI0,
        &spiTivaDMAscratchBuf[0],
        0,
        UDMA_CHANNEL_SSI0RX,
        UDMA_CHANNEL_SSI0TX,
        uDMAChannelAssign,
        UDMA_CH10_SSI0RX,
        UDMA_CH11_SSI0TX
    },
//    {
//        SSI2_BASE,
//        INT_SSI2,
//        &spiTivaDMAscratchBuf[1],
//        0,
//        UDMA_SEC_CHANNEL_UART2RX_12,
//        UDMA_SEC_CHANNEL_UART2TX_13,
//        uDMAChannelAssign,
//        UDMA_CH12_SSI2RX,
//        UDMA_CH13_SSI2TX
//    },
//    {
//        SSI3_BASE,
//        INT_SSI3,
//        &spiTivaDMAscratchBuf[2],
//        0,
//        UDMA_SEC_CHANNEL_TMR2A_14,
//        UDMA_SEC_CHANNEL_TMR2B_15,
//        uDMAChannelAssign,
//        UDMA_CH14_SSI3RX,
//        UDMA_CH15_SSI3TX
//    }
};

const SPI_Config SPI_config[] = {
    {&SPITivaDMA_fxnTable, &spiTivaDMAobjects[0], &spiTivaDMAHWAttrs[0]},
//    {&SPITivaDMA_fxnTable, &spiTivaDMAobjects[1], &spiTivaDMAHWAttrs[1]},
//    {&SPITivaDMA_fxnTable, &spiTivaDMAobjects[2], &spiTivaDMAHWAttrs[2]},
    {NULL, NULL, NULL},
};

/*
 *  ======== EK_TM4C123GXL_initSPI ========
 */
void EK_TM4C123GXL_initSPI(void)
{
  //INEEDMD_ADC_SPI
  //
  MAP_SysCtlPeripheralEnable(INEEDMD_ADC_SYSCTL_PRIPH_SSI);

  // Enable pin SSI CLK
  //
  MAP_GPIOPinConfigure(INEEDMD_ADC_GPIO_SSICLK);

  // Enable pin SSI TX
  //
  MAP_GPIOPinConfigure(INEEDMD_ADC_GPIO_SSITX);

  // Enable pin SSI RX
  //
  MAP_GPIOPinConfigure(INEEDMD_ADC_GPIO_SSIRX);

  //Set the pins to type SSI
  //
  MAP_GPIOPinTypeSSI(INEEDMD_ADC_GPIO_PORT, (INEEDMD_ADC_SSICLK_PIN | INEEDMD_ADC_SSITX_PIN | INEEDMD_ADC_SSIRX_PIN));

  EK_TM4C123GXL_initDMA();
  SPI_init();
}
#endif /* TI_DRIVERS_SPI_INCLUDED */

#if TI_DRIVERS_UART_INCLUDED
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTTiva.h>

/* UART objects */
UARTTiva_Object uartTivaObjects[EK_TM4C123GXL_UARTCOUNT];

/* UART configuration structure */
const UARTTiva_HWAttrs uartTivaHWAttrs[EK_TM4C123GXL_UARTCOUNT] = {
    {UART0_BASE, INT_UART0}, /* EK_TM4C123GXL_UART0 */
    {INEEDMD_RADIO_UART, INT_UART1}, /* EK_TM4C123GXL_UART1 */
    {DEBUG_UART, INT_UART5}, /* EK_TM4C123GXL_UART5 */
};

const UART_Config UART_config[] = {
    // EK_TM4C123GXL_UART0
    {
        &UARTTiva_fxnTable,
        &uartTivaObjects[EK_TM4C123GXL_UART0],
        &uartTivaHWAttrs[EK_TM4C123GXL_UART0]
    },
    // EK_TM4C123GXL_UART1
    {
        &UARTTiva_fxnTable,
        &uartTivaObjects[EK_TM4C123GXL_UART1],
        &uartTivaHWAttrs[EK_TM4C123GXL_UART1]
    },
    // EK_TM4C123GXL_UART5
    {
        &UARTTiva_fxnTable,
        &uartTivaObjects[EK_TM4C123GXL_UART5],
        &uartTivaHWAttrs[EK_TM4C123GXL_UART5]
    },
    {NULL, NULL, NULL}
};

/*
 *  ======== EK_TM4C123GXL_initUART ========
 */
void EK_TM4C123GXL_initUART(void)
{
  // Enable and configure the peripherals used by the radio uart
  //
  MAP_SysCtlPeripheralEnable(INEEDMD_RADIO_SYSCTL_PERIPH_UART);

  // Configure the alternate function for UART RTS pin
  MAP_GPIOPinConfigure(INEEDMD_GPIO_UARTRTS);
  MAP_GPIOPinTypeUART(INEEDMD_RADIO_RTS_PORT, INEEDMD_RADIO_RTS_PIN);

  // Configure the alternate function for UART CTS pin
  MAP_GPIOPinConfigure(INEEDMD_GPIO_UARTCTS);
  MAP_GPIOPinTypeUART(INEEDMD_RADIO_CTS_PORT, INEEDMD_RADIO_CTS_PIN);

  // Configure the alternate function for UART TX and RX pins
  MAP_GPIOPinConfigure(INEEDMD_GPIO_UARTTX);
  MAP_GPIOPinConfigure(INEEDMD_GPIO_UARTRX);

  // Set the TX and RX pin type for the radio uart
  MAP_GPIOPinTypeUART(INEEDMD_GPIO_TX_PORT, INEEDMD_GPIO_TX_PIN);
  MAP_GPIOPinTypeUART(INEEDMD_GPIO_RX_PORT, INEEDMD_GPIO_RX_PIN);

  //Set the UART clock source to internal
  //
//  MAP_UARTClockSourceSet(INEEDMD_RADIO_UART, UART_CLOCK_PIOSC);

  //Config the uart speed, len, stop bits and parity
  //
//  MAP_UARTConfigSetExpClk( INEEDMD_RADIO_UART, INEEDMD_RADIO_UART_CLK, INEEDMD_RADIO_UART_BAUD, ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE ));

  //Enable and configure the peripherals used by the debug uart
  //
  MAP_SysCtlPeripheralEnable(DEBUG_SYSCTL_PERIPH_UART);

  // Configure the debug port pins
  MAP_GPIOPinConfigure(DEBUG_TX_PIN_MUX_MODE);
  MAP_GPIOPinConfigure(DEBUG_RX_PIN_MUX_MODE);

  // Set the debug uart pin types
  MAP_GPIOPinTypeUART(DEBUG_UART_PIN_PORT, DEBUG_TX_PIN);
  MAP_GPIOPinTypeUART(DEBUG_UART_PIN_PORT, DEBUG_RX_PIN);

  //Set the clock source for the debug uart
//  UARTClockSourceSet(DEBUG_UART, UART_CLOCK_PIOSC);

  /* Initialize the UART driver */
  UART_init();
}
#endif /* TI_DRIVERS_UART_INCLUDED */

/*
 *  ======== EK_TM4C123GXL_initUSB ========
 *  This function just turns on the USB
 */
void EK_TM4C123GXL_initUSB(EK_TM4C123GXL_USBMode usbMode)
{
    /* Enable the USB peripheral and PLL */
    MAP_SysCtlPeripheralEnable(INEEDMD_USB_SYSCTL_PERIPH);
    MAP_SysCtlUSBPLLEnable();

    EK_TM4C123GXL_initDMA();

    /* Setup pins for USB operation */
    GPIOPinTypeUSBAnalog(INEEDMD_USB_GPIO_PORT, INEEDMD_USBDP_PIN | INEEDMD_USBDM_PIN);

    if (usbMode == EK_TM4C123GXL_USBHOST) {
        System_abort("USB host not supported\n");
    }
}

#if TI_DRIVERS_USBMSCHFATFS_INCLUDED
#include <ti/drivers/USBMSCHFatFs.h>
#include <ti/drivers/usbmschfatfs/USBMSCHFatFsTiva.h>

/* USBMSCHFatFs objects */
USBMSCHFatFsTiva_Object usbmschfatfstivaObjects[1];

/* USBMSCHFatFs configuration structure, describing which pins are to be used */
const USBMSCHFatFsTiva_HWAttrs usbmschfatfstivaHWAttrs[1] = {
    {INT_USB0}
};

const USBMSCHFatFs_Config USBMSCHFatFs_config[] = {
    {
        &USBMSCHFatFsTiva_fxnTable,
        &usbmschfatfstivaObjects[0],
        &usbmschfatfstivaHWAttrs[0]
    },
    {NULL, NULL, NULL}
};

/*
 *  ======== DK_TM4C123G_initUSBMSCHFatFs ========
 */
void EK_TM4C123GXL_initUSBMSCHFatFs(void)
{
    /* Initialize the DMA control table */
    EK_TM4C123GXL_initDMA();

    /* Call the USB initialization function for the USB Reference modules */
    EK_TM4C123GXL_initUSB(EK_TM4C123GXL_USBDEVICE);
    USBMSCHFatFs_init();
}
#endif /* TI_DRIVERS_USBMSCHFATFS_INCLUDED */

#if TI_DRIVERS_WATCHDOG_INCLUDED
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogTiva.h>

/* Watchdog objects */
WatchdogTiva_Object watchdogTivaObjects[EK_TM4C123GXL_WATCHDOGCOUNT];

/* Watchdog configuration structure */
const WatchdogTiva_HWAttrs watchdogTivaHWAttrs[EK_TM4C123GXL_WATCHDOGCOUNT] = {
    /* EK_TM4C123GXL_WATCHDOG0 with 1 sec period at default CPU clock freq */
    {WATCHDOG0_BASE, INT_WATCHDOG, WD_PAT80},
};

const Watchdog_Config Watchdog_config[] = {
    {&WatchdogTiva_fxnTable, &watchdogTivaObjects[0], &watchdogTivaHWAttrs[0]},
    {NULL, NULL, NULL},
};

/*
 *  ======== EK_TM4C123GXL_initWatchdog ========
 *
 * NOTE: To use the other watchdog timer with base address WATCHDOG1_BASE,
 *       an additional function call may need be made to enable PIOSC. Enabling
 *       WDOG1 does not do this. Enabling another peripheral that uses PIOSC
 *       such as ADC0 or SSI0, however, will do so. Example:
 *
 *       SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
 *       SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG1);
 *
 *       See the following forum post for more information:
 *       http://e2e.ti.com/support/microcontrollers/stellaris_arm_cortex-m3_microcontroller/f/471/p/176487/654390.aspx#654390
 */
void EK_TM4C123GXL_initWatchdog(void)
{
    /* Enable peripherals used by Watchdog */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);

    /* Initialize the Watchdog driver */
    Watchdog_init();
}
#endif /* TI_DRIVERS_WATCHDOG_INCLUDED */

#if TI_DRIVERS_WIFI_INCLUDED
#include <ti/drivers/WiFi.h>

#if TI_DRIVERS_WIFI_CC3000
#include <ti/drivers/wifi/WiFiTivaCC3000.h>

/* WiFi objects */
WiFiTivaCC3000_Object wiFiTivaCC3000Objects[EK_TM4C123GXL_WIFICOUNT];

/* WiFi configuration structure */
const WiFiTivaCC3000_HWAttrs wiFiTivaCC3000HWAttrs[EK_TM4C123GXL_WIFICOUNT] = {
    {
      GPIO_PORTB_BASE, /* IRQ port */
      GPIO_PIN_2,      /* IRQ pin */
      INT_GPIOB,       /* IRQ port interrupt vector */

      GPIO_PORTE_BASE, /* CS port */
      GPIO_PIN_0,      /* CS pin */

      GPIO_PORTB_BASE, /* WLAN EN port */
      GPIO_PIN_5       /* WLAN EN pin */
    }
};

const WiFi_Config WiFi_config[] = {
    {
        &WiFiTivaCC3000_fxnTable,
        &wiFiTivaCC3000Objects[0],
        &wiFiTivaCC3000HWAttrs[0]
    },
    {NULL,NULL, NULL},
};

/*
 *  ======== EK_TM4C123GXL_initWiFi ========
 */
void EK_TM4C123GXL_initWiFi(void)
{
    /* Configure SSI2 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    GPIOPinConfigure(GPIO_PB4_SSI2CLK);
    GPIOPinConfigure(GPIO_PB6_SSI2RX);
    GPIOPinConfigure(GPIO_PB7_SSI2TX);
    GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7);

    /* Configure IRQ pin */
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_FALLING_EDGE);

    /* Configure EN pin */
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0);

    /* Configure CS pin */
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, 0);

    /* Call necessary SPI init functions */
    SPI_init();
    EK_TM4C123GXL_initDMA();

    /* Initialize WiFi driver */
    WiFi_init();
}
#endif /* TI_DRIVERS_WIFI_CC3000 */

#if TI_DRIVERS_WIFI_CC3100
#include <ti/drivers/wifi/WiFiCC3100.h>

/* WiFi objects */
WiFiCC3100_Object wiFiCC3100Objects[EK_TM4C123GXL_WIFICOUNT];

/* WiFi configuration structure */
const WiFiCC3100_HWAttrs wiFiCC3100HWAttrs[EK_TM4C123GXL_WIFICOUNT] = {
    {
        GPIO_PORTB_BASE, /* IRQ port */
        GPIO_PIN_2,      /* IRQ pin */
        INT_GPIOB,       /* IRQ port interrupt vector */

        GPIO_PORTE_BASE, /* CS port */
        GPIO_PIN_0,      /* CS pin */

        GPIO_PORTE_BASE, /* WLAN EN port */
        GPIO_PIN_4       /* WLAN EN pin */
    }
};

const WiFi_Config WiFi_config[] = {
    {
        &WiFiCC3100_fxnTable,
        &wiFiCC3100Objects[0],
        &wiFiCC3100HWAttrs[0]
    },
    {NULL,NULL, NULL},
};

/*
 *  ======== EK_TM4C123GXL_initWiFi ========
 */
void EK_TM4C123GXL_initWiFi(void)
{
    /* Configure EN & CS pins to disable CC3100 */
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0);

    /* Configure SSI2 for CC3100 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    GPIOPinConfigure(GPIO_PB4_SSI2CLK);
    GPIOPinConfigure(GPIO_PB6_SSI2RX);
    GPIOPinConfigure(GPIO_PB7_SSI2TX);
    GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7);

    /* Configure IRQ pin */
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPD);
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_RISING_EDGE);

    SPI_init();
    EK_TM4C123GXL_initDMA();

    WiFi_init();
}
#endif /* TI_DRIVERS_WIFI_CC3100 */

#endif /* TI_DRIVERS_WIFI_INCLUDED */
