/*
 * ineedmd_watchdog.h
 *
 *  Created on: Aug 1, 2014
 *      Author: BrianS
 */

#ifndef INEEDMD_WATCHDOG_H_
#define INEEDMD_WATCHDOG_H_

#include "utils_inc/error_codes.h"
ERROR_CODE ineedmd_watchdog_setup(void);  /*sets up the watchdog*/
ERROR_CODE ineedmd_watchdog_pat(void);  /*resets the watchdog timer with the "short" period */
ERROR_CODE ineedmd_watchdog_feed(void);  /*resets the watchdog timer with the "long" period*/
ERROR_CODE ineedmd_watchdog_doorbell(void);  /*makes the watchdog proc effectivly immediatly*/
ERROR_CODE ineedmd_watchdog_debug_mode(void); /*disables the watchdog reset for debugging*/



#endif /* INEEDMD_WATCHDOG_H_ */
