/******************************************************************************
*
* ineedmd_USB.h - USB include file
* Copyright (c) notice
*
******************************************************************************/
#ifndef __INEEDMD_USB_H__
#define __INEEDMD_USB_H__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

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
//eUSB_request_ID enum that will be part of the USB request. ID's denote specific usb request to perform
typedef enum eUSB_request_ID
{
  USB_REQUEST_NONE,
  USB_REQUEST_REGISTER_CONN_CALLBACK,
  USB_REQUEST_UNREGISTER_CONN_CALLBACK,
  USB_REQUEST_FORCE_DISCONNECT,
  USB_REQUEST_RECONNECT,
  USB_REQUEST_LIMIT
}eUSB_request_ID;

/******************************************************************************
*public structures ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//tUSB_req USB request structure
typedef struct tUSB_req
{
  eUSB_request_ID eRequest;
  void (* vUSB_connection_callback) (bool bUSB_Physical_connection, bool bUSB_Data_connection);
}tUSB_req;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eUSB_request_params_init (tUSB_req * tParams );
ERROR_CODE eUSB_request             (tUSB_req * tRequest);
#endif //__INEEDMD_USB_H__
