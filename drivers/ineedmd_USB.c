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

#include "app_inc/ineedmd_UI.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#define SD_DRIVE_NUM           0
#define STR_(n)             #n
#define STR(n)              STR_(n)

#define USB_MBOX_TIMEOUT_LONG   1000
#define USB_MBOX_TIMEOUT_SHORT  10

#define USB_CB_ARRAY_LIMIT      3

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

void (* vUSB_connection_callback[USB_CB_ARRAY_LIMIT]) (bool bUSB_Physical_connection, bool bUSB_Data_connection);

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
  USB_MSG_LIMIT
}eUSB_message_ID;

eUSB_Conn_State eUSB_Task_connection_state = USB_CONN_NONE;

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//tExample_struct description
typedef struct tUSB_message
{
  eUSB_message_ID eMessage;
}tUSB_message;

SDSPI_Handle sdspiHandle;

static Semaphore_Handle semUSB_task;
/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eUSB_mass_storage_setup(void); //short function declaration description
void vUSB_mass_storage_connection_callback(void);
ERROR_CODE eUSB_set_connection_state(eUSB_Conn_State eState);
ERROR_CODE eUSB_notify_callbacks(bool bUSB_Physical_connection, bool bUSB_Data_connection);
ERROR_CODE eUSB_Register_callback(void (* vUSB_conn_cb)(bool, bool));

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
  int i = 0;

  //Init the callback function array
  for(i = 0; i < USB_CB_ARRAY_LIMIT; i++)
  {
    vUSB_connection_callback[i] = NULL;
  }

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
    eUSB_MSCD_register_cb(&vUSB_mass_storage_connection_callback);
    USBMSCD_init();
  }

  eUSB_set_connection_state(USB_CONN_NONE);

  return eEC;
}

//callback funciton called when the USB MSCD connection state has changed
void vUSB_mass_storage_connection_callback(void)
{
  Semaphore_post(semUSB_task);
}

ERROR_CODE eUSB_set_connection_state(eUSB_Conn_State eState)
{
  ERROR_CODE eEC = ER_FAIL;

  eUSB_Task_connection_state = eState;

  if(eUSB_Task_connection_state != eState)
  {
    eEC = ER_FAIL;
  }
  else
  {
    eEC = ER_OK;
  }

  return eEC;
}

ERROR_CODE eUSB_notify_callbacks(bool bUSB_Physical_connection, bool bUSB_Data_connection)
{
  ERROR_CODE eEC = ER_FAIL;
  int i;

  for(i = 0; i < USB_CB_ARRAY_LIMIT; i++)
  {
    if(vUSB_connection_callback[i] != NULL)
    {
      vUSB_connection_callback[i](bUSB_Physical_connection, bUSB_Data_connection);
      eEC = ER_OK;
    }
  }

  return eEC;
}

ERROR_CODE eUSB_Register_callback(void (* vUSB_conn_cb)(bool, bool))
{
  ERROR_CODE eEC = ER_FAIL;
  int i;

  for(i = 0; i < USB_CB_ARRAY_LIMIT; i++)
  {
    if(vUSB_connection_callback[i] == NULL)
    {
      vUSB_connection_callback[i] = vUSB_conn_cb;
      eEC = ER_OK;
      break;
    }
  }

  return eEC;
}

ERROR_CODE eUSB_Unregister_callback(void (* vUSB_conn_cb)(bool, bool))
{
  ERROR_CODE eEC = ER_FAIL;
  int i;

  for(i = 0; i < USB_CB_ARRAY_LIMIT; i++)
  {
    if(vUSB_connection_callback[i] == vUSB_conn_cb)
    {
      vUSB_connection_callback[i] = NULL;
      eEC = ER_OK;
      break;
    }
  }

  return eEC;
}

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
            eEC_register_callback = eUSB_Register_callback(tRequest->vUSB_connection_callback);

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
            eUSB_Unregister_callback(tRequest->vUSB_connection_callback);

            eEC = ER_OK;
          }
          break;
        }
        case USB_REQUEST_FORCE_DISCONNECT:
          eEC = ER_NOT_ENABLED;
          break;
        case USB_REQUEST_RECONNECT:
          eEC = ER_NOT_ENABLED;
          break;
        case USB_REQUEST_FORCE_ENABLE:
          //todo: add force enable request code

          //NOTE: error code is set to OK since USB is already enabled, will need to be re-evaluated when the "todo: add force enable request code" is implemented
          eEC = ER_OK;
          break;
        case USB_REQUEST_FORCE_DISABLE:
          //todo: add force disable request code
          eEC = ER_NOT_ENABLED;
          break;
        default:
          eEC = ER_REQUEST;
          break;
      }
    }
  }

  return eEC;
}

