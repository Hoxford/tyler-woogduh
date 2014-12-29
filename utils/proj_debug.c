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
#include <stdlib.h>

#include "inc/hw_memmap.h"
//#include "EK_TM4C123GXL.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include "utils_inc/proj_debug.h"
#include "utils_inc/error_codes.h"
#include "utils_inc/osal.h"

#include "board.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

#define SYS_HALT_MSG  "****!!!!SYS HALT!!!!****\r\n"
#define SYS_ASSERT_RST_MSG  "****!!!!SYS ASSERT RESET!!!!****\r\n"

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
 void vDEBUG_CALLBACK(UART_Handle sHandle, void *buf, size_t  count);
/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
void vDEBUG_CALLBACK(UART_Handle sHandle, void *buf, size_t  count)
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
  ERROR_CODE eEC_Is_OS_Running = ER_FALSE;
  uint16_t uiLen = 0;
  uiLen = strlen(cMsg);

  eEC_Is_OS_Running = eOSAL_Is_OS_Running();

  if(eEC_Is_OS_Running == ER_FALSE)
  {
    UART_writePolling(sDebug_UART_handle, cMsg, uiLen);
    UART_writePolling(sDebug_UART_handle, "\r\n", strlen("\r\n"));
  }
  else
  {
    UART_write(sDebug_UART_handle, cMsg, uiLen);
    UART_write(sDebug_UART_handle, "\r\n", strlen("\r\n"));
  }
  return;
}

void vDEBUG_ASSERT(char * cMsg,int iAssert)
{
  ERROR_CODE eEC;
  uint16_t uiLen = 0;
  uiLen = 0;

  if(iAssert)
  {
    return;
  }
  else
  {
    eEC = eBSP_debugger_detect();
    if(eEC == ER_OK)
    {
      uiLen = strlen(cMsg);
      UART_writePolling(sDebug_UART_handle, SYS_HALT_MSG, strlen(SYS_HALT_MSG));
      UART_writePolling(sDebug_UART_handle, cMsg, uiLen);
      UART_writePolling(sDebug_UART_handle, "\r\n", strlen("\r\n"));

      System_printf(SYS_HALT_MSG);
      System_flush();
      System_abort(cMsg);

      ROM_IntMasterDisable();
      ROM_SysTickIntDisable();
      ROM_SysTickDisable();
      while(1){};
    }
    else
    {
      uiLen = strlen(cMsg);
      UART_writePolling(sDebug_UART_handle, SYS_ASSERT_RST_MSG, strlen(SYS_ASSERT_RST_MSG));
      UART_writePolling(sDebug_UART_handle, cMsg, uiLen);
      UART_writePolling(sDebug_UART_handle, "\r\n", strlen("\r\n"));

      ROM_IntMasterDisable();
      ROM_SysCtlReset();
    }
  }
}

/******************************************************************************
* name: vDEBUG_GPIO_SET_1
* description: sets high DEBUG_GPIO_PIN_1
* param description:
* return value description:
******************************************************************************/
void vDEBUG_GPIO_SET_1(void)
{
  GPIO_write(EK_TM4C123GXL_DEBUG, DEBUG_GPIO_PIN_SET_1);
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
  GPIO_write(EK_TM4C123GXL_DEBUG, DEBUG_GPIO_PIN_CLR_1);
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
  ERROR_CODE eEC;

  sDebug_UART_params.readMode       = UART_MODE_BLOCKING;  /*!< Mode for all read calls */
  sDebug_UART_params.writeMode      = UART_MODE_BLOCKING;  /*!< Mode for all write calls */
  sDebug_UART_params.readTimeout    = BIOS_WAIT_FOREVER;   /*!< Timeout for read semaphore */
  sDebug_UART_params.writeTimeout   = BIOS_WAIT_FOREVER;   /*!< Timeout for write semaphore */
  sDebug_UART_params.readCallback   = vDEBUG_CALLBACK;     /*!< Pointer to read callback */
  sDebug_UART_params.writeCallback  = vDEBUG_CALLBACK;     /*!< Pointer to write callback */
  sDebug_UART_params.readReturnMode = UART_RETURN_FULL;    /*!< Receive return mode */
  sDebug_UART_params.readDataMode   = UART_DATA_BINARY;    /*!< Type of data being read */
  sDebug_UART_params.writeDataMode  = UART_DATA_BINARY;    /*!< Type of data being written */
  sDebug_UART_params.readEcho       = UART_ECHO_OFF;       /*!< Echo received data back */
  sDebug_UART_params.baudRate       = 115200;              /*!< Baud rate for UART */
  sDebug_UART_params.dataLength     = UART_LEN_8;          /*!< Data length for UART */
  sDebug_UART_params.stopBits       = UART_STOP_ONE;       /*!< Stop bits for UART */
  sDebug_UART_params.parityType     = UART_PAR_NONE;       /*!< Parity bit type for UART */

  sDebug_UART_handle = UART_open(EK_TM4C123GXL_UART3, &sDebug_UART_params);

  eEC = eBSP_debugger_detect();
  if(eEC == ER_OK)
  {
    vDEBUG("Debugger connected!");
  }
  else
  {
    vDEBUG("Debugger NOT connected!");
  }

  return;
}
#endif //__PROJ_DEBUG_C__
#endif //DEBUG
