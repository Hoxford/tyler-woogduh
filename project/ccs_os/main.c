/*
 *  ======== main.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <xdc/std.h>

#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_ints.h>

#include <driverlib/sysctl.h>


#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/ENV.h>

#include "utils_inc/error_codes.h"
#include "utils_inc/proj_debug.h"
#include "utils_inc/osal.h"
#include "board.h"
#include "drivers_inc/ineedmd_watchdog.h"
#include "drivers_inc/ineedmd_bluetooth_radio.h"

/******************************************************************************
* name: vTask_Idle
* description: idle task for the system. Its only job is to call the watchdog clear function.
* param description:
* return value description:
******************************************************************************/
void vIdle_Task(UArg a0, UArg a1)
{
  ERROR_CODE eEC = ER_FAIL;
  uint32_t uiClock_time_start = 0;
  uint32_t uiClock_time_current = 0;
  uint32_t uiClock_time_diff = 0;

  vDEBUG("enter vTask_Idle()");

  uiClock_time_start = Clock_getTicks();

  eEC = ineedmd_watchdog_setup();
  if(eEC == ER_OK)
  {
    vDEBUG("ineedmd_watchdog_setup()");
  }

  while(1)
  {
    Task_sleep(1);
    uiClock_time_current = Clock_getTicks();
    uiClock_time_diff = (uiClock_time_current - uiClock_time_start);
    if(uiClock_time_diff >= 60000)
    {
      vDEBUG("I'm alive and running!");
      uiClock_time_start = Clock_getTicks();
    }
    ineedmd_watchdog_pat();
    GPIO_toggle(EK_TM4C123GXL_DEBUG);
  }
}

/*
 *  ======== main ========
 */
int main(void)
{
  //Perform the basic board init functions
  //
  eBSP_Board_init();

  vDEBUG_init();
  vDEBUG("Hello World!");

  eOSAL_OS_start();/* does not return */

  return(0);
}
