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


#include "app_inc/ineedmd_watchdog.h"

#define WD_BASE WATCHDOG0_BASE
#define WD_PHERF SYSCTL_PERIPH_WDOG0
#define WD_BARK 0x00    // instant reset
#define WD_PAT  0x8fFFFFFF    // about 30 sec at 80Mhz
#define WD_BIG_PAT 0xFFFFFFFF // about 3 min 30 sec at 80Mhz

int
ineedmd_watchdog_setup()
{

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
  ineedmd_watchdog_debug_mode();
#endif
  return 1;
}

int
ineedmd_watchdog_pat(void)
{
  MAP_WatchdogReloadSet(WD_BASE, WD_PAT);
  return 1;
}

int
ineedmd_watchdog_feed(void)
{
  MAP_WatchdogReloadSet(WD_BASE, WD_BIG_PAT);
  return 1;
}

int
ineedmd_watchdog_doorbell(void)
{
  MAP_WatchdogReloadSet(WD_BASE, WD_BARK);
  return 1;
}

int
ineedmd_watchdog_debug_mode(void)
{
  MAP_WatchdogStallEnable(WD_BASE);
  return 1;
}
