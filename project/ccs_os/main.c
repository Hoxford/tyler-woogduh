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

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>

#include <ti/drivers/ENV.h>

#include "utils_inc/error_codes.h"
#include "utils_inc/proj_debug.h"
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
  vDEBUG("enter vTask_Idle()");

  while(1)
  {
    ineedmd_watchdog_pat();
    GPIO_toggle(EK_TM4C123GXL_DEBUG);
  }
}

/*
 *  ======== main ========
 */
int main(void)
{
  ERROR_CODE eEC = ER_OK;

  Error_Block eb;

  vDEBUG("Hello World!");

  //Perform the basic board init functions
  //
  eEC = eBSP_Board_init();
  if(eEC == ER_OK)
  {
      vDEBUG("eBSP_Board_init() done");
  }

  //Call driver init functions
  //
  //Watchdog
  eEC = ineedmd_watchdog_setup();
  if(eEC == ER_OK)
  {
    vDEBUG("ineedmd_watchdog_setup()");
  }

  //Wireless radio
  eEC = eIneedMD_radio_setup();
  if(eEC == ER_OK)
  {
    vDEBUG("eIneedMD_radio_setup()");
  }

  Error_init(&eb);

  BIOS_start();    /* does not return */
  return(0);
}
