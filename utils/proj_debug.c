//*****************************************************************************
//
// proj_debug.c - project wide debug api
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef DEBUG

#else
#ifndef __PROJ_DEBUG_C__
#define __PROJ_DEBUG_C__
//*****************************************************************************
// includes
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "utils_inc/proj_debug.h"

//*****************************************************************************
// defines
//*****************************************************************************
// Debug uart mappings
//
#define  DEBUG_SYSCTL_PERIPH_GPIO  SYSCTL_PERIPH_GPIOE
#define  DEBUG_SYSCTL_PERIPH_UART  SYSCTL_PERIPH_UART5
#define  DEBUG_UART                UART5_BASE
#define  DEBUG_UART_PIN_PORT       GPIO_PORTE_BASE
#define  DEBUG_TX_PIN_MUX_MODE     GPIO_PE5_U5TX
#define  DEBUG_RX_PIN_MUX_MODE     GPIO_PE4_U5RX
#define  DEBUG_TX_PIN              GPIO_PIN_5
#define  DEBUG_RX_PIN              GPIO_PIN_4

#define  DEBUG_BAUD                115200
#define  DEBUG_UART_CONFIG         (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)

//Debug pin 1 mappings
//
#define DEBUG_GPIO_PORT_1  GPIO_PORTD_BASE
#define DEBUG_GPIO_PIN_1   GPIO_PIN_1
#define DEBUG_GPIO_PIN_SET_1   GPIO_PIN_1
#define DEBUG_GPIO_PIN_CLR_1   ~GPIO_PIN_1

//*****************************************************************************
// variables
//*****************************************************************************

//*****************************************************************************
// external variables
//*****************************************************************************

//*****************************************************************************
// enums
//*****************************************************************************

//*****************************************************************************
// structures
//*****************************************************************************

//*****************************************************************************
// external functions
//*****************************************************************************

//*****************************************************************************
// function declarations
//*****************************************************************************

//*****************************************************************************
// functions
//*****************************************************************************

void __error__(char *pcFilename, uint32_t ui32Line)
{
  return;
}
//*****************************************************************************
// name: vUSB_driverlib_out
// description: called when an invalid argument is passed to the USB API
// param description:
// return value description: none
//*****************************************************************************
void vUSB_driverlib_out(char *pcFilename, uint32_t ui32Line)
{
  return;
  //todo add a debug out feature for USB
}

//*****************************************************************************
// name: vDEBUG
// description: sends a message out the debug message interface
// param description: none
// return value description: none
//*****************************************************************************
void vDEBUG(char * cMsg, ...)
{

  uint32_t i, uiMsg_len;
  uiMsg_len = strlen(cMsg);

  for (i = 0; i<uiMsg_len; i++)
  {
    UARTCharPut(DEBUG_UART, cMsg[i]);
  }

  UARTCharPut(DEBUG_UART, '\r');
  UARTCharPut(DEBUG_UART, '\n');

  return;
}

/******************************************************************************
* name: vDEBUG_GPIO_SET_1
* description: sets high DEBUG_GPIO_PIN_1
* param description:
* return value description:
******************************************************************************/
void vDEBUG_GPIO_SET_1(void)
{
  MAP_GPIOPinWrite(DEBUG_GPIO_PORT_1, DEBUG_GPIO_PIN_1, DEBUG_GPIO_PIN_SET_1);
  return;
}

/******************************************************************************
* name: vDEBUG_GPIO_CLR_1
* description: sets low DEBUG_GPIO_PIN_1
* param description:
* return value description:
******************************************************************************/
void vDEBUG_GPIO_CLR_1(void)
{
  MAP_GPIOPinWrite(DEBUG_GPIO_PORT_1, DEBUG_GPIO_PIN_1, DEBUG_GPIO_PIN_CLR_1);
  return;
}

/******************************************************************************
* name: vDEBUG_GPIO_TOGGLE_1
* description: toggles DEBUG_GPIO_PIN_1
* param description:
* return value description:
******************************************************************************/
void vDEBUG_GPIO_TOGGLE_1(void)
{
  if(MAP_GPIOPinRead(DEBUG_GPIO_PORT_1, DEBUG_GPIO_PIN_1) == DEBUG_GPIO_PIN_SET_1)
    vDEBUG_GPIO_CLR_1();
  else
    vDEBUG_GPIO_SET_1();

  return;
}

/******************************************************************************
* name: vDEBUG_init
* description: initalizes the debug message interface and the debug gpio pins
* param description: none
* return value description: none
******************************************************************************/
void vDEBUG_init(void)
{

  //Enable the peripheral gpio, all must be enabled and in order
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  //Init debug gpio pin/pins
  //
  MAP_GPIOPinTypeGPIOOutput(DEBUG_GPIO_PORT_1, DEBUG_GPIO_PIN_1);
  MAP_GPIOPinWrite(DEBUG_GPIO_PORT_1, DEBUG_GPIO_PIN_1, DEBUG_GPIO_PIN_CLR_1);

  //Enable the debug uart
  //
  MAP_SysCtlPeripheralEnable(DEBUG_SYSCTL_PERIPH_UART);

  // Configure the debug port pins
  //
  MAP_GPIOPinConfigure(DEBUG_TX_PIN_MUX_MODE);
  MAP_GPIOPinConfigure(DEBUG_RX_PIN_MUX_MODE);

  // Set the pin types
  //
  MAP_GPIOPinTypeUART(DEBUG_UART_PIN_PORT, DEBUG_TX_PIN);
  MAP_GPIOPinTypeUART(DEBUG_UART_PIN_PORT, DEBUG_RX_PIN);

  //Set the clock source for the uart
  //
  UARTClockSourceSet(DEBUG_UART, UART_CLOCK_PIOSC);

  //Configure the uart
  //
  UARTConfigSetExpClk(DEBUG_UART, 16000000, DEBUG_BAUD, DEBUG_UART_CONFIG);

  //Enable the debug UART
  //
  UARTEnable(DEBUG_UART);
  return;
}
#endif //__PROJ_DEBUG_C__
#endif //DEBUG
