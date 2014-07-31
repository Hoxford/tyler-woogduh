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
#define  DEBUG_SYSCTL_PERIPH_UART  SYSCTL_PERIPH_UART5
#define  DEBUG_UART                UART5_BASE
#define  DEBUG_UART_PIN_PORT       GPIO_PORTE_BASE
#define  DEBUG_TX_PIN_MUX_MODE     GPIO_PE5_U5TX
#define  DEBUG_RX_PIN_MUX_MODE     GPIO_PE4_U5RX
#define  DEBUG_TX_PIN              GPIO_PIN_5
#define  DEBUG_RX_PIN              GPIO_PIN_4

#define  DEBUG_BAUD                115200
#define  DEBUG_UART_CONFIG         (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)

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
  return;
}

//*****************************************************************************
// name: vDEBUG_init
// description: initalizes the debug message interface
// param description: none
// return value description: none
//*****************************************************************************
void vDEBUG_init(void)
{
  MAP_SysCtlPeripheralEnable(DEBUG_SYSCTL_PERIPH_UART);

  // Enable pin PF0 for UART1 U1RTS
  // First open the lock and select the bits we want to modify in the GPIO commit register.
  //
//  HWREG(INEEDMD_RADIO_SERIAL_PORT + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//  HWREG(INEEDMD_RADIO_SERIAL_PORT + GPIO_O_CR) = 0x1;
  //
  // Now modify the configuration of the pins that we unlocked.
  //
//  MAP_GPIOPinConfigure(GPIO_PF0_U1RTS);
//  MAP_GPIOPinTypeUART(INEEDMD_RADIO_SERIAL_PORT, GPIO_PIN_0);
  //
  // Enable pin PF1 for UART1 U1CTS
  //
//  MAP_GPIOPinConfigure(GPIO_PF1_U1CTS);
//  MAP_GPIOPinTypeUART(INEEDMD_RADIO_SERIAL_PORT, GPIO_PIN_1);
  //
  // Enable pin PE4 for UART5 U5TX and PE4 for U5RX
  //
  MAP_GPIOPinConfigure(DEBUG_TX_PIN_MUX_MODE);
  MAP_GPIOPinConfigure(DEBUG_RX_PIN_MUX_MODE);
  //
  // Enable pin PE5 for UART5 U5TX
  //
  MAP_GPIOPinTypeUART(DEBUG_UART_PIN_PORT, DEBUG_TX_PIN);
  //
  // Enable pin PE4 for UART5 U5RX
  //
  MAP_GPIOPinTypeUART(DEBUG_UART_PIN_PORT, DEBUG_RX_PIN);

  //
  // Configure the UART for 115,200, 8-N-1 operation.
  // This function uses SysCtlClockGet() to get the system clock
  // frequency.
  UARTConfigSetExpClk(DEBUG_UART, MAP_SysCtlClockGet(), DEBUG_BAUD, DEBUG_UART_CONFIG);

  //And the Radio UART
  UARTEnable(DEBUG_UART);
  return;
}
#endif //__PROJ_DEBUG_C__
#endif //DEBUG