eUSB_Conn_State eUSB_get_connection_task_state(void)
{
  return eUSB_Task_connection_state;
}

/******************************************************************************
* name: vUSB_task
* description: USB task function
* param description:
* return value description: none
******************************************************************************/
void vUSB_task(UArg arg0, UArg arg1)
{
  Error_Block eb;
  ERROR_CODE eEC = ER_FAIL;
  eUSB_Conn_State eConn_State = USB_CONN_NONE;
  tUSB_message tMsg;
  bool bDid_sem_pend = false;
  Semaphore_Params semParams;
  USBMSD_USBState ePrevious_MSD_State;
  USBMSD_USBState eMSD_State;
  tUI_request tUI_req;

  ePrevious_MSD_State = eUSB_MassStorage_state();

  Error_init(&eb);
  Semaphore_Params_init(&semParams);
  semParams.mode = Semaphore_Mode_BINARY;

  semUSB_task = Semaphore_create(0, &semParams, &eb);
  if (semUSB_task == NULL) {
      System_abort("Can't create USB task semaphore");
  }

  eEC = eUSB_mass_storage_setup();
  if(eEC == ER_FAIL)
  {
    vDEBUG("vUSB_task SYS HALT, USB setup failure!");
    while(1){};
  }else{/*do nothing*/}

  while(1)
  {
    bDid_sem_pend = Semaphore_pend(semUSB_task, OSAL_SEM_WAIT_TIMEOUT_WAITFOREVER);
    if(bDid_sem_pend == true)
    {
      eMSD_State = eUSB_MassStorage_state();
      if(eMSD_State != ePrevious_MSD_State)
      {
        ePrevious_MSD_State = eMSD_State;

        //get the current USB task state
        eConn_State = eUSB_get_connection_task_state();
        switch(eConn_State)
        {
          case  USB_CONN_NONE:
            if(eUSB_MSCD_check_physical_connection() == ER_CONNECTED)
            {
              eUSB_set_connection_state(USB_CONN_PHYSICAL);

              if(eUSB_MassStorage_data_connection() == ER_CONNECTED)
              {
                eUSB_set_connection_state(USB_CONN_DATA);

                //notify callbacks
                eUSB_notify_callbacks(true, true);
              }
              else
              {
                eUSB_set_connection_state(USB_CONN_PHYSICAL);
                eUSB_notify_callbacks(true, false);
              }
            }
            else
            {
              eUSB_set_connection_state(USB_CONN_NONE);
              eUSB_notify_callbacks(false, false);
            }
            break;
          case  USB_CONN_PHYSICAL:
            if(eUSB_MSCD_check_physical_connection() == ER_CONNECTED)
            {
              if(eUSB_MassStorage_data_connection() == ER_CONNECTED)
              {
                eUSB_set_connection_state(USB_CONN_DATA);

                //notify callbacks
                eUSB_notify_callbacks(true, true);
              }
              else
              {
                eUSB_set_connection_state(USB_CONN_PHYSICAL);
                //notify callbacks
                eUSB_notify_callbacks(true, false);
              }
            }
            else
            {
              eUSB_set_connection_state(USB_CONN_NONE);
              //notify callbacks
              eUSB_notify_callbacks(false, false);
            }
            break;
          case  USB_CONN_DATA:
            if(eUSB_MSCD_check_physical_connection() == ER_NOT_CONNECTED)
            {
              eUSB_set_connection_state(USB_CONN_NONE);
              //Notify callbacks
              eUSB_notify_callbacks(false, false);
            }else{/*do nothing*/}
            break;
          default:
            break;
        }

        //Perform a UI request
        //
        //get the current USB task state
        eConn_State = eUSB_get_connection_task_state();
        if(eConn_State == USB_CONN_DATA)
        {
          eIneedmd_UI_params_init(&tUI_req);
          tUI_req.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
          tUI_req.eComms_led_sequence = COMMS_LED_USB_CONNECTION_SUCESSFUL;
          eIneedmd_UI_request(&tUI_req);
        }
      }else{/*do nothing*/}

      //check the mailbox for any messages
      if(Mailbox_pend(tUSB_mailbox, &tMsg, OSAL_MBOX_WAIT_TIMEOUT_NO_WAIT) == true)
      {
        switch(tMsg.eMessage)
        {
          case USB_MSG_FORCE_DISCONNECT:
            break;
          case USB_MSG_REQUEST_RECONNECT:
            break;
          default:
            break;
        }
      }

    }else{/*do nothing*/}
  }
}

#endif //__INEEDMD_USB_C__
