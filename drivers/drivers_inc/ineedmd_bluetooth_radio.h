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
//eExample_enum description
typedef enum eINMD_tx_rx_priority
{
  TXRX_PRIORITY_BLOCKING,  //will send over interface w/o task processing
  TXRX_PRIORITY_IMMEDIATE, //will post frame to front of task queue
  TXRX_PRIORITY_QUEUE      //will post frame to back of queue
}eINMD_tx_rx_priority;

typedef enum eRadio_Settings
{
  RADIO_SETTINGS_RFD,
  RADIO_SETTINGS_BAUD_115K,
  RADIO_SETTINGS_BAUD_1382K,
  RADIO_SETTINGS_APPLICATION_DEFAULT
}eRadio_Settings;

/******************************************************************************
*public structures ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//tExample_struct description
typedef struct
{
    void * vBuff;
    uint32_t uiBuff_size; //in bytes
    eINMD_tx_rx_priority ePriority;
}tINMD_tx_rx_frame;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE  eIneedMD_radio_task_init(void); //Function to call to initalize the radio driver task
void        ineedmd_radio_power     (bool);
void        ineedmd_radio_reset     (void);
//void ineedmd_radio_soft_reset(void);
ERROR_CODE  eIneedmd_radio_rfd(void);
void        ineedmd_radio_send_string(char *send_string, uint16_t uiBuff_size);
int         ineedmd_radio_send_frame(uint8_t *send_frame, uint16_t uiFrame_size);
//int         iIneedmd_radio_rcv_string(char *cRcv_string, uint16_t uiBuff_size);
int         iIneedmd_radio_rcv_byte(uint8_t *uiRcv_byte);
int         iIneedmd_radio_rcv_frame(uint8_t *uiRcv_frame, uint16_t uiBuff_size);
ERROR_CODE  eIneedmd_radio_int_rcv_frame(uint8_t *uiRcv_frame, uint16_t uiBuff_size, uint32_t * uiBytes_rcvd);
//ERROR_CODE  eIneedMD_radio_setup(void);
//void        vIneedMD_radio_read_cb(UART_Handle sHandle, void *buf, int count);
//void        vIneedMD_radio_write_cb(UART_Handle sHandle, void *buf, int count);
void        vIneedMD_radio_read_cb(RADIO_INTERFACE_RXCB_PARAMS);
void        vIneedMD_radio_write_cb(RADIO_INTERFACE_TXCB_PARAMS);
//ERROR_CODE  eIneedMD_radio_check_for_connection(void);
ERROR_CODE  eIneedmd_radio_txrx_params_init (tINMD_tx_rx_frame * tParams);
ERROR_CODE  eIneedmd_radio_tx_que_frame     (tINMD_tx_rx_frame * tParams);
ERROR_CODE  eIneedmd_radio_change_settings  (eRadio_Settings eRdo_set_request);
#endif /* INEEDMD_BLUETOOTH_RADIO_H_ */
