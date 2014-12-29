/*
 * ineedmd_watchdog.c
 *
 *  Created on: Aug 1, 2014
 *      Author: BrianS
 */
#ifndef __INEEDMD_WATCHDOG_C__
#define __INEEDMD_WATCHDOG_C__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/drivers/Watchdog.h>

#include "driverlib/rom.h"

#include "utils_inc/error_codes.h"
#include "utils_inc/proj_debug.h"
#include "drivers_inc/ineedmd_watchdog.h"

#include "board.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#define WD_BASE WATCHDOG0_BASE
#define WD_PHERF SYSCTL_PERIPH_WDOG0
#define WD_BARK 0x00    // instant reset
#define WD_SMALL_PAT  0x0FFFFFFF    // about 30 sec at 80Mhz
#define WD_PAT  0x8FFFFFFF    // about 30 sec at 80Mhz
#define WD_BIG_PAT 0xFFFFFFFF // about 3 min 30 sec at 80Mhz

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
bool flag = false;
/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
Watchdog_Handle     handle;
Watchdog_Params     params;
/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/*
 *  ======== callback ========
 *  Watchdog interrupt callback function. It toggles and LED, and if a button
 *  has not been pressed, clears the watchdog interrupt flag.
 */
void vINMD_watchdog_callback(UArg handle)
{
  ERROR_CODE eEC;

  eEC = eBSP_debugger_detect();
  if(eEC == ER_OK)
  {
    System_printf("****!!!!SYS HALT!!!!****\r\n");
    System_printf("Watchdog Bark!\r\n");
    System_flush();
    vDEBUG_ASSERT("Watchdog Bark!",0);
  }
  else
  {
    vDEBUG("Watchdog Bark!");
    vDEBUG("****!!!!SYS RESET!!!!****\r\n");
    ROM_IntMasterDisable();
    ROM_SysCtlReset();
  }
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
ERROR_CODE ineedmd_watchdog_setup(void)
{
#ifdef USING_TIRTOS
  ERROR_CODE eEC = ER_FAIL;

  Watchdog_Params_init(&params);
  params.resetMode = Watchdog_RESET_OFF;
#ifdef DEBUG
  //For debugging Watchdog counter will stop while stepping through code and reset is disabled
  //
  ineedmd_watchdog_debug_mode();

#endif
  params.callbackFxn = vINMD_watchdog_callback;
  handle = Watchdog_open(Board_WATCHDOG0, &params);
  if (!handle)
  {
    System_printf("Watchdog did not open");
    eEC = ER_FAIL;
  }
  else
  {
    Watchdog_setReload(handle, WD_BIG_PAT);
    eEC = ER_OK;
  }

  return eEC;
#else
  ERROR_CODE eEC = ER_OK;
  uint32_t uiWD_shake = 0;
  MAP_SysCtlPeripheralEnable(WD_PHERF);
  MAP_SysCtlPeripheralReset(WD_PHERF);

  eMaster_int_enable();

  //Unlock the watchdog
  //
  if(MAP_WatchdogLockState(WD_BASE) == true)
  {
    MAP_WatchdogUnlock(WD_BASE);
  }

  //Enable the Watchdog timer interrupt
  //
  MAP_WatchdogIntEnable(WD_BASE);
  MAP_IntEnable(INT_WATCHDOG);
  MAP_WatchdogIntClear(WD_BASE);

  //Set the watchdog default timeout
  //
  MAP_WatchdogReloadSet(WD_BASE, WD_PAT);

  //Enable the watchdog to reset the system
  //
  MAP_WatchdogResetEnable(WD_BASE);

#ifdef DEBUG
  //For debugging Watchdog counter will stop while stepping through code and reset is disabled
  //
  ineedmd_watchdog_debug_mode();
//  MAP_WatchdogResetDisable(WD_BASE);
#endif

  //Finally enable the watchdog
  //
  MAP_WatchdogEnable(WD_BASE);

  //Check to make sure the watchdog is running
  //
  uiWD_shake = MAP_WatchdogValueGet(WD_BASE);
  if(uiWD_shake < WD_PAT)
  {
    eEC = ER_OK;
  }
  else
  {
    eEC = ER_FAIL;
  }
  return eEC;
#endif //#ifdef USING_TIRTOS
}

ERROR_CODE ineedmd_watchdog_pat(void)
{
#ifdef USING_TIRTOS
  ERROR_CODE eEC = ER_OK;
  Watchdog_clear((Watchdog_Handle)handle);
  return eEC;
#else
  ERROR_CODE eEC = ER_OK;
  uint32_t uiWD_shake = 0;
  MAP_WatchdogReloadSet(WD_BASE, WD_PAT);

  uiWD_shake = MAP_WatchdogValueGet(WD_BASE);

  if(uiWD_shake < WD_PAT)
  {
    eEC = ER_OK;
  }
  else
  {
    eEC = ER_FAIL;
  }

  return eEC;
#endif
}

ERROR_CODE ineedmd_watchdog_feed(void)
{
#ifdef USING_TIRTOS
  ERROR_CODE eEC = ER_OK;
  Watchdog_clear((Watchdog_Handle)handle);
  return eEC;
#else
  ERROR_CODE eEC = ER_OK;
  uint32_t uiWD_shake = 0;

  MAP_WatchdogReloadSet(WD_BASE, WD_BIG_PAT);

  if(uiWD_shake < WD_PAT)
  {
    eEC = ER_OK;
  }
  else
  {
    eEC = ER_FAIL;
  }

  return eEC;
#endif
}

ERROR_CODE ineedmd_watchdog_doorbell(void)
{
#ifdef USING_TIRTOS
  ERROR_CODE eEC = ER_OK;

  return eEC;
#else
  ERROR_CODE eEC = ER_OK;

  //Disable the stall even if it was enabled for debug
  MAP_WatchdogStallDisable(WD_BASE);

  //set the watchdog timer to zero
  MAP_WatchdogReloadSet(WD_BASE, WD_BARK);

  //release the hounds
  while(1){};

  //Should never get here
  return eEC;
#endif
}

ERROR_CODE ineedmd_watchdog_barkcollar(void)
{
  ERROR_CODE eEC = ER_FAIL;

  if(handle == NULL)
  {
    //no watchdog the shut down, set the error code
    eEC = ER_OK;
  }
  else
  {
    Watchdog_close((Watchdog_Handle)handle);
    eEC = ER_OK;
  }

  return eEC;

}

ERROR_CODE ineedmd_watchdog_debug_mode(void)
{
#ifdef USING_TIRTOS
  ERROR_CODE eEC = ER_OK;
  params.debugStallMode = Watchdog_DEBUG_STALL_ON;
  return eEC;
#else
  ERROR_CODE eEC = ER_OK;
  MAP_WatchdogStallEnable(WD_BASE);

  //todo: see if wd reg changes with the rom call and can be measured
  return eEC;
#endif
}
#endif //#define __INEEDMD_WATCHDOG_C__
