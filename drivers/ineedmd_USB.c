/******************************************************************************
*
* ineedmd_USB.c - USB driver code for wireless EKG device
* Copyright (c) notice
*
******************************************************************************/
#ifndef __INEEDMD_USB_C__
#define __INEEDMD_USB_C__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"

#include "utils_inc/error_codes.h"
#include "drivers_inc/ineedmd_USB.h"
#include "utils_inc/proj_debug.h"
#include "utils_inc/osal.h"

/* usblib Header files */
#include <usblib/usb-ids.h>
#include <usblib/usblib.h>
#include <usblib/usbmsc.h>
#include <usblib/device/usbdevice.h>
#include <usblib/device/usbdmsc.h>

/* Example/Board Header files */
#include "USBMSCD.h"
#include "board.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#define SD_DRIVE_NUM           0
#define STR_(n)             #n
#define STR(n)              STR_(n)
/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

const char  inputfile[] = "fat:"STR(SD_DRIVE_NUM)":log.txt";

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
extern Mailbox_Handle tUSB_mailbox;
/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//eExample_enum description
typedef enum eUSB_message_ID
{
  USB_MSG_NONE,
  USB_MSG_FORCE_DISCONNECT,
  USB_MSG_REQUEST_RECONNECT,
  USB_MSG_CONNECTED,
  USB_MSG_DISCONNECTED,
  USB_MSG_LIMIT
}eUSB_message_ID;

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//tExample_struct description
typedef struct tUSB_message
{
  eUSB_message_ID eMessage;
}tUSB_message;

SDSPI_Handle sdspiHandle;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eUSB_mass_storage_setup(void); //short function declaration description
void vUSB_mass_storage_connect_callback(void);
void vUSB_mass_storage_disconnect_callback(void);

/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
/******************************************************************************
* name: eUSB_mass_storage_setup
* description:
* param description: type - value: value description (in order from left to right)
*                    bool - true: do action when set to true
* return value description: ERROR_CODE - ER_OK: setup complete
*                                      - ER_FAIL: SD card interface
*
******************************************************************************/
ERROR_CODE eUSB_mass_storage_setup(void)
{
  ERROR_CODE eEC = ER_OK;
  SDSPI_Handle sdspiHandle;
  SDSPI_Params sdspiParams;

  //init SD card IO
  /* Mount and register the USB Drive */
  SDSPI_Params_init(&sdspiParams);

  sdspiHandle = SDSPI_open(Board_SDSPI0, SD_DRIVE_NUM, &sdspiParams);
  if (sdspiHandle == NULL)
  {
    eEC = ER_FAIL;
    vDEBUG("SD card IO open FAIL");
  }
  else
  {
    eEC = ER_OK;
    vDEBUG("SD card IO open");
  }

//  /* Mount and register the USB Drive */
//  USBMSCHFatFs_Params_init(&usbmschfatfsParams);
//  usbmschfatfsParams.servicePriority = 3;
//  usbmschfatfsHandle = USBMSCHFatFs_open(0,
//                                         0,//USB_DRIVE_NUM,
//                                        &usbmschfatfsParams);
//  if (usbmschfatfsHandle == NULL) {
//      System_abort("Error starting the USB Drive\n");
//  }
//
//  /* Need to block until a USB Drive has been enumerated */
//  bIs_usb_connected = USBMSCHFatFs_waitForConnect(usbmschfatfsHandle, 10000);
////  bIs_usb_connected = USBMSCHFatFs_waitForConnect(usbmschfatfsHandle, OSAL_SEM_WAIT_TIMEOUT_WAITFOREVER);
//  if (bIs_usb_connected == false)
//  {
//      System_abort("No USB drive present, aborting...\n");
//  }

  if(eEC == ER_OK)
  {
    USBMSCD_init();
  }

  return eEC;
}

//ERROR_CODE eUSB_MassStorage_waitForConnect(uint32_t uiTimeout)
//{
//  ERROR_CODE eEC = ER_FAIL;
//  bool bDid_sem_pend = false;
//  unsigned int key;
//
//  /* Need exclusive access to prevent a race condition */
//  key = GateMutex_enter(gateUSBWait);
//
//  if (state == USBMSD_STATE_UNCONFIGURED)
//  {
//    bDid_sem_pend = Semaphore_pend(semUSBConnected, uiTimeout);
////    if (!Semaphore_pend(semUSBConnected, uiTimeout))
//    if(bDid_sem_pend == false)
//    {
////      ret = false;
//      eEC = ER_FAIL;
//    }
//  }
//
//  GateMutex_leave(gateUSBWait, key);
//
//  return eEC;
//}

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

ERROR_CODE eUSB_request_params_init(tUSB_req * tParams)
{
  ERROR_CODE eEC = ER_FAIL;

  return eEC;
}

ERROR_CODE eUSB_request(tUSB_req * tRequest)
{
  ERROR_CODE eEC = ER_FAIL;
  ERROR_CODE eEC_register_callback = ER_FAIL;

  if(tRequest == NULL)
  {
    eEC = ER_PARAM;
  }
  else
  {
    if(tRequest->eRequest >= USB_REQUEST_LIMIT)
    {
      eEC = ER_REQUEST;
    }
    else
    {
      switch(tRequest->eRequest)
      {
        case USB_REQUEST_REGISTER_CONN_CALLBACK:
        {
          if(tRequest->vUSB_connection_callback == NULL)
          {
            eEC = ER_PARAM1;
          }
          else
          {
            //Set callback into callback array
            //

            if(eEC_register_callback == ER_FAIL)
            {
              eEC = ER_FULL;
            }
            else
            {
              eEC = ER_OK;
            }
          }
          break;
        }
        case USB_REQUEST_UNREGISTER_CONN_CALLBACK:
        {
          if(tRequest->vUSB_connection_callback == NULL)
          {
            eEC = ER_PARAM1;
          }
          else
          {
            //Clear callback in callback array
            //

            if(eEC_register_callback == ER_NOT_SET)
            {
              eEC = ER_NOT_SET;
            }
            else
            {
              eEC = ER_OK;
            }
          }
          break;
        }
        case USB_REQUEST_FORCE_DISCONNECT:
          break;
        case USB_REQUEST_RECONNECT:
          break;
        default:
          eEC = ER_REQUEST;
          break;
      }
    }
  }

  return eEC;
}

/******************************************************************************
* name: vUSB_task
* description: USB task function
* param description:
* return value description: none
******************************************************************************/
void vUSB_task(UArg arg0, UArg arg1)
{
  ERROR_CODE eEC = ER_FAIL;
  tUSB_message tMsg;

  eEC = eUSB_mass_storage_setup();
  if(eEC == ER_FAIL)
  {
    vDEBUG("vUSB_task SYS HALT, USB setup failure!");
    while(1){};
  }else{/*do nothing*/}

  while(1)
  {
    if(Mailbox_pend(tUSB_mailbox, &tMsg, BIOS_WAIT_FOREVER) == true)
    {
      switch(tMsg.eMessage)
      {
        case USB_MSG_FORCE_DISCONNECT:
          break;
        case USB_MSG_REQUEST_RECONNECT:
          break;
        case USB_MSG_CONNECTED:
          break;
        case USB_MSG_DISCONNECTED:
          break;
        default:
          break;
      }


      if(eUSB_MassStorage_waitForConnect() == ER_OK)
      {
        //do task shutdown for mass storage device procedure
        eEC = ER_OK;

      }

      if(eUSB_MassStorage_waitForDisonnect() == ER_OK)
      {
        //do task startup
        eEC = ER_OK;
      }
    }
  }
}

#endif //__INEEDMD_USB_C__
