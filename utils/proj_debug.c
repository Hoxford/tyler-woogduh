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
//#include "EK_TM4C123GXL.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include "utils_inc/proj_debug.h"
#include "utils_inc/error_codes.h"

#include "board.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
bool bIs_xmit_done = false;
/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//UART parameters
UART_Handle sDebug_UART_handle;
UART_Params sDebug_UART_params;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
 void vDEBUG_CALLBACK(UART_Handle sHandle, void *buf, int count);
/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
void vDEBUG_CALLBACK(UART_Handle sHandle, void *buf, int count)
{
  bIs_xmit_done = true;
}
/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

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
  uint16_t uiLen = 0;
  uiLen = strlen(cMsg);
  UART_writePolling(sDebug_UART_handle, cMsg, uiLen);
  UART_writePolling(sDebug_UART_handle, "\r\n", strlen("\r\n"));
  //  System_printf(cMsg);
  //  System_printf("\r\n");
  //  System_flush(); /* force SysMin output to console */
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
  GPIO_toggle(EK_TM4C123GXL_DEBUG);

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
//  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
//  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
//  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
//  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//
//  //Init debug gpio pin/pins
//  //
//  MAP_GPIOPinTypeGPIOOutput(DEBUG_GPIO_PORT_1, DEBUG_GPIO_PIN_1);
//  MAP_GPIOPinWrite(DEBUG_GPIO_PORT_1, DEBUG_GPIO_PIN_1, DEBUG_GPIO_PIN_CLR_1);
//
//  //Enable the debug uart
//  //
//  MAP_SysCtlPeripheralEnable(DEBUG_SYSCTL_PERIPH_UART);
//
//  // Configure the debug port pins
//  //
//  MAP_GPIOPinConfigure(DEBUG_TX_PIN_MUX_MODE);
//  MAP_GPIOPinConfigure(DEBUG_RX_PIN_MUX_MODE);
//
//  // Set the pin types
//  //
//  MAP_GPIOPinTypeUART(DEBUG_UART_PIN_PORT, DEBUG_TX_PIN);
//  MAP_GPIOPinTypeUART(DEBUG_UART_PIN_PORT, DEBUG_RX_PIN);
//
//  //Set the clock source for the uart
//  //
//  UARTClockSourceSet(DEBUG_UART, UART_CLOCK_PIOSC);
//
//  //Configure the uart
//  //
//  UARTConfigSetExpClk(DEBUG_UART, 16000000, DEBUG_BAUD, DEBUG_UART_CONFIG);
//
//  //Enable the debug UART
//  //
//  UARTEnable(DEBUG_UART);

  sDebug_UART_params.readMode       = UART_MODE_CALLBACK;              /*!< Mode for all read calls */
  sDebug_UART_params.writeMode      = UART_MODE_CALLBACK;              /*!< Mode for all write calls */
  sDebug_UART_params.readTimeout    = 1000;                            /*!< Timeout for read semaphore */
  sDebug_UART_params.writeTimeout   = 1000;                            /*!< Timeout for write semaphore */
  sDebug_UART_params.readCallback   = vDEBUG_CALLBACK;                            /*!< Pointer to read callback */
  sDebug_UART_params.writeCallback  = vDEBUG_CALLBACK;                            /*!< Pointer to write callback */
  sDebug_UART_params.readReturnMode = UART_RETURN_NEWLINE;             /*!< Receive return mode */
  sDebug_UART_params.readDataMode   = UART_DATA_TEXT;                  /*!< Type of data being read */
  sDebug_UART_params.writeDataMode  = UART_DATA_TEXT;                  /*!< Type of data being written */
  sDebug_UART_params.readEcho       = UART_ECHO_ON;                    /*!< Echo received data back */
  sDebug_UART_params.baudRate       = 115200;                          /*!< Baud rate for UART */
  sDebug_UART_params.dataLength     = UART_LEN_8;                      /*!< Data length for UART */
  sDebug_UART_params.stopBits       = UART_STOP_ONE;                   /*!< Stop bits for UART */
  sDebug_UART_params.parityType     = UART_PAR_NONE;                   /*!< Parity bit type for UART */

  sDebug_UART_handle = UART_open(EK_TM4C123GXL_UART5, &sDebug_UART_params);

  return;
}
#endif //__PROJ_DEBUG_C__
#endif //DEBUG
