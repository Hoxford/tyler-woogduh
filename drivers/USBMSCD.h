/*
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== USBMSCD.h ========
 */

#ifndef USBMSCD_H_
#define USBMSCD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <xdc/std.h>

typedef volatile enum USBMSD_USBstate
{
  USBMSD_STATE_NONE,
  USBMSD_STATE_UNCONFIGURED,
  USBMSD_STATE_INIT,
  USBMSD_STATE_IDLE,
  USBMSD_STATE_LIMIT
} USBMSD_USBState;

/*!
 *  ======== USBMSCD_init ========
 *  Function to initialize the USB mass storage class reference module.
 *
 *  Note: This function is not reentrant safe.
 */
ERROR_CODE eUSB_MassStorage_data_connection(void);
USBMSD_USBState  eUSB_MassStorage_state(void);
ERROR_CODE eUSB_MSCD_register_cb(void (* vUSB_MSCD_change_callback)(void));
ERROR_CODE eUSB_MSCD_check_physical_connection(void);
extern void USBMSCD_init(void);

#ifdef __cplusplus
}
#endif

#endif /* USBMSCD_H_ */