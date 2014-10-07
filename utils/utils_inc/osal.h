/******************************************************************************
*
* osal.h - Operating System abstraction layer include file
* Copyright (c) notice
*
******************************************************************************/
#ifndef __OSAL_H__
#define __OSAL_H__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Mailbox.h>
/******************************************************************************
*public defines ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
*public variables /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
*public enums /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/


/******************************************************************************
*public structures ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
typedef struct tOSAL_Mailbox_Handle
{
  uint32_t uiWait_time;         //OS wait time before returning
  uint16_t uiBuff_Size;         //size of each mailbox message
  void *   pvBuff;              //Pointer to the mailbox buffer, generated by the OSAL
  uint16_t uiNumber_of_items;   //number of mailbox messages
  void *   pvMailbox_Handle;    //OS mailbox handle, generated by the OSAL
}tOSAL_Mailbox_Handle;
/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eOSAL_OS_start            (void);
ERROR_CODE eOSAL_delay               (uint32_t uiDelay, uint32_t * puiMS_Delayed);
ERROR_CODE eOSAL_Mailbox_Params_Init (tOSAL_Mailbox_Handle * ptMbox_handle);
ERROR_CODE eOSAL_Mailbox_Create      (tOSAL_Mailbox_Handle * ptMbox_handle);
ERROR_CODE eOSAL_Mailbox_Get_msg     (tOSAL_Mailbox_Handle * ptMailbox_handle);
ERROR_CODE eOSAL_Queue_Get_msg       (void * pvQueue_handle);
#endif //__FILE_NAME_H__
