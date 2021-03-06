/*
 * ineedmd_watchdog.c
 *
 *  Created on: Aug 1, 2014
 *      Author: BrianS
 */


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <driverlib/rom.h>

//Pull the processor
#include <inc/tm4c1233h6pm.h>
#include "driverlib/rom_map.h"

//#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/watchdog.h"

#include "error_codes.h"
#include "ineedmd_watchdog.h"

#define WD_BASE WATCHDOG0_BASE
#define WD_PHERF SYSCTL_PERIPH_WDOG0
#define WD_BARK 0x00    // instant reset
#define WD_PAT  0x8fFFFFFF    // about 30 sec at 80Mhz
#define WD_BIG_PAT 0xFFFFFFFF // about 3 min 30 sec at 80Mhz

ERROR_CODE ineedmd_watchdog_setup(void)
{
  ERROR_CODE eEC = ER_OK;
  uint32_t uiWD_shake = 0;
  MAP_SysCtlPeripheralEnable(WD_PHERF);
  MAP_SysCtlPeripheralReset(WD_PHERF);

  if(MAP_WatchdogLockState(WD_BASE) == true)
  {
    MAP_WatchdogUnlock(WD_BASE);
  }
  MAP_WatchdogReloadSet(WD_BASE, WD_PAT);
  MAP_WatchdogResetEnable(WD_BASE);
  MAP_WatchdogEnable(WD_BASE);
  MAP_WatchdogIntClear(WD_BASE);
#ifdef DEBUG
//  ineedmd_watchdog_debug_mode();
#endif
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
}

ERROR_CODE ineedmd_watchdog_pat(void)
{
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
}

ERROR_CODE ineedmd_watchdog_feed(void)
{
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
}

ERROR_CODE ineedmd_watchdog_doorbell(void)
{
  ERROR_CODE eEC = ER_OK;

  //Disable the stall even if it was enabled for debug
  MAP_WatchdogStallDisable(WD_BASE);

  //set the watchdog timer to zero
  MAP_WatchdogReloadSet(WD_BASE, WD_BARK);

  //release the hounds
  while(1){};

  //Should never get here
  return eEC;
}

ERROR_CODE ineedmd_watchdog_debug_mode(void)
{
  ERROR_CODE eEC = ER_OK;
//  MAP_WatchdogStallEnable(WD_BASE);

  //todo: see if wd reg changes with the rom call and can be measured
  return eEC;
}
