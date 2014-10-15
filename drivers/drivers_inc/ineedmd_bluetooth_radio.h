/******************************************************************************
*
* ineedmd_bluetooth_radio.h - Bluetooth radio interface include file
* Copyright (c) notice
*
*  Created on: Jul 7, 2014
*  Author: BrianS
*
******************************************************************************/
#ifndef __INEEDMD_BLUETOOTH_RADIO_H__
#define __INEEDMD_BLUETOOTH_RADIO_H__

/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
*public defines ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#define TI_RTOS_UART_CB_PARAMS  UART_Handle sHandle, void *buf, int count
  #define RADIO_INTERFACE_CB_PARAMS   TI_RTOS_UART_CB_PARAMS
  #define RADIO_INTERFACE_TXCB_PARAMS TI_RTOS_UART_CB_PARAMS
  #define RADIO_INTERFACE_RXCB_PARAMS TI_RTOS_UART_CB_PARAMS
/******************************************************************************
*public variables /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
*public enums /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

typedef enum eRadio_request
{
  RADIO_REQUEST_NONE = 0,
  RADIO_REQUEST_CHANGE_SETTINGS,
  RADIO_REQUEST_POWER_ON,
  RADIO_REQUEST_POWER_OFF,
  RADIO_REQUEST_SEND_FRAME,
  RADIO_REQUEST_RECEIVE_FRAME,
  RADIO_REQUEST_WAIT_FOR_CONNECTION,
  RADIO_REQUEST_HALT_WAIT_FOR_CONNECTION,
  RADIO_REQUEST_BREAK_CONNECTION,
  RADIO_REQUEST_LIMIT
}eRadio_request;

typedef enum eTransmit_priority
{
  TX_PRIORITY_IMMEDIATE, //will post frame to front of task queue
  TX_PRIORITY_QUEUE      //will post frame to back of queue
}eTransmit_priority;

typedef enum eRadio_Settings
{
  RADIO_SETTINGS_NONE,
  RADIO_SETTINGS_RFD,
  RADIO_SETTINGS_BAUD_115K,
  RADIO_SETTINGS_BAUD_1382K,
  RADIO_SETTINGS_APPLICATION_DEFAULT,
  RADIO_SETTINGS_CHANGE_ERROR
}eRadio_Settings;

typedef enum eRadio_receieve_state
{
  RADIO_RECEIVE_NO_STATE,
  RADIO_RECEIVE_BUFF_FULL,
  RADIO_RECEIVE_REQUEST_FAIL,
  RADIO_RECEIVE_REQUEST_CANCELLED
}eRadio_receive_state;

typedef enum eRadio_connection_state
{
  RADIO_CONN_UNKNOWN,
  RADIO_CONN_NONE,
  RADIO_CONN_WAITING_FOR_CONNECTION,
  RADIO_CONN_VERIFYING_CONNECTION,
  RADIO_CONN_CONNECTED,
  RADIO_CONN_HALT,
  RADIO_CONN_ERROR
}eRadio_connection_state;

//Radio setup ready
typedef enum eRadio_setup_state
{
    RDIO_SETUP_NOT_STARTED = 0,
    RDIO_SETUP_ENABLE_INTERFACE,
    RDIO_SETUP_INTERFACE_ENABLED,
    RDIO_SETUP_START_POWER,
    RDIO_SETUP_FINISH_POWER,
    RDIO_SETUP_START_SOFT_RESET,
    RDIO_SETUP_FINISH_SOFT_RESET,
    RDIO_SETUP_START_RFD,
    RDIO_SETUP_FINISH_RFD,
    RDIO_SETUP_READY,
    RDIO_SETUP_RADIO_READY_POWERED_OFF,
    RDIO_SETUP_ERROR,
    RDIO_SETUP_UNKNOWN,
    RDIO_SETUP_LIMIT
}eRadio_setup_state;

/******************************************************************************
*public structures ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//tExample_struct description
typedef struct tRadio_request
{
  uint8_t uiBuff[256];  //pointer to the TX or RX buffer
  uint32_t uiBuff_size; //size of the buffer in bytes
  uint32_t uiTimeout;  //timeout for receive and wait for connection in milliseconds
  eRadio_request eRequest; //radio request type
  eRadio_Settings eSetting; //radio setting change
  eTransmit_priority eTX_Priority; //transmit priority
  void (* vBuff_sent_callback)         (uint32_t uiBytes_sent);
  void (* vBuff_receive_callback)      (eRadio_receive_state eRcv_state);
  void (* vChange_setting_callback)    (eRadio_Settings eSetting);
  void (* vConnection_status_callback) (eRadio_connection_state eRadio_conn);
  void (* vSetup_state_callback)       (eRadio_setup_state eState);
}tRadio_request;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//ERROR_CODE  eIneedMD_radio_task_init(void); //Function to call to initalize the radio driver task
//void        ineedmd_radio_power     (bool);
//void        ineedmd_radio_reset     (void);
//void ineedmd_radio_soft_reset(void);
//ERROR_CODE  eIneedmd_radio_rfd(void);
//void        ineedmd_radio_send_string(char *send_string, uint16_t uiBuff_size);
//int         ineedmd_radio_send_frame(uint8_t *send_frame, uint16_t uiFrame_size);
//int         iIneedmd_radio_rcv_string(char *cRcv_string, uint16_t uiBuff_size);
//int         iIneedmd_radio_rcv_byte(uint8_t *uiRcv_byte);
//int         iIneedmd_radio_rcv_frame(uint8_t *uiRcv_frame, uint16_t uiBuff_size);
//ERROR_CODE  eIneedmd_radio_int_rcv_frame(uint8_t *uiRcv_frame, uint16_t uiBuff_size, uint32_t * uiBytes_rcvd);
//ERROR_CODE  eIneedMD_radio_setup(void);
//void        vIneedMD_radio_read_cb(UART_Handle sHandle, void *buf, int count);
//void        vIneedMD_radio_write_cb(UART_Handle sHandle, void *buf, int count);
eRadio_connection_state eGet_connection_state(void);
eRadio_setup_state      eGet_radio_setup_state(void);
ERROR_CODE              eSet_radio_setup_state(eRadio_setup_state eState);
void        vIneedMD_radio_read_cb(RADIO_INTERFACE_RXCB_PARAMS);
void        vIneedMD_radio_write_cb(RADIO_INTERFACE_TXCB_PARAMS);
//ERROR_CODE  eIneedMD_radio_check_for_connection(void);
ERROR_CODE  eIneedmd_radio_request_params_init (tRadio_request * tParams);
ERROR_CODE  eIneedmd_radio_request     (tRadio_request * tParams);
ERROR_CODE  eIneedmd_radio_change_settings_params_init  (eRadio_Settings eRdo_set_request);
ERROR_CODE  eIneedmd_radio_change_settings  (eRadio_Settings eRdo_set_request);

#endif /* INEEDMD_BLUETOOTH_RADIO_H_ */
