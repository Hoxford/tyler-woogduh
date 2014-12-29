//*****************************************************************************
//
// ineedmd_command_protocol.c - command protocol for ineedMD. Contains the api,
//  input, output and parsers.
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __INEEDMD_COMMAND_PROTOCOL_C__
#define __INEEDMD_COMMAND_PROTOCOL_C__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_nvic.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"

#include "utils_inc/error_codes.h"
#include "board.h"

#include "utils_inc/proj_debug.h"
#include "utils_inc/osal.h"

#include "drivers_inc/battery.h"

#include "app_inc/ineedmd_command_protocol.h"
#include "app_inc/ineedmd_waveform.h"
#include "app_inc/ineedmd_watchdog.h"
#include "app_inc/ineedmd_UI.h"
#include "drivers_inc/ineedmd_sounder.h"
#include "drivers_inc/ineedmd_bluetooth_radio.h"
#include "drivers_inc/ineedmd_adc.h"
#include "drivers_inc/ineedmd_USB.h"
#include "drivers_inc/nvram.h"

#include "app_inc/ineedmd_power_control.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//Protocol frame defines
#define PROTOCOL_FRAME_MAX_LEN          128
#define PROTOCOL_FRAME_START_BYTE       0x9C
#define PROTOCOL_FRAME_END_BYTE         0xC9
#define PROTOCOL_FRAME_ACK_LEN          4
#define PROTOCOL_FRAME_NACK_LEN         4
#define PROTOCOL_FRAME_CMND_TYPE_INDEX  1
#define PROTOCOL_FRAME_CMND_LEN_INDEX   2
#define PROTOCOL_FRAME_CMND_ID_INDEX    3

#define STATUS_0X14_LEN 23
#define STATUS_0X17_LEN 24
#define STATUS_0X15_LEN 23
#define STATUS_LEN      0x38

//protocol connection defines
#define PROTOCOL_CONN_TIMEOUT_1MIN    60000
#define PROTOCOL_CONN_TIMEOUT_FOREVER 0

#ifdef DEBUG
  #define printf        vDEBUG
  #define PrintCommand  vCMND_ECHO_FRAME
  #define debug_out     vDEBUG
#else
//  #define printf(a, __VA_ARGS__)
  #define printf(a,...)
  #define PrintCommand(a,x)
//  #define debug_out(c,__VA_ARGS__)
  #define debug_out(c,...)
#endif //DEBUG

#define DFU_REQ_MAX_TEMP    3500
#define DFU_REQ_MIN_TEMP    500
#define DFU_REQ_EKG_HALT_TIMEOUT    100
#define DFU_REQ_EKG_SHTDWN_TIMEOUT    100


#define BLANK_ID "------------"

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
bool bIs_carrier_connection = false;
bool bIs_protocol_connection = false;
bool bWait_forever_for_protocol_connection = false;
bool bDid_radio_rfd = false;
bool bDid_radio_send_frame = false;
bool bIs_radio_on = false;
bool bDid_ekg_halt = false;
bool bDid_ekg_shtdwn = false;

//static uint8_t uiINMD_Protocol_frame[INMD_FRAME_BUFF_SIZE];
////static uint16_t uiINMD_Protocol_frame_len = 0;
//
//static bool bIs_protocol_frame = false;
//
////static const int dataApplicable[7] = { 0, 1, 0, 1, 1, 1, 1 };
//static char NACK[NACK_FRAME_SIZE]; //this is a string
//static char ACK[ACK_FRAME_SIZE]; //this too is a string
////static const char status0X11[] = { 0x9C, 0x01, 0x11, 0x01, 0x64, 0x50, 0x0B, 0xB8, 0xD7, 0x68, 0x56, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xC9 }; //reply for high voltage query.
////static const char status0x13[] = { 0x9C, 0x04, 0x20, 0x21, 0x22,0xC9 };
////static const char status0x14[] = { 0x9C, 0x04, 0x20, 0x21, 0x22, 0xC9 };
////static const char status0x15[] = { 0x9C, 0x03, 0x0E, 0x16, 0x03, 0x10, 0x0A, 0x9C, 0xC9, 0xC9, 0x0E, 0x2C, 0x9C, 0xC9 };

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
typedef enum eINDM_Cmnd_Proto_msg
{
  CMND_MSG_NONE,
  CMND_MSG_WAKEUP,
  CMND_MSG_SLEEP,
  CMND_MSG_OPEN_PROTOCOL_CONNECTION,
  CMND_MSG_PROTOCOL_FRAME,
  CMND_MSG_INTERFACE_ESTABLISHED,
  CMND_MSG_PROTOCOL_INTERFACE_ESTABLISHED,
  CMND_MSG_PROTOCOL_INTERFACE_CLOSED,
  CMND_MSG_SEND_PERIODIC_STATUS,
}eINDM_Cmnd_Proto_msg;

typedef enum eINMD_protocol_packet_type
{
  DATAGRAM_TYPE_COMMAND = 0x01,
  DATAGRAM_TYPE_STATUS_INFO = 0x02,
  DATAGRAM_TYPE_DATA = 0x03,
  DATAGRAM_TYPE_REAL_TIME_DATA = 0x04,
  DATAGRAM_TYPE_STORED_DATA = 0x05
}eINMD_Cmnd_packet_type;

typedef enum eINMD_protocol_command_id
{
  COMMAND_ID_REQUEST_STATUS  = 0x11,
  COMMAND_ID_SET_STATUS = 0x12,
  COMMAND_ID_GET_DATA_SET_INFO = 0x13,
  COMMAND_ID_DATA_SET_TRANSFER = 0x14,
  COMMAND_ID_ERASE_DATA_SET = 0x15,
  COMMAND_ID_CAPTURE_DATA_SET = 0x16,
  COMMAND_ID_STREAM_DATA = 0x17,
  COMMAND_ID_REQUEST_TO_TEST = 0x18,
  COMMAND_ID_SET_SYSTEM_PARAMETER = 0x19
}eINMD_protocol_command_id;

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
extern Mailbox_Handle tTIRTOS_cmnd_protocol_mailbox;
extern Clock_Handle   tProto_Cmndr_status_clock;

typedef struct tProtocol_msg_struct
{
  eINDM_Cmnd_Proto_msg eMsg;
  uint8_t   uiCmnd_Frame[PROTOCOL_FRAME_MAX_LEN];
  uint32_t  uiCmnd_Frame_len;
}tProtocol_msg_struct;

typedef struct tStatus_msg_struct
{
  int8_t major_firmware_version;
  int8_t minor_firmware_version;
  int8_t subversion_firmware;
  uint8_t battery_voltage;
  int8_t dongle_temp;
  int8_t operating_mode;
  int8_t capture_settings[2];
  uint32_t real_time;
  uint8_t leads_off_p;
  uint8_t leads_off_n;
  //uint16_t leads_off;

  int8_t alarms[2];
  char dongle_ID[12];
  char patient_ID[12];
  char facilities_ID[12];
}tStatus_msg_struct;

static tStatus_msg_struct sStatus_Message;

typedef struct tProtocol_Commander_Activity_State
{
  bool bIs_carrier_connection;
  bool bIs_protocol_connection;
  bool bWait_forever_for_protocol_connection;
  bool bDid_radio_rfd;
  bool bDid_radio_send_frame;
  bool bIs_radio_on;
  bool bDid_ekg_halt;
  bool bDid_ekg_shtdwn;
  bool bDid_rcv_cmnd_to_DFU;
}tProtocol_Commander_Activity_State;

tProtocol_Commander_Activity_State tPC_Activity_State =
{
    false, //bool bIs_carrier_connection = false;
    false, //bool bIs_protocol_connection = false;
    false, //bool bWait_forever_for_protocol_connection = false;
    false, //bool bDid_radio_rfd = false;
    false, //bool bDid_radio_send_frame = false;
    false, //bool bIs_radio_on = false;
    false, //bool bDid_ekg_halt = false;
    false, //bool bDid_ekg_shtdwn = false;
    false, //bool bDid_rcv_cmnd_to_DFU;
};
/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

//extern void check_for_update(void);
/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
void       vRadio_connection_callback      (bool bRadio_Carrier_Connection, bool bRadio_Protocol_Connection);
void       vRadio_change_settings_callback (bool bDid_Setting_Change);
void       vRadio_sent_frame_callback      (uint32_t uiCount);
void       vRadio_rcv_frame_callback(uint8_t * pBuff, uint32_t uiRcvd_len);
void       vRadio_setup_callback(bool bRadio_Ready, bool bRadio_On);
void       vEKG_halt_request_callback(uint32_t uiData, bool bEKG_running);
void       vEKG_shtdwn_request_callback(uint32_t uiData, bool bEKG_on);
void       vUSB_conn_callback(bool bPhys_conn, bool bData_conn);
void       vProto_Commander_status_INT_service(void);
ERROR_CODE eIneedmd_send_ack(void);  //send protocol acknowledgement
ERROR_CODE eIneedmd_send_nack(void);  //send protocol fail acknowledgement
ERROR_CODE eParse_Protocol_Frame(uint8_t * pRcvd_protocol_frame, uint32_t uiRcvd_frame_len);
ERROR_CODE eValidatePacketType(eINMD_Cmnd_packet_type eProtocolFrameType);
ERROR_CODE eValidateCommandID(eINMD_Cmnd_packet_type eProtocolFrameType, eINMD_protocol_command_id szRecCommand);

ERROR_CODE eCommand_led_test(INMD_LED_COMMAND eInmd_LED_cmnd);
ERROR_CODE eCommand_enable_test_signal(uint8_t signal);
ERROR_CODE eCommand_stop_test();
ERROR_CODE eCommand_DFU(void);
ERROR_CODE eCommand_reset_test();
ERROR_CODE eCommand_set_status(uint8_t* statusToSet);
ERROR_CODE eCommand_capture_data(uint8_t* duration);
ERROR_CODE eCommand_get_data_set_info(uint8_t* dataSet);
ERROR_CODE eCommand_transfer_data_set(uint8_t* dataSet);
ERROR_CODE eCommand_erase_data_set(uint8_t* dataSet);
ERROR_CODE eCommand_send_status(void);
ERROR_CODE eCommand_stream_data(uint8_t bRealTime);
ERROR_CODE eCommand_error_test(uint8_t uiError_to_send);

ERROR_CODE eIneedmd_command_message_params_init(tProtocol_msg_struct * tParams);

////int debug_out(char * cOut_buff,...);

void writeDataToPort(unsigned char * cOut_buff, uint16_t uiLen);
ERROR_CODE eCheckAlarmBytes(const unsigned char *Frame, int noPosh, int *ackFlag);
ERROR_CODE eCheckTimeBytes(const unsigned char *Frame, int noPosh, int *ackFlag);
#ifdef PrintCommand
  void vCMND_ECHO_FRAME(const unsigned char * uiFrame, uint16_t uiFrame_len);
#endif

/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
void vRadio_connection_callback(bool bRadio_Carrier_Connection, bool bRadio_Protocol_Connection)
{
  tINMD_protocol_req_notify tCmnd_Req_Notify;
  ERROR_CODE eEC = ER_FAIL;
  eIneedmd_cmnd_Proto_ReqNote_params_init(&tCmnd_Req_Notify);

  if((bRadio_Carrier_Connection  == true) & \
     (bRadio_Protocol_Connection == true))
  {
    //A carrier is connected and attempting to establish a protocol connection
    //
    if(bIs_protocol_connection == false)
    {
      bIs_protocol_connection = true;
      tCmnd_Req_Notify.eReq_Notify = CMND_NOTIFY_PROTOCOL_INTERFACE_ESTABLISHED;
      eEC = ER_OK;
    }else{/*do nothing*/}
  }
  else if((bRadio_Carrier_Connection  == true) & \
          (bRadio_Protocol_Connection == false))
  {
    //A carrier is connected but is not attempting to establish a protocol connection
    //
    if(bIs_protocol_connection == true)
    {
      bIs_protocol_connection = false;
      tCmnd_Req_Notify.eReq_Notify = CMND_NOTIFY_PROTOCOL_INTERFACE_CLOSED;
      eEC = ER_OK;
    }
    else
    {
      Clock_stop(tProto_Cmndr_status_clock);
      bIs_carrier_connection = true;
    }
  }
  else if((bRadio_Carrier_Connection  == false) & \
          (bRadio_Protocol_Connection == false))
  {
    if(bWait_forever_for_protocol_connection == false)
    {
      if((bIs_protocol_connection == false) &\
         (bIs_carrier_connection == true))
      {
        //The carrier dropped without verifying the protocol connection
        //re-open the interface for a connection
        bIs_carrier_connection = false;
        Clock_stop(tProto_Cmndr_status_clock);
        tCmnd_Req_Notify.eReq_Notify = CMND_NOTIFY_PROTOCOL_INTERFACE_CLOSED;
        eEC = ER_OK;
      }
      else if ((bIs_protocol_connection == false) &\
               (bIs_carrier_connection == false))
      {
        //No carrier and protocol connection was detected
        //
        Clock_stop(tProto_Cmndr_status_clock);
        tCmnd_Req_Notify.eReq_Notify = CMND_NOTIFY_PROTOCOL_INTERFACE_CONN_TIME_OUT;
        eEC = ER_OK;
      }
      else
      {
        //Carrier and or protocol connection disconnected, notify that proto conn closed
        //
        bIs_protocol_connection = false;
        bIs_carrier_connection = false;
        Clock_stop(tProto_Cmndr_status_clock);
        tCmnd_Req_Notify.eReq_Notify = CMND_NOTIFY_PROTOCOL_INTERFACE_CLOSED;
        eEC = ER_OK;
      }
    }
  }
  else
  {
    vDEBUG_ASSERT("vRadio_connection_callback SYS_HALT, unhandled connection status case", 0);
  }


  if(eEC == ER_OK)
  {
    eIneedmd_cmnd_Proto_Request_Notify(&tCmnd_Req_Notify);
  }
  return;
}

void vRadio_change_settings_callback(bool bDid_Setting_Change)
{

  //todo: protocol commander needs to keep track of which setting it requested to change and notify if it was successful

  return;
}

void vRadio_sent_frame_callback(uint32_t uiCount)
{
  bDid_radio_send_frame = true;

  return;
}

void vRadio_rcv_frame_callback(uint8_t * pBuff, uint32_t uiRcvd_len)
{
  tINMD_protocol_req_notify tProtocol_req_notify;
  eIneedmd_cmnd_Proto_ReqNote_params_init(&tProtocol_req_notify);

  tProtocol_req_notify.eReq_Notify = CMND_REQUEST_RCV_PROTOCOL_FRAME;

  tProtocol_req_notify.uiCmnd_Frame = pBuff;
  tProtocol_req_notify.uiFrame_Len = uiRcvd_len;
  eIneedmd_cmnd_Proto_Request_Notify(&tProtocol_req_notify);
  return;
}

void vRadio_setup_callback(bool bRadio_Ready, bool bRadio_On)
{
  tProtocol_msg_struct tProtocol_msg;

  if(bRadio_On == true)
  {
    tProtocol_msg.eMsg = CMND_MSG_OPEN_PROTOCOL_CONNECTION;
    Mailbox_post(tTIRTOS_cmnd_protocol_mailbox, &tProtocol_msg, BIOS_WAIT_FOREVER);
    bIs_radio_on = true;
  }
  else
  {
    bIs_radio_on = false;
  }

  return;
}

void vEKG_halt_request_callback(uint32_t uiData, bool bEKG_running)
{
  if(bEKG_running == false)
  {
    bDid_ekg_halt = true;
  }
  else
  {
    bDid_ekg_halt = false;
  }

  return;
}

void vEKG_shtdwn_request_callback(uint32_t uiData, bool bEKG_on)
{
  if(bEKG_on == false)
  {
    bDid_ekg_shtdwn = true;
  }
  else
  {
    bDid_ekg_shtdwn = false;
  }

  return;
}

void vUSB_conn_callback(bool bPhys_conn, bool bData_conn)
{
  return;
}

void vProto_Commander_UI_Status_INT_service(void)
{
  tProtocol_msg_struct tProtocol_msg;

  //send a status message
  tProtocol_msg.eMsg = CMND_MSG_SEND_PERIODIC_STATUS;
  Mailbox_post(tTIRTOS_cmnd_protocol_mailbox, &tProtocol_msg, BIOS_WAIT_FOREVER);
  //set the power_LED
  //set the heart led
  return;
}


void vProto_Cmndr_RTC_clock_INT_service(void)
{

	//increments the clock by one second
	if (sStatus_Message.real_time >= 0xffffffff)
	{
		sStatus_Message.real_time=0;
	}
	else
	{
		sStatus_Message.real_time++;
	}

  return;
}

/******************************************************************************
* name: eIneedmd_send_ack
* description:
* param description: void
* return value description: ERROR_CODE - ER_OK:
*                                        ER_FAIL:
******************************************************************************/
ERROR_CODE eIneedmd_send_ack(void)
{
  ERROR_CODE eEC = ER_FAIL;
  uint8_t ACK_[PROTOCOL_FRAME_ACK_LEN] = {0x9c, 0x00, 0x04, 0xC9};
  tRadio_request tRequest;

  if(bIs_protocol_connection == true)
  {
    eEC = eIneedmd_radio_request_params_init (&tRequest);

    if(eEC == ER_OK)
    {
      bDid_radio_send_frame = false;

      tRequest.eRequest = RADIO_REQUEST_SEND_FRAME;
      memcpy(tRequest.uiBuff, ACK_, PROTOCOL_FRAME_ACK_LEN);
      tRequest.uiBuff_size = PROTOCOL_FRAME_ACK_LEN;
      tRequest.eTX_Priority = TX_PRIORITY_QUEUE;
      tRequest.vBuff_sent_callback = &vRadio_sent_frame_callback;
      eEC = eIneedmd_radio_request(&tRequest);
    }else{/*do nothing*/}

    if(eEC == ER_OK)
    {
      while(bDid_radio_send_frame == false)
      {
        Task_sleep(100);
      }
    }

    vDEBUG("ACK");
  }
  else
  {
    vDEBUG("No ACK, proto conn closed");
    eEC = ER_CLOSE;
  }

  return eEC;
}

/******************************************************************************
* name: eIneedmd_send_nack
* description:
* param description: void
* return value description: ERROR_CODE - ER_OK:
*                                        ER_FAIL:
******************************************************************************/
ERROR_CODE eIneedmd_send_nack(void)
{
  ERROR_CODE eEC = ER_FAIL;
  uint8_t NACK_[PROTOCOL_FRAME_NACK_LEN] = {0x9c, 0xFF, 0x04, 0xC9};
  tRadio_request tRequest;
  eEC = eIneedmd_radio_request_params_init (&tRequest);

  if(eEC == ER_OK)
  {
    bDid_radio_send_frame = false;
    tRequest.eRequest = RADIO_REQUEST_SEND_FRAME;
    memcpy(tRequest.uiBuff, NACK_, PROTOCOL_FRAME_NACK_LEN);
    tRequest.uiBuff_size = PROTOCOL_FRAME_NACK_LEN;
    tRequest.eTX_Priority = TX_PRIORITY_QUEUE;
    tRequest.vBuff_sent_callback = &vRadio_sent_frame_callback;
    eEC = eIneedmd_radio_request(&tRequest);
  }else{/*do nothing*/}

  while(bDid_radio_send_frame == false)
  {
    Task_sleep(100);
  }
  vDEBUG("NACK\n");
  //printf("NACK\n");

  return eEC;
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void writeDataToPort(unsigned char * cOut_buff, uint16_t uiLen)
{
  ERROR_CODE eEC = ER_FAIL;
  tRadio_request tRequest;

  eEC = eIneedmd_radio_request_params_init (&tRequest);
  if(eEC == ER_OK)
  {
    bDid_radio_send_frame = false;

    tRequest.eRequest = RADIO_REQUEST_SEND_FRAME;
    memcpy(tRequest.uiBuff, cOut_buff, uiLen);
    tRequest.uiBuff_size = uiLen;
    tRequest.eTX_Priority = TX_PRIORITY_QUEUE;
    tRequest.vBuff_sent_callback = &vRadio_sent_frame_callback;
    eEC = eIneedmd_radio_request(&tRequest);

    if(eEC == ER_OK)
    {
      while(bDid_radio_send_frame == false)
      {
        //todo: need timeout
        Task_sleep(1);
      }
//    ineedmd_radio_send_frame((uint8_t *)cOut_buff, uiLen);
    }else{/*do nothing*/}
  }

  return;
}

/*
 * This function requests a change of the front end then - if sucessfull reflects it back into the status information
 *
 *
 */
ERROR_CODE eRequestEkgSettingChange(uint8_t config_byte1, uint8_t config_byte2)
{
	tINMD_EKG_req tEKG_req;
	ERROR_CODE eROR = ER_FAIL;
	uint8_t setting_to_pass;
	uint8_t working_case = 0x00;

	eIneedmd_EKG_request_param_init(&tEKG_req);


// first check the speed against the existing speed...

	working_case = ADS_SPEED_BITS & config_byte1;

    if (working_case != (ADS_SPEED_BITS & sStatus_Message.capture_settings[0]))
    {
	  tEKG_req.eReq_ID = EKG_REQUEST_SAMPLE_RATE;
      switch(working_case)
	  {
	    case SPEED_7:
	    {
	      // set speed to 500 samples per second
	      setting_to_pass = ADS_SAMPLE_RATE_500SPS;
	      working_case = SPEED_3;
	      break;
	    }
	    case SPEED_6:
	    {
	      // set speed to 500 samples per second
	      setting_to_pass = ADS_SAMPLE_RATE_500SPS;
		  working_case = SPEED_3;

		  break;
	    }
	    case SPEED_5:
	    {
		 // set speed to 2k samples per second
	      setting_to_pass = ADS_SAMPLE_RATE_2KSPS;
		  working_case = SPEED_5;

		  break;
	    }
	    case SPEED_4:
	    {
	      // set speed to 1k samples per second
	      setting_to_pass = ADS_SAMPLE_RATE_1KSPS;
		  working_case = SPEED_4;

		  break;
	    }
	    case SPEED_3:
	    {
          // set speed to 500 samples per second
	      setting_to_pass = ADS_SAMPLE_RATE_500SPS;
	      working_case = SPEED_3;

	      break;
	    }
	    case SPEED_2:
	    {
	      // set speed to 250 samples per second
	      setting_to_pass = ADS_SAMPLE_RATE_250SPS;
	      working_case = SPEED_2;
	      break;
	    }
	    case SPEED_1:
	    {
          // set speed to 125 samples per second
	      setting_to_pass = ADS_SAMPLE_RATE_125SPS;
	      working_case = SPEED_1;
	      break;
	    }
	    default:
	    	break;

	  }


			tEKG_req.uiADS_Setting = &setting_to_pass;
			sStatus_Message.capture_settings[0] = working_case | (sStatus_Message.capture_settings[0] & ~ADS_SPEED_BITS);

      eROR = eIneedmd_EKG_request(&tEKG_req);
    }

	eIneedmd_EKG_request_param_init(&tEKG_req);

	//todo: put this back in once switching the reference works
	/*
// first check the reference against the existing reference...

	working_case = ADS_REFERENCE_BITS & config_byte1;

    if (working_case != (ADS_REFERENCE_BITS & sStatus_Message.capture_settings[0]))
    {
	  tEKG_req.eReq_ID = EKG_REQUEST_REFERENCE;
      switch(working_case)
	  {
	    case NO_REFERENCE:
	    {
	      // set speed to 1k samples per second
	      setting_to_pass = VREF_OFF;
	      working_case = NO_REFERENCE;
	      break;
	    }
	    case INTERNAL_REFERENCE:
	    {
	      // set speed to 1k samples per second
	      setting_to_pass = VREF_INTERNAL;
		  working_case = INTERNAL_REFERENCE;

		  break;
	    }
	    case EXTERNAL_REFERENCE:
	    {
		 // set speed to 2k samples per second
	      setting_to_pass = VREF_EXTERNAL;
		  working_case = EXTERNAL_REFERENCE;

		  break;
	    }

	    default:
	    	break;

	  }
	  tEKG_req.uiADS_Setting = &setting_to_pass;
	  sStatus_Message.capture_settings[0] = working_case | (sStatus_Message.capture_settings[0] & ~ADS_REFERENCE_BITS);

      eROR = eIneedmd_EKG_request(&tEKG_req);
    }
	eIneedmd_EKG_request_param_init(&tEKG_req);
    */

	// first check the gain against the existing gain...

	working_case = ADS_GAIN_BITS & config_byte1;

    if (working_case != (ADS_GAIN_BITS & sStatus_Message.capture_settings[0]))
    {
	  tEKG_req.eReq_ID = EKG_REQUEST_CHANGE_GAIN;
      switch(working_case)
      {
	    case GAIN_1:
	    {
	      // set gain to 1
          setting_to_pass = ADS_GAIN_1;
	      working_case = GAIN_1;
	      break;
	    }
	    case GAIN_2:
	    {
	      // set gain to 2
	      setting_to_pass = ADS_GAIN_2;
		  working_case = GAIN_2;

		  break;
	    }
	    case GAIN_3:
	    {
          // set gain to 3
	      setting_to_pass = ADS_GAIN_3;
		  working_case = GAIN_3;

		  break;
	    }
	    case GAIN_4:
	    {
	      // gain to 4
	      setting_to_pass = ADS_GAIN_4;
		  working_case = GAIN_4;

		  break;
	    }
	    case GAIN_6:
	    {
          // gain 6
	      setting_to_pass = ADS_GAIN_6;
	      working_case = GAIN_6;

	      break;
	    }
	    case GAIN_8:
	    {
	      // gain 8
	      setting_to_pass = ADS_GAIN_8;
	      working_case = GAIN_8;
	      break;
	    }
	    case GAIN_12:
	    {
          // gain 12
	      setting_to_pass = ADS_GAIN_12;
	      working_case = GAIN_12;
	      break;
	    }
	    default:
	    	break;
	  }


	  tEKG_req.uiADS_Setting = &setting_to_pass;
	  sStatus_Message.capture_settings[0] = working_case | (sStatus_Message.capture_settings[0] & ~ADS_GAIN_BITS);

      eROR = eIneedmd_EKG_request(&tEKG_req);
    }
    return eROR;
}



/******************************************************************************
* name: eParse_Protocol_Frame
* description:
* param description: uint8_t - pointer: pointer to the buffer containing the protocol frame
* return value description: ERROR_CODE - ER_OK:
******************************************************************************/
ERROR_CODE eParse_Protocol_Frame(uint8_t * pRcvd_protocol_frame, uint32_t uiRcvd_frame_len)
{
  ERROR_CODE eEC = ER_FAIL;
  eINMD_Cmnd_packet_type    eProtocolFrameType;//dataPacketType;// = ucProtocol_Frame[1];
  eINMD_protocol_command_id eProtocolFrameCommandID;//actCommand;    // = ucProtocol_Frame[3];
  char temp_id[13];
  char saved_id[13];
  memset(temp_id, 0x00, 13);
  memset(saved_id, 0x00, 13);
  int iCmp = 0;

  tPower_control_request tPC_req;

  eProtocolFrameType      = (eINMD_Cmnd_packet_type)   pRcvd_protocol_frame[PROTOCOL_FRAME_CMND_TYPE_INDEX];
  eProtocolFrameCommandID = (eINMD_protocol_command_id)pRcvd_protocol_frame[PROTOCOL_FRAME_CMND_ID_INDEX];

  //cntDataBytes = ucProtocolFrameLength - 5;

  eEC = eValidatePacketType(eProtocolFrameType);
  if(eEC == ER_OK)
  {
    eEC = eValidateCommandID(eProtocolFrameType, eProtocolFrameCommandID);
    if (eEC == ER_FAIL)
    {
      printf("\nCommand - %X is invalid for DataPacketType - %X!", eProtocolFrameCommandID, eProtocolFrameType);
      printf("\nSending NACK...");
      eIneedmd_send_nack();
    }
    else if (eEC == ER_INVALID)
    {
      printf("\nCommand - %X is currently not supported for DataPacketType - %X!", eProtocolFrameCommandID, eProtocolFrameType);
      printf("\nSending NACK...");
      eIneedmd_send_nack();
    }
    else
    {
      //command ID good, length and packet check passed
      switch (eProtocolFrameCommandID)
      {

        case COMMAND_ID_SET_STATUS: //status
        {

          eEC = eCommand_set_status(&pRcvd_protocol_frame[COMMAND_DATA_BYTE]);
          if(eEC == ER_OK)
          {
            eIneedmd_send_ack();
          }
          else
          {
            eIneedmd_send_nack();
            eEC = ER_OK;
          }
          break;
        }
        case COMMAND_ID_CAPTURE_DATA_SET: //capture data set
        {

          eEC = eCommand_capture_data(&pRcvd_protocol_frame[COMMAND_DATA_BYTE]);

          if(eEC == ER_OK)
          {
            eIneedmd_send_ack();
          }
          else
          {
            eIneedmd_send_nack();
            eEC = ER_OK;
          }
          break;
        }
        case COMMAND_ID_GET_DATA_SET_INFO: //get data set info
        {
          eEC = eCommand_get_data_set_info(&pRcvd_protocol_frame[COMMAND_DATA_BYTE]);
          if(eEC == ER_OK)
          {
            eIneedmd_send_ack();
          }
          else
          {
            eIneedmd_send_nack();
            eEC = ER_OK;
          }
          break;
        }
        case COMMAND_ID_DATA_SET_TRANSFER://data set transfer command  --  NAK or (ACK + respective data set corresponding to the id sent). For now, ACK.
        {
          eEC = eCommand_transfer_data_set(&pRcvd_protocol_frame[COMMAND_DATA_BYTE]);
          if(eEC == ER_OK)
          {
            eIneedmd_send_ack();
          }
          else
          {
            eIneedmd_send_nack();
            eEC = ER_OK;
          }
          break;
        }
        case COMMAND_ID_ERASE_DATA_SET://erase data set
        {

          //data set ID, 2bytes
          eEC = eCommand_erase_data_set(&pRcvd_protocol_frame[COMMAND_DATA_BYTE]);
          if(eEC == ER_OK)
          {
            eIneedmd_send_ack();
          }
          else
          {
            eIneedmd_send_nack();
            eEC = ER_OK;
          }
          break;
        }
        case COMMAND_ID_REQUEST_STATUS://get status.
        {
          eEC = eCommand_send_status();
          if(eEC == ER_OK)
          {
            eIneedmd_send_ack();
          }
          else
          {
            eIneedmd_send_nack();
            eEC = ER_OK;
          }
          

          break;
        }
        case COMMAND_ID_STREAM_DATA:
        {
          eEC = eCommand_stream_data(pRcvd_protocol_frame[COMMAND_DATA_BYTE]);
          if(eEC == ER_OK)
          {
            eIneedmd_send_ack();
          }
          else
          {
            eIneedmd_send_nack();
            eEC = ER_OK;
          }
          break;
        }
        //todo: magic numbers
        case COMMAND_ID_REQUEST_TO_TEST://request to send test signal.
        {
          //printf("Received Request to Test");
          if(pRcvd_protocol_frame[COMMAND_DATA_BYTE] == EKG_TEST_PAT)
          {
            printf("Received test signal request...");
            eEC = eCommand_enable_test_signal((char)pRcvd_protocol_frame[5]);
            if(eEC == ER_OK)
            {
              eIneedmd_send_ack();
            }
            else
            {
              eIneedmd_send_nack();
              eEC = ER_OK;
            }
          }
          else if(pRcvd_protocol_frame[COMMAND_DATA_BYTE] == LED_TEST_PATTERN)
          {
            printf("Received LED test request");
            eEC = eCommand_led_test((INMD_LED_COMMAND)pRcvd_protocol_frame[5]);

            //Check if the UI request was successful
            if(eEC == ER_OK)
            {
              eIneedmd_send_ack();
            }
            else
            {
              eIneedmd_send_nack();
              eEC = ER_OK;
            }
            eEC = ER_OK;
          }
          else if(pRcvd_protocol_frame[COMMAND_DATA_BYTE] == STOP_EKG_TEST_PAT)
          {
            eEC = eCommand_stop_test();
            if(eEC == ER_OK)
            {
              eIneedmd_send_ack();
            }
            else
            {
              eIneedmd_send_nack();
              eEC = ER_OK;
            }
          }
          else if(pRcvd_protocol_frame[COMMAND_DATA_BYTE] == REQ_FOR_DFU)
          {
            eEC = eCommand_DFU();
            if(eEC == ER_OK)
            {
              eIneedmd_send_ack();
              eEC = ER_FAIL;    //tell radio to not expect any more commands
            }
            else
            {
              eIneedmd_send_nack();
              eEC = ER_OK;
            }
          }
          else if(pRcvd_protocol_frame[COMMAND_DATA_BYTE] == REQ_FOR_RESET)
          {
            eEC = eCommand_reset_test();
            if(eEC == ER_OK)
            {
              eIneedmd_send_ack();
              ineedmd_watchdog_doorbell();
              eEC = ER_FAIL;
            }
            else
            {
              eIneedmd_send_nack();
              eEC = ER_OK;
            }
          }
          else if(pRcvd_protocol_frame[COMMAND_DATA_BYTE] == REQ_POWER_DOWN)
          {
              eIneedmd_send_ack();

              ePower_Control_Request_params_init(&tPC_req);
              tPC_req.eRequest = PC_REQUEST_FORCE_SHUTDOWN;
              ePower_Control_Request(&tPC_req);

          }
          else if(pRcvd_protocol_frame[COMMAND_DATA_BYTE] == REQ_ERROR_INJECT)
          {
        	  eEC = eCommand_error_test(pRcvd_protocol_frame[5]);
			  if(eEC == ER_OK)
			  {
				eIneedmd_send_ack();
			  }
			  else
			  {
				eIneedmd_send_nack();
				eEC = ER_OK;
			  }
          }
          else
          {
            eIneedmd_send_nack();
            break;
          }
          printf("Done");
          break;
        } //end test request switch
        case COMMAND_ID_SET_SYSTEM_PARAMETER:
        	switch (pRcvd_protocol_frame[COMMAND_DATA_BYTE])
        	{
        	case COMMAND_ID_SET_CAPTURE_SETTINGS:
        	      eRequestEkgSettingChange (pRcvd_protocol_frame[5], pRcvd_protocol_frame[6]);
        	      eIneedmd_send_ack();
        		break;
        	case COMMAND_ID_SET_TIME:
        		memcpy(&sStatus_Message.real_time,&pRcvd_protocol_frame[5],(4));
        		eIneedmd_send_ack();
        		break;
        	case COMMAND_ID_SET_PATINET_ID:
        		memcpy ((char *)sStatus_Message.patient_ID ,(char *)&pRcvd_protocol_frame[5],12);
        		eIneedmd_send_ack();
        		break;
        	case COMMAND_ID_SET_DEVICE_ID:
        		memcpy(temp_id,&pRcvd_protocol_frame[5],(pRcvd_protocol_frame[2] - 6));
            memcpy((char *)saved_id, (char *)sStatus_Message.dongle_ID, 12);

        		iCmp = strcmp(saved_id, temp_id);
        		if (iCmp == 0)
        		{
        			save_dongle_ID(sStatus_Message.dongle_ID);
        		}
        		else
        		{
        			memcpy((char *)sStatus_Message.dongle_ID , (char *)temp_id, 12);
        		}
        		eIneedmd_send_ack();
        		break;
        	case COMMAND_ID_SET_FACILITY_ID:
        		memcpy ((char *)temp_id,(char *)&pRcvd_protocol_frame[5],12);
        		memcpy((char *)saved_id, (char *)sStatus_Message.facilities_ID, 12);

                if (strcmp(saved_id, temp_id) == 0)
                {
                  save_facilities_ID(sStatus_Message.facilities_ID);
                }
                else
                {
                  memcpy((char *)sStatus_Message.facilities_ID, (char *)temp_id, 12);
                }
    		    eIneedmd_send_ack();
        		break;
        	default:
        		eIneedmd_send_nack();
        		break;
          }//end COMMAND_ID_SET_SYSTEM_PARAMETER switch
        eCommand_send_status();
      }//end command type switch
    }//end command validate else/if
  }//end error code check
  else          //packet type not valid
  {
    printf("\nInvalid type of packet received!");
    printf("\nSending NACK!");
    eIneedmd_send_nack();
  }

  return eEC;
}

/******************************************************************************
* name: eValidatePacketType
* description: validates datagram packet type byte.
* param description: const unsigned - char: packet type byte
* return value description: ERROR_CODE - ER_OK: datagram packet type is valid
*                                        ER_FAIL: datagram packet type is invalid
******************************************************************************/
ERROR_CODE eValidatePacketType(eINMD_Cmnd_packet_type eProtocolFrameType)
{
  ERROR_CODE eEC = ER_FAIL;

  if((eProtocolFrameType == DATAGRAM_TYPE_COMMAND) |\
     (eProtocolFrameType == DATAGRAM_TYPE_STATUS_INFO) |\
     (eProtocolFrameType == DATAGRAM_TYPE_DATA) |\
     (eProtocolFrameType == DATAGRAM_TYPE_REAL_TIME_DATA) |\
     (eProtocolFrameType == DATAGRAM_TYPE_STORED_DATA))
  {
    eEC = ER_OK;
  }
  else
  {
    eEC = ER_FAIL;
  }

  return eEC;
}

/******************************************************************************
* name: eSearchArrCmd
* description: determines the command from the recieved protocol datagram
* param description: eINMD_Cmnd_packet_type - enum: the command packet type
*                    eINMD_protocol_command_id - enum: the recieved command id
* return value description: ERROR_CODE - ER_OK: The command id is valid for the packet type
*                                        ER_FAIL: The command id is not valid for the packet type
*                                        ER_NOT_ENABLED: the packet type or command ide is currently not supported
******************************************************************************/
ERROR_CODE eValidateCommandID(eINMD_Cmnd_packet_type eProtocolFrameType, eINMD_protocol_command_id szRecCommand)
{
  ERROR_CODE eEC = ER_FAIL;

  switch(eProtocolFrameType)
  {
    case DATAGRAM_TYPE_COMMAND:
      if((szRecCommand == COMMAND_ID_REQUEST_STATUS)      |\
         (szRecCommand == COMMAND_ID_SET_STATUS)          |\
         (szRecCommand == COMMAND_ID_GET_DATA_SET_INFO)   |\
         (szRecCommand == COMMAND_ID_DATA_SET_TRANSFER)   |\
         (szRecCommand == COMMAND_ID_ERASE_DATA_SET)      |\
         (szRecCommand == COMMAND_ID_CAPTURE_DATA_SET)    |\
         (szRecCommand == COMMAND_ID_STREAM_DATA)         |\
         (szRecCommand == COMMAND_ID_REQUEST_TO_TEST)     |\
         (szRecCommand == COMMAND_ID_SET_SYSTEM_PARAMETER))
      {
        eEC = ER_OK;
      }
      break;
    case DATAGRAM_TYPE_STATUS_INFO:
    case DATAGRAM_TYPE_DATA:
    case DATAGRAM_TYPE_REAL_TIME_DATA:
    case DATAGRAM_TYPE_STORED_DATA:
    default:
      eEC = ER_NOT_ENABLED;
      break;
  }

  return eEC;
}


/******************************************************************************
* name: eCommand_enable_test_signal
* description: enables test signal
* param description: char signal
* signal can be one of the following test signals:
*       EKG_TEST_PAT      0x01
*       TRIANGLE_TEST_PAT 0x00
*       SQUARE_TEST_PAT   0x01
*       WAVEFORM_TEST_PAT 0x02
* return value description: ERROR_CODE - ER_OK: The command id is valid for the packet type
*                                        ER_FAIL: The command id is not valid for the packet type
*                                        ER_NOT_ENABLED: the packet type or command is currently not supported
******************************************************************************/
ERROR_CODE eCommand_enable_test_signal(uint8_t signal)
{
  vDEBUG("Test Signal request received\n");
  ERROR_CODE eEC = ER_FAIL;
  tINMD_EKG_req tEKG_req;
  eIneedmd_EKG_request_param_init(&tEKG_req);
  switch(signal)
  {
    case TRIANGLE_TEST_PAT:
    {
      tEKG_req.eReq_ID = EKG_REQUEST_START_TRIANGLE_WAVE_STREAM;
      tEKG_req.uiLeadOffData = NULL;
      tEKG_req.vEKG_read_callback = NULL;
      eEC = eIneedmd_EKG_request(&tEKG_req);
      //enable triangle waveform UI
      if(eEC == ER_OK)
      {
        eIneedmd_UI_Test_Mode_Enable(true);
        tUI_request tUI_request_params;
        eEC = eIneedmd_UI_params_init(&tUI_request_params);
        tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
        tUI_request_params.eHeart_led_sequence = HEART_LED_TRIANGLE;
        tUI_request_params.eComms_led_sequence = COMMS_LED_TRIANGLE;
        tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
        tUI_request_params.bIs_test_pattern = true;
        eEC =  eIneedmd_UI_request(&tUI_request_params);
      }

      break;
    }
    case SQUARE_TEST_PAT:
    {
      tEKG_req.eReq_ID = EKG_REQUEST_START_SQUARE_WAVE_STREAM;
      tEKG_req.uiLeadOffData = NULL;
      tEKG_req.vEKG_read_callback = NULL;
      eEC = eIneedmd_EKG_request(&tEKG_req);
      //enable 1mv cal UI
      if(eEC == ER_OK)
      {
        eIneedmd_UI_Test_Mode_Enable(true);
        tUI_request tUI_request_params;
        eEC = eIneedmd_UI_params_init(&tUI_request_params);
        tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
        tUI_request_params.eHeart_led_sequence = HEART_LED_1MV_CAL;
        tUI_request_params.eComms_led_sequence = COMMS_LED_1MV_CAL;
        tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
        tUI_request_params.bIs_test_pattern = true;
        eEC =  eIneedmd_UI_request(&tUI_request_params);
      }

      break;
    }
    case WAVEFORM_TEST_PAT:
    {
      tEKG_req.eReq_ID = EKG_REQUEST_START_ECG_STREAM;
      tEKG_req.uiLeadOffData = NULL;
      tEKG_req.vEKG_read_callback = NULL;
      eEC = eIneedmd_EKG_request(&tEKG_req);
      break;
    }

    default:
      break;
  }
  return eEC;
}


/******************************************************************************
* name: eCommand_stop_test
* description: stops test signal generation
* param description: none
* return value description: ERROR_CODE - ER_OK: The command id is valid for the packet type
*                                        ER_FAIL: The command id is not valid for the packet type
*                                        ER_NOT_ENABLED: the packet type or command is currently not supported
******************************************************************************/
ERROR_CODE eCommand_stop_test()
{
  vDEBUG("Stop Test request received\n");
  ERROR_CODE eEC = ER_OK;
  tINMD_EKG_req tEKG_req;
  eEC = eIneedmd_EKG_request_param_init(&tEKG_req);
  tEKG_req.eReq_ID = EKG_REQUEST_STOP_STREAM;
  tEKG_req.uiLeadOffData = NULL;
  tEKG_req.vEKG_read_callback = NULL;
  eEC = eIneedmd_EKG_request(&tEKG_req);

  eEC = eIneedmd_EKG_request_param_init(&tEKG_req);
  tEKG_req.eReq_ID = EKG_REQUEST_LEADS_OFF_ONLY;
  tEKG_req.uiLeadOffData = NULL;
  tEKG_req.vEKG_read_callback = NULL;
  eEC = eIneedmd_EKG_request(&tEKG_req);

  if(eEC == ER_OK)
  {
    eIneedmd_UI_Test_Mode_Enable(false);
    tUI_request tUI_request_params;
    eEC = eIneedmd_UI_params_init(&tUI_request_params);
    tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED | INMD_UI_ELEMENT_SOUNDER);
    tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
    tUI_request_params.eComms_led_sequence = COMMS_LED_UI_OFF;
    tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
    tUI_request_params.eAlert_sound = ALERT_SOUND_UI_OFF;
    tUI_request_params.bIs_test_pattern = false;
    eEC =  eIneedmd_UI_request(&tUI_request_params);

  }

  return eEC;
}

/******************************************************************************
* name: eCommand_DFU
* description: puts device into DFU mode, requests UI sequence
* param description: none
* return value description: ERROR_CODE - ER_OK: The command id is valid for the packet type
*                                        ER_FAIL: The command id is not valid for the packet type
*                                        ER_NOT_ENABLED: the packet type or command is currently not supported
******************************************************************************/
ERROR_CODE eCommand_DFU(void)
{
  vDEBUG("DFU request received");
  ERROR_CODE eEC = ER_FAIL;
  ERROR_CODE eEC_safety_checks = ER_FAIL;
  ERROR_CODE eEC_sys_prep = ER_FAIL;
  tINMD_EKG_req tEKG_req;
  eEKG_Task_state eEKG_state;
  tUSB_req tUSB_request;
  tUI_request tUI_req;
  ADC_REQUEST tADC_req;
  tRadio_request tRadio_req;
  uint32_t uiEKG_halt_timeout = 0;
  uint32_t uiEKG_shtdwn_timeout = 0;

  tPC_Activity_State.bDid_rcv_cmnd_to_DFU = true;
  //Perform safety checks//////////////
  //
  //Check if EKG is in a patient monitoring state
  eEKG_state = eIneedmd_EKG_get_task_state();
  if(eEKG_state == EKG_TASK_WAVEFORM_CAPTURE)
  {
    eEC_safety_checks = ER_FAIL;
  }
  else
  {
    eEC_safety_checks = ER_OK;
  }

  //Check the current battery voltage
  if(eEC_safety_checks == ER_OK)
  {
    eADC_Request_param_init(&tADC_req);
    tADC_req.eRequest_ID = ADC_REQUEST_DEVICE_SUPPLY_VOLTAGE;
    tADC_req.uiDevice_supp_voltage = &sStatus_Message.battery_voltage;
    eADC_Request(&tADC_req);
    //iSupply_Voltage = uiPower_Control_Get_Supply_Voltage();
    if((sStatus_Message.battery_voltage == 0)   |\
       (sStatus_Message.battery_voltage <= DFU_MIN_VOLTAGE))  // this is 10th of a volt above 2.5 --- or 3.4v
    {
      eEC_safety_checks = ER_FAIL;
    }
    else
    {
      eEC_safety_checks = ER_OK;
    }
  }

  //Check the current system temperature
  if(eEC_safety_checks == ER_OK)
  {
    eADC_Request_param_init(&tADC_req);
    tADC_req.eRequest_ID = ADC_REQUEST_DEVICE_TEMPERATURE;
    tADC_req.uiDevice_temperature = &sStatus_Message.dongle_temp;
    eADC_Request(&tADC_req);
    if((sStatus_Message.dongle_temp >=  DFU_REQ_MAX_TEMP) |\
       (sStatus_Message.dongle_temp <=  DFU_REQ_MIN_TEMP))
    {
      eEC_safety_checks = ER_FAIL;
    }
    else
    {
      eEC_safety_checks = ER_OK;
    }

    //todo: temp check is not properly reading
    eEC_safety_checks = ER_OK;
  }

  //Check if the safety checks passed
  if(eEC_safety_checks == ER_FAIL)
  {
    //safety checks failed, set the error code to not ready to perform DFU yet
    eEC = ER_NOT_READY;
  }
  else
  {
    //Safety check passed, perform system prep
    //
    //Get the current EKG state
    eEKG_state = eIneedmd_EKG_get_task_state();
    //check if the EKG is in a "running" state
    if(eEKG_state == EKG_TASK_NONE)
    {
      //ekg not even started, nothing to shutdown
      eEC_sys_prep = ER_OK;
    }
    else if(eEKG_state != EKG_TASK_IDLE)
    {

      eIneedmd_EKG_request_param_init(&tEKG_req);
      tEKG_req.eReq_ID = EKG_REQUEST_STOP_STREAM;
      tEKG_req.vEKG_read_callback = &vEKG_halt_request_callback;
      eEC = eIneedmd_EKG_request(&tEKG_req);
      if(eEC == ER_OK)
      {
        //wait for the EKG task to finish halting
        while(bDid_ekg_halt == false)
        {
          Task_sleep(100);
          uiEKG_halt_timeout++;
          if(uiEKG_halt_timeout >= DFU_REQ_EKG_HALT_TIMEOUT)
          {
            eEC_sys_prep = ER_FAIL;
            break;
          }

          //Get the current EKG state
          eEKG_state = eIneedmd_EKG_get_task_state();
        }
        eEC_sys_prep = ER_OK;
      }
      else
      {
        eEC_sys_prep = ER_FAIL;
      }
    }

    //If EKG is in an idle state, request it to shut down completly
    if(eEKG_state == EKG_TASK_IDLE)
    {
      eIneedmd_EKG_request_param_init(&tEKG_req);
      tEKG_req.eReq_ID = EKG_REQUEST_EKG_SHUTDOWN;
      tEKG_req.vEKG_read_callback = &vEKG_shtdwn_request_callback;
      eEC = eIneedmd_EKG_request(&tEKG_req);
      if(eEC == ER_OK)
      {
        //wait for the EKG task to finish shutting down
        while(bDid_ekg_shtdwn == false)
        {
          Task_sleep(100);
          uiEKG_shtdwn_timeout++;
          if(uiEKG_shtdwn_timeout >= DFU_REQ_EKG_SHTDWN_TIMEOUT)
          {
            eEC_sys_prep = ER_FAIL;
            break;
          }
        }
        eEC_sys_prep = ER_OK;
      }
      else
      {
        //EKG request to shut down failed, set the error code
        eEC_sys_prep = ER_FAIL;
      }
    }

    //turn on the USB interface
    if(eEC_sys_prep == ER_OK)
    {
      eUSB_request_params_init(&tUSB_request);
      tUSB_request.eRequest = USB_REQUEST_FORCE_ENABLE;
      eEC = eUSB_request(&tUSB_request);
      if(eEC == ER_OK)
      {
        eEC_sys_prep = ER_OK;
      }
      else
      {
        eEC_sys_prep = ER_FAIL;
      }
    }

    if(eEC_sys_prep == ER_OK)
    {
      ineedmd_watchdog_barkcollar();
      eEC_sys_prep = ER_OK;
    }
  }

  //check if safety and system checks passed
  if((eEC_safety_checks == ER_OK) & \
     (eEC_sys_prep == ER_OK))
  {
    //put the system into DFU

    eIneedmd_UI_params_init(&tUI_req);
    tUI_req.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
    tUI_req.eComms_led_sequence = COMMS_LED_DFU;
    tUI_req.eHeart_led_sequence = HEART_LED_DFU;
    tUI_req.ePower_led_sequence = POWER_LED_DFU;
    eIneedmd_UI_request(&tUI_req);

    eIneedmd_send_ack();

    eIneedmd_radio_request_params_init (&tRadio_req);
    tRadio_req.eRequest = RADIO_REQUEST_BREAK_CONNECTION;
    eEC = eIneedmd_radio_request(&tRadio_req);

    tRadio_req.eRequest = RADIO_REQUEST_PERMANENT_POWER_OFF;
    eEC = eIneedmd_radio_request(&tRadio_req);

    //perform USB dfu
    vDEBUG("Starting USB DFU");
    Task_sleep(1000);

    //will not return!!!
    vBSP_USB_DFU();

  }
  else
  {
    //safety and system checks failed restore the system to previoius operation mode


    eEC = ER_FAIL;
  }


  //system restore
  return eEC;
}

/******************************************************************************
* name: eCommand_reset_test
* description: resets device, requests UI sequence
* param description: none
* return value description: ERROR_CODE - ER_OK: The command id is valid for the packet type
*                                        ER_FAIL: The command id is not valid for the packet type
*                                        ER_NOT_ENABLED: the packet type or command is currently not supported
******************************************************************************/
ERROR_CODE eCommand_reset_test()
{
  ERROR_CODE eEC = ER_FAIL;
  tRadio_request tRadio_req;
  tUI_request tUI_request_params;

  printf("Performing RESET..");
  //give the watchdog a long timeout timer
  ineedmd_watchdog_feed();
  eIneedmd_UI_params_init(&tUI_request_params);

  tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED | INMD_UI_ELEMENT_SOUNDER);
  tUI_request_params.eHeart_led_sequence = HEART_LED_REBOOT;
  tUI_request_params.eComms_led_sequence = COMMS_LED_REBOOT;
  tUI_request_params.ePower_led_sequence = POWER_LED_REBOOT;
  tUI_request_params.eAlert_sound = ALERT_SOUND_UI_OFF;
  eIneedmd_UI_request(&tUI_request_params);

  eIneedmd_radio_request_params_init (&tRadio_req);
  tRadio_req.eRequest = RADIO_REQUEST_BREAK_CONNECTION;
  eEC = eIneedmd_radio_request(&tRadio_req);

  eOSAL_delay(2000, NULL);

  vBSP_System_reset();

  return eEC;
}




/******************************************************************************
* name: eCommand_set_status
* description: sets device status based on statusToSet
* param description: statusToSet - pointer to status to set. should point to command packet
* return value description: ERROR_CODE - ER_OK: The command id is valid for the packet type
*                                        ER_FAIL: The command id is not valid for the packet type
*                                        ER_NOT_ENABLED: the packet type or command is currently not supported
******************************************************************************/
ERROR_CODE eCommand_set_status(uint8_t* statusToSet)
{
  tStatus_msg_struct status_set_packet;
  tPower_control_request tPC_req;

  memcpy (&status_set_packet, statusToSet, sizeof(status_set_packet) );

  if ( (0x60 & status_set_packet.operating_mode ) == 0x00 )
  {
    ePower_Control_Request_params_init(&tPC_req);
    tPC_req.eRequest = PC_REQUEST_FORCE_SHUTDOWN;
    ePower_Control_Request(&tPC_req);
  }


  //todo  lots of sub things need to be done in here depending on what is contained.


  vDEBUG("status set request received\n");
  //TODO: check if statusToSet is valid
  //TODO: set staus



  ERROR_CODE eEC = ER_FAIL;
  return eEC;
}

ERROR_CODE eCommand_capture_data(uint8_t* time)
{
  vDEBUG("capture request received\n");
  //TODO: check time/duration info
  ERROR_CODE eEC = ER_FAIL;
  return eEC;
}

ERROR_CODE eCommand_get_data_set_info(uint8_t* dataSet)
{
  vDEBUG("data set info request received\n");
  ERROR_CODE eEC = ER_FAIL;
  return eEC;
}

ERROR_CODE eCommand_transfer_data_set(uint8_t* dataSet)
{
  vDEBUG("transfer data request received\n");
  ERROR_CODE eEC = ER_FAIL;
  return eEC;
}

ERROR_CODE eCommand_erase_data_set(uint8_t* dataSet)
{
  vDEBUG("erase data set request received\n");
  ERROR_CODE eEC = ER_FAIL;
  return eEC;
}

ERROR_CODE eCommand_send_status(void)
{
//#define DEBUG_eCommand_send_status
#ifdef DEBUG_eCommand_send_status
  #define  vDEBUG_STATUS_PRINT_CMND vCMND_ECHO_FRAME
  #define  vDEBUG_SEND_STATUS  vDEBUG
  char cSend_buff[113];
  memset(cSend_buff, 0x00, 113);
#else
  #define vDEBUG_STATUS_PRINT_CMND(a, s)
  #define vDEBUG_SEND_STATUS(a)
#endif
  ERROR_CODE eEC = ER_FAIL;
  unsigned char* ucProtocol_Frame_resp;
  ucProtocol_Frame_resp = malloc(STATUS_LEN);
  tUI_request tUI_request_params;
  uint16_t uiLeadsOff = 0;

  eIneedmd_EKG_get_leads_off(&uiLeadsOff);
  sStatus_Message.leads_off_p = (uiLeadsOff >> 8) & 0xFF;
  sStatus_Message.leads_off_n = (uiLeadsOff & 0x08);
  sStatus_Message.operating_mode = 0x68;
  
  eIneedmd_UI_params_init(&tUI_request_params);

  if(uiLeadsOff != 0)
  {
    tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_SOUNDER);
    tUI_request_params.eHeart_led_sequence = HEART_LED_LEAD_OFF_NO_DATA;
    tUI_request_params.eAlert_sound = ALERT_SOUND_ALERT;
  }
  else
  {
    tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_SOUNDER);
    tUI_request_params.eHeart_led_sequence = HEART_LED_LEAD_ON;
    tUI_request_params.eAlert_sound = ALERT_SOUND_UI_OFF;
  }
  eIneedmd_UI_request(&tUI_request_params);

  ADC_REQUEST information_request;

  information_request.eRequest_ID = ADC_REQUEST_DEVICE_TEMPERATURE;
  information_request.uiDevice_temperature = &sStatus_Message.dongle_temp;
  eADC_Request(&information_request);


  information_request.eRequest_ID = ADC_REQUEST_DEVICE_SUPPLY_VOLTAGE;
  information_request.uiDevice_supp_voltage = &sStatus_Message.battery_voltage;
  eADC_Request(&information_request);


  if(ucProtocol_Frame_resp != NULL)
  {

    ucProtocol_Frame_resp[0] = 0x9c;
    ucProtocol_Frame_resp[1] = 0x02;
    ucProtocol_Frame_resp[2] = STATUS_LEN;

    memcpy(&ucProtocol_Frame_resp[3], &sStatus_Message, sizeof(sStatus_Message) );

    ucProtocol_Frame_resp[(STATUS_LEN-1)] = 0xc9;

    vDEBUG_STATUS_PRINT_CMND(ucProtocol_Frame_resp, ucProtocol_Frame_resp[2]); //this is just to display on our side, what reply we are sending.
    vDEBUG_SEND_STATUS("Sending status record...");

    writeDataToPort(ucProtocol_Frame_resp, STATUS_LEN );
    free(ucProtocol_Frame_resp);
    eEC = ER_OK;
  }
  return eEC;

#undef vDEBUG_STATUS_PRINT_CMND
#undef vDEBUG_SEND_STATUS
}

ERROR_CODE eCommand_stream_data(uint8_t realTime)
{
  vDEBUG("stream data request received\n");
  ERROR_CODE eEC = ER_FAIL;
  tINMD_EKG_req tEKG_req;
  if(realTime == 0xFF)
  {
    eEC = ER_OK;

    // create a new patient ID

    if ( memcmp((char *)sStatus_Message.patient_ID, (char *)BLANK_ID, 12) == 0 )
    {
    	set_patient_ID(sStatus_Message.patient_ID);
    }

    eIneedmd_EKG_request_param_init(&tEKG_req);
    tEKG_req.eReq_ID = EKG_REQUEST_START_ECG_STREAM;
    tEKG_req.uiLeadOffData = NULL;
    tEKG_req.vEKG_read_callback = NULL;
    eEC = eIneedmd_EKG_request(&tEKG_req);
  }
  else if (realTime == 0x00)
  {
    eEC = ER_OK;

    //stop the stream
    eIneedmd_EKG_request_param_init(&tEKG_req);
    tEKG_req.eReq_ID = EKG_REQUEST_STOP_STREAM;
    tEKG_req.uiLeadOffData = NULL;
    tEKG_req.vEKG_read_callback = NULL;
    eEC = eIneedmd_EKG_request(&tEKG_req);

    //put adc in leads off mode
    eEC = eIneedmd_EKG_request_param_init(&tEKG_req);
  	tEKG_req.eReq_ID = EKG_REQUEST_LEADS_OFF_ONLY;
  	tEKG_req.uiLeadOffData = NULL;
  	tEKG_req.vEKG_read_callback = NULL;
  	eEC = eIneedmd_EKG_request(&tEKG_req);

    blank_patient_ID(sStatus_Message.patient_ID);
  }
  else
  {
    eEC = ER_FAIL;
  }

  return eEC;
}

ERROR_CODE eCommand_led_test(INMD_LED_COMMAND eInmd_LED_cmnd)
{
  vDEBUG("LED Test request received\n");
  ERROR_CODE eEC = ER_FAIL;
  tUI_request tUI_request_params;

  if(eInmd_LED_cmnd >= INMD_LED_CMND_LIMIT)
  {
    eEC = ER_PARAM;
  }
  else
  {
    eIneedmd_UI_Test_Mode_Enable(true);
    eEC = eIneedmd_UI_params_init(&tUI_request_params);
    if(eEC == ER_OK)
    {
      switch(eInmd_LED_cmnd)
      {
        case INMD_LED_CMND_LED_OFF:
          printf("LED_OFF");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_UI_OFF;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_POWER_ON_BATT_LOW:
          printf("POWER_ON_BATT_LOW");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_UI_OFF;
          tUI_request_params.ePower_led_sequence = POWER_LED_POWER_ON_25to50;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_POWER_ON_BATT_GOOD:
          printf("POWER_ON_BATT_GOOD");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_UI_OFF;
          tUI_request_params.ePower_led_sequence = POWER_LED_POWER_ON_90to100;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_BATT_CHARGING:
          printf("BATT_CHARGING");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_UI_OFF;
          tUI_request_params.ePower_led_sequence = POWER_LED_POWER_ON_CHARGE_50to90;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_BATT_CHARGING_LOW:
          printf("BATT_CHARGING_LOW");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_UI_OFF;
          tUI_request_params.ePower_led_sequence = POWER_LED_POWER_ON_CHARGE_0to25;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_LEAD_LOOSE:
          printf("LEAD_LOOSE");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_LEAD_OFF_NO_DATA;
          tUI_request_params.eComms_led_sequence = COMMS_LED_UI_OFF;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_LEAD_GOOD_UPLOADING:
          printf("LEAD_GOOD_UPLOADING");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_LEAD_ON;
          tUI_request_params.eComms_led_sequence = COMMS_LED_UI_OFF;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_DIG_FLATLINE:
          printf("DIG_FLATLINE");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_DIGITAL_FLATLINE;
          tUI_request_params.eComms_led_sequence = COMMS_LED_UI_OFF;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_BT_CONNECTED:
          printf("BT_CONNECTED");
          //todo: update once 3 led
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_BLUETOOTH_CONNECTION_SUCESSFUL;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_BT_ATTEMPTING:
          printf("BT_ATTEMPTING");
          //todo: update once 3 led
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_BLUETOOTH_PAIRING;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_BT_FAILED:
          printf("BT_FAILED");
          //todo: update once 3 led
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_BLUETOOTH_PAIRING_FAILIED;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_USB_CONNECTED:
          printf("USB_CONNECTED");
          //todo: update once 3 led
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_USB_CONNECTION_SUCESSFUL;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_USB_FAILED:
          printf("USB_FAILED");
          //todo: update once 3 led
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_USB_CONNECTION_FAILED;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_DATA_TRANSFER:
          printf("DATA_TRANSFER");
          //todo: update once 3 led
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_DATA_TRANSFERING_FROM_DONGLE;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_TRANSFER_DONE:
          printf("TRANSFER_DONE");
          //todo: update once 3 led
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_DATA_TRANSFER_SUCESSFUL;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_STORAGE_WARNING:
          printf("STORAGE_WARNING");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_DONGLE_STORAGE_WARNING;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_ERASING:
          printf("ERASING");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_ERASING_STORED_DATA;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_ERASE_DONE:
          printf("ERASE_DONE");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
          tUI_request_params.eComms_led_sequence = COMMS_LED_ERASE_COMPLETE;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          //todo: set UI request params
          break;
        case INMD_LED_CMND_DFU_MODE:
          printf("DFU_MODE");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_DFU;
          tUI_request_params.eComms_led_sequence = COMMS_LED_DFU;
          tUI_request_params.ePower_led_sequence = POWER_LED_DFU;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_MV_CAL:
          printf("MV_CAL");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_1MV_CAL;
          tUI_request_params.eComms_led_sequence = COMMS_LED_1MV_CAL;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          //todo: set UI request params
          break;
        case INMD_LED_CMND_TRI_WVFRM:
          printf("TRI_WVFRM");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_TRIANGLE;
          tUI_request_params.eComms_led_sequence = COMMS_LED_TRIANGLE;
          tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          //todo: set UI request params
          break;
        case INMD_LED_CMND_REBOOT:
          printf("REBOOT");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_REBOOT;
          tUI_request_params.eComms_led_sequence = COMMS_LED_REBOOT;
          tUI_request_params.ePower_led_sequence = POWER_LED_REBOOT;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_LED_CMND_HIBERNATE:
          printf("HIBERNATE");
          //todo: set UI request params
          //todo: implement battery check function
          //check_battery();
          break;
        case INMD_LED_CMND_LEADS_ON:
          printf("LEADS_ON");
          //todo: set UI request params
          break;
        case INMD_LED_CMND_MEMORY_TEST:
          printf("MEMORY_TEST");
          //todo: set UI request params
          break;
        case INMD_LED_CMND_COM_BUS_TEST:
          printf("COM_BUS_TEST");
          //todo: set UI request params
          break;
        case INMD_LED_CMND_CPU_CLOCK_TEST:
          printf("CPU_CLOCK_TEST");
          //todo: set UI request params
          break;
        case INMD_LED_CMND_FLASH_TEST:
          printf("FLASH_TEST");
          //todo: set UI request params
          break;
        case INMD_LED_CMND_TEST_PASS:
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
          tUI_request_params.eHeart_led_sequence = HEART_LED_TEST_PASS;
          tUI_request_params.eComms_led_sequence = COMMS_LED_TEST_PASS;
          tUI_request_params.ePower_led_sequence = POWER_LED_TEST_PASS;
          tUI_request_params.bIs_test_pattern = true;
          printf("TEST_PASS");
          //todo: set UI request params
          break;
        case INMD_SOUNDER_CMND_OFF:
          printf("Sounder _off Test");
          tUI_request_params.uiUI_element = ( INMD_UI_ELEMENT_SOUNDER);
          tUI_request_params.eAlert_sound = ALERT_SOUND_UI_OFF;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_SOUNDER_CMND_ALERT:
          printf("Sounder Alert Test");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_SOUNDER);
          tUI_request_params.eAlert_sound = ALERT_SOUND_ALERT;
          tUI_request_params.bIs_test_pattern = true;
          break;
        case INMD_SOUNDER_CMND_ALARM:
          printf("Sounder Alarm Test");
          tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_SOUNDER);
          tUI_request_params.eAlert_sound = ALERT_SOUND_HP_ALARM;
          tUI_request_params.bIs_test_pattern = true;
          break;
        default:
          printf("Unrecongnized Sequence - No Soup For You");
          eEC = ER_FAIL;
          break;
      }

      if(eEC == ER_OK)
      {
        eEC =  eIneedmd_UI_request(&tUI_request_params);
      }
      else
      {
        eEC = ER_UNKNOWN_PARAM;
      }
    }else{/*do nothing*/}
  }
  return eEC;
}

ERROR_CODE eCommand_error_test(uint8_t uiError_to_send)
{
	vDEBUG("Error request received");
	ERROR_CODE eEC = ER_FAIL;
	tINMD_EKG_req tEKG_req;
	switch(uiError_to_send)
	{
	  case ERROR_DATA_PACKET:
	  {
		//tell the waveform task to inject an error. If successful, ER_OK should be returned
		eIneedmd_EKG_request_param_init(&tEKG_req);
		tEKG_req.eReq_ID = EKG_REQUEST_ERROR_PACKET;
		tEKG_req.uiLeadOffData = NULL;
		tEKG_req.vEKG_read_callback = NULL;
		eEC = eIneedmd_EKG_request(&tEKG_req);
		break;
	  }
	  case ERROR_STATUS_PACKET:
	  {
		//todo: add way to send error in status packet
		break;
	  }
	}
	//return an error code, eParse_Protocol_Frame() will take care of ack/nack behavior
	return eEC;
}

ERROR_CODE eIneedmd_command_message_params_init(tProtocol_msg_struct * tParams)
{
  ERROR_CODE eEC = ER_FAIL;

  tParams->eMsg = CMND_MSG_NONE;
  tParams->uiCmnd_Frame_len = 0;
  memset(tParams->uiCmnd_Frame, 0x00, PROTOCOL_FRAME_MAX_LEN);

  if((tParams->eMsg == CMND_MSG_NONE) &\
     (tParams->uiCmnd_Frame_len == 0))
  {
    eEC = ER_OK;
  }

  return eEC;
}

////*****************************************************************************
//// name:
//// description:
//// param description:
//// return value description:
////*****************************************************************************
//int iIneedmd_Rcv_cmnd_frame(uint8_t * uiCmnd_Frame, uint16_t uiCmnd_Frame_len)
//{
//  int iEC = 0;
//  void * vDid_copy = NULL;
//  memset(uiINMD_Protocol_frame, 0x00, INMD_FRAME_BUFF_SIZE);
//
//  if(uiCmnd_Frame_len >= INMD_FRAME_BUFF_SIZE)
//  {
//    iEC = -1;
//  }
//  else
//  {
//    vDid_copy = memcpy(uiINMD_Protocol_frame, uiCmnd_Frame, uiCmnd_Frame_len);
//    if(vDid_copy != NULL)
//    {
//      bIs_protocol_frame = true;
////      uiINMD_Protocol_frame_len = uiCmnd_Frame_len;
//    }
//    else
//    {
//      iEC = -2;
//    }
//  }
//  return iEC;
//}
//
////*****************************************************************************
//// name:
//// description:
//// param description:
//// return value description:
////*****************************************************************************
//int iIneedmd_Send_cmnd_frame(uint8_t * uiCmnd_Frame, uint16_t uiCmnd_Frame_len)
//{
//  return 1;
//}
//
////*****************************************************************************
//// name:
//// description:
//// param description:
//// return value description:
////*****************************************************************************
//bool iIneedmd_is_protocol_frame(void)
//{
//  bool bWas_frame;
//
//  bWas_frame = bIs_protocol_frame;
//  bIs_protocol_frame = false;
//
//  return bWas_frame;
//}

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* name: eIneedmd_cmnd_Proto_ReqNote_params_init
* description: initalizes the tINMD_protocol_request parameters to default values
* param description: tINMD_protocol_request - pointer: pointer to the structure
*                      to be initalized that will be passed to the request notify funcion
*
* return value description: ERROR_CODE - ER_OK: params initalized
*                                        ER_FAIL: params failed init
*                                        ER_PARAM: pointer to parameter
*                                          stucture passed in is invalid
******************************************************************************/
ERROR_CODE eIneedmd_cmnd_Proto_ReqNote_params_init(tINMD_protocol_req_notify * ptParams)
{
  ERROR_CODE eEC = ER_FAIL;

  if(ptParams == NULL)
  {
    eEC = ER_PARAM;
  }
  else
  {
    ptParams->eReq_Notify = CMND_REQUEST_NONE;
    ptParams->uiCmnd_Frame = NULL;
    ptParams->uiFrame_Len = 0;
  }

  if((ptParams->eReq_Notify != CMND_REQUEST_NONE) |\
     (ptParams->uiCmnd_Frame != NULL) |\
     (ptParams->uiFrame_Len != 0))
  {
    eEC = ER_FAIL;
  }
  else
  {
    eEC = ER_OK;
  }
  return eEC;
}

/******************************************************************************
* name: eIneedmd_cmnd_Proto_Request_Notify
* description:
* param description: tINMD_protocol_req_notify - pointer pointer to the structure
*                      that will be passed to the request notify funcion
* return value description: ERROR_CODE - ER_OK: request/notification accepted
*                                        ER_FAIL: request/notification not accepted
*                                        ER_PARAM: pointer to parameter stucture
*                                         passed in is invalid
*                                        ER_SIZE: command frame length is too large.
*                                        ER_LEN - command frame length is wrong.
*                                        ER_COMMAND - command is malformed.
*                                        ER_INVALID - command pointer is invalid
******************************************************************************/
ERROR_CODE eIneedmd_cmnd_Proto_Request_Notify(tINMD_protocol_req_notify * ptParams)
{
  ERROR_CODE eEC = ER_FAIL;
  tProtocol_msg_struct tProtocol_msg;
  tUI_request tUI_Req;
  bool bDid_msg_post = false;
  uint8_t uiProto_start_byte = 0;
  uint8_t uiProto_end_byte = 0;
  uint8_t uiProto_frame_len = 0;

  if(ptParams == NULL)
  {
    eEC = ER_PARAM;
  }
  else
  {
    switch(ptParams->eReq_Notify)
    {
      case CMND_REQUEST_WAKEUP:
        tProtocol_msg.eMsg = CMND_MSG_WAKEUP;
        eEC = ER_OK;
        break;
      case CMND_REQUEST_SLEEP:
        if(bIs_radio_on == true)
        {
          tProtocol_msg.eMsg = CMND_MSG_SLEEP;
          eEC = ER_OK;
        }
        else
        {
          eEC = ER_SKIP;
        }
        break;
      case CMND_REQUEST_RCV_PROTOCOL_FRAME:
        //Check if the frame length is greather then the max frame length size
        vDEBUG_ASSERT("eIneedmd_cmnd_Proto_Request_Notify SYS_HALT, proto frame size to large", (ptParams->uiFrame_Len <= PROTOCOL_FRAME_MAX_LEN));
        if(ptParams->uiFrame_Len > PROTOCOL_FRAME_MAX_LEN)
        {
          //Frame length exceedes limit
          //
          eEC = ER_SIZE;
        }
        else
        {
          //Frame length is within the limit
          //
          //Check if the command frame pointer is valid
          if(ptParams->uiCmnd_Frame == NULL)
          {
            //Command frame pointer is invalid
            //
            eEC = ER_INVALID;
          }
          else
          {
            //Command frame pointer is valid
            //
            uiProto_start_byte = ptParams->uiCmnd_Frame[0];
            uiProto_end_byte = ptParams->uiCmnd_Frame[(ptParams->uiFrame_Len - 1)];
            uiProto_frame_len = ptParams->uiCmnd_Frame[2];
            //Check if the command frame is properly formed
            if((uiProto_start_byte == PROTOCOL_FRAME_START_BYTE) & \
               (uiProto_frame_len  == ptParams->uiFrame_Len)     & \
               (uiProto_end_byte   == PROTOCOL_FRAME_END_BYTE))
            {
              //frame was properly formed
              //

              eIneedmd_command_message_params_init(&tProtocol_msg);

              //post command message to task
              tProtocol_msg.eMsg = CMND_MSG_PROTOCOL_FRAME;
              tProtocol_msg.uiCmnd_Frame_len = ptParams->uiFrame_Len;
              memcpy(&tProtocol_msg.uiCmnd_Frame[0], ptParams->uiCmnd_Frame, ptParams->uiFrame_Len);
              eEC = ER_OK;
            }
            else
            {
              //frame is malformed
              //
              eEC = ER_COMMAND;
            }
          }
        }
        break;
      case CMND_NOTIFY_INTERFACE_ESTABLISHED:
        break;
      case CMND_NOTIFY_PROTOCOL_INTERFACE_ESTABLISHED:
        //notify task that protocol interface was established
        tProtocol_msg.eMsg = CMND_MSG_PROTOCOL_INTERFACE_ESTABLISHED;
        eEC = ER_OK;
        break;
      case CMND_NOTIFY_PROTOCOL_INTERFACE_TIME_OUT:
        break;
      case CMND_NOTIFY_PROTOCOL_INTERFACE_CONN_TIME_OUT:
        //todo: add system shutdown since no connection was detected during the conn timout period
        eIneedmd_UI_params_init(&tUI_Req);
        tUI_Req.eComms_led_sequence = COMMS_LED_BLUETOOTH_PAIRING_FAILIED;
        tUI_Req.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
        eIneedmd_UI_request(&tUI_Req);
        break;
      case CMND_NOTIFY_PROTOCOL_INTERFACE_CLOSED:
        tProtocol_msg.eMsg = CMND_MSG_WAKEUP;
        eEC = ER_OK;
        break;
      case CMND_NOTIFY_LEADS_OFF:
    	//send a status message
    	tProtocol_msg.eMsg = CMND_MSG_SEND_PERIODIC_STATUS;
    	eEC = ER_OK;
    	break;
      default:
        break;
    }
  }

  if(eEC == ER_OK)
  {
    bDid_msg_post = Mailbox_post(tTIRTOS_cmnd_protocol_mailbox, &tProtocol_msg, BIOS_WAIT_FOREVER);
    if(bDid_msg_post == true)
    {
      eEC = ER_OK;
    }
    else
    {
      eEC = ER_FAIL;
    }
  }
  else if(eEC == ER_SKIP)
  {
    eEC = ER_OK;

  }else{/*do nothing*/}

  return eEC;
}


//*****************************************************************************
// name: vSeetup_Status_data_structure
// description: populated the data structure field so that all the session static information is stored.
// param description: NONE
// return value description: NONE
//*****************************************************************************

void vSeetup_Status_data_structure()
{
	//version number is the build version number and is coded in #defines.
	sStatus_Message.major_firmware_version = RELEASE_VERSION_MAJOR;
	sStatus_Message.minor_firmware_version = RELEASE_VERSION_MINOR;
	sStatus_Message.subversion_firmware = RELEASE_VERSION_SUBVE;

	sStatus_Message.capture_settings[0] = SPEED_3;

	read_facilities_ID(sStatus_Message.facilities_ID);
	read_dongle_ID(sStatus_Message.dongle_ID);
	blank_patient_ID(sStatus_Message.patient_ID);

	sStatus_Message.real_time = 0xD648EF00;

	return;
}


//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void vIneedmd_command_task(UArg a0, UArg a1)
{
  ERROR_CODE eEC = ER_OK;
  tProtocol_msg_struct tProtocol_msg;
  tRadio_request tRadio_request;
  tUI_request tUI_Req;
  uint32_t uiMsg_size = sizeof(tProtocol_msg_struct);
  uint32_t uiMbox_size = 0;
  tINMD_protocol_req_notify tCmnd_Req_Notify;
  tUSB_req tUSB_request;
  bool bIs_Status_Clock_Running = false;
  uint8_t iSupply_Voltage = 0;
  tSounder_Request tSounder_Req;
  ADC_REQUEST tADC_req;
  tPower_control_request tPC_req;

  uiMbox_size = Mailbox_getMsgSize(tTIRTOS_cmnd_protocol_mailbox);
  vDEBUG_ASSERT("vIneedmd_command_task invalid mailbox msg size", uiMsg_size == uiMbox_size);

  eIneedmd_radio_request_params_init (&tRadio_request);
  tRadio_request.eRequest                    = RADIO_REQUEST_REGISTER_CALLBACKS;
  tRadio_request.vBuff_sent_callback         = &vRadio_sent_frame_callback;
  tRadio_request.vBuff_receive_callback      = &vRadio_rcv_frame_callback;
  tRadio_request.vChange_setting_callback    = &vRadio_change_settings_callback;
  tRadio_request.vConnection_status_callback = &vRadio_connection_callback;
  tRadio_request.vSetup_state_callback       = &vRadio_setup_callback;
  eIneedmd_radio_request(&tRadio_request);

  eUSB_request_params_init(&tUSB_request);
  tUSB_request.eRequest = USB_REQUEST_REGISTER_CONN_CALLBACK;
  tUSB_request.vUSB_connection_callback = &vUSB_conn_callback;
  eUSB_request(&tUSB_request);

  eIneedmd_cmnd_Proto_ReqNote_params_init(&tCmnd_Req_Notify);
  tCmnd_Req_Notify.eReq_Notify = CMND_REQUEST_WAKEUP;
  eIneedmd_cmnd_Proto_Request_Notify(&tCmnd_Req_Notify);

  vSeetup_Status_data_structure();

  while(1)
  {
    if(Mailbox_pend(tTIRTOS_cmnd_protocol_mailbox, &tProtocol_msg, BIOS_WAIT_FOREVER) == true)
    {
      switch(tProtocol_msg.eMsg)
      {
        case CMND_MSG_WAKEUP:
        {
          //Perform wakeup procedure
          eIneedmd_radio_request_params_init (&tRadio_request);
          tRadio_request.eRequest = RADIO_REQUEST_POWER_ON;
          tRadio_request.vSetup_state_callback = &vRadio_setup_callback;
          eEC = eIneedmd_radio_request(&tRadio_request);
          if(eEC == ER_NOT_READY)
          {
            Task_sleep(500);
            eIneedmd_cmnd_Proto_ReqNote_params_init(&tCmnd_Req_Notify);
            tCmnd_Req_Notify.eReq_Notify = CMND_REQUEST_WAKEUP;
            eIneedmd_cmnd_Proto_Request_Notify(&tCmnd_Req_Notify);
          }
          else if(eEC == ER_OK)
          {
            // waveform task
            //todo add request for status

            //USB Task
            //todo add request for status

            //power control task is up
            //todo add request for status

            //check the UI is up
            //todo add request for status

            // so all the processes are up
            //todo add request for status

            // check the voltages

            //check the ADC process is up
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_DEVICE_SUPPLY_VOLTAGE;
            tADC_req.uiDevice_supp_voltage = &iSupply_Voltage;
            eEC = eADC_Request(&tADC_req);

            if ( eEC != ER_OK)
            {
              Task_sleep(500);
              eIneedmd_cmnd_Proto_ReqNote_params_init(&tCmnd_Req_Notify);
              tCmnd_Req_Notify.eReq_Notify = CMND_REQUEST_WAKEUP;
              eIneedmd_cmnd_Proto_Request_Notify(&tCmnd_Req_Notify);
            }


            if (iSupply_Voltage < 40 )
            {
              //if good Flash and Buzz


              eIneedmd_UI_params_init(&tUI_Req);
              tUI_Req.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
              tUI_Req.eHeart_led_sequence = HEART_LED_REBOOT;
              tUI_Req.eComms_led_sequence = COMMS_LED_REBOOT;
              tUI_Req.ePower_led_sequence = POWER_LED_REBOOT;
              eIneedmd_UI_request(&tUI_Req);

              Task_sleep(500);

              ePower_Control_Request_params_init(&tPC_req);
              tPC_req.eRequest = PC_REQUEST_FORCE_SHUTDOWN;
              ePower_Control_Request(&tPC_req);
            }

            eEC = eSounder_Request_Params_Init(&tSounder_Req);
            tSounder_Req.eRequest = SOUNDER_REQ_SETUP;
            eEC = eSounder_Request(&tSounder_Req);

            eEC = eSounder_Request_Params_Init(&tSounder_Req);
            tSounder_Req.eRequest = SOUNDER_REQ_TEST;
            eEC = eSounder_Request(&tSounder_Req);



            //tUI_request tUI_request_params;
            eIneedmd_UI_params_init(&tUI_Req);
            tUI_Req.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
            tUI_Req.eHeart_led_sequence = HEART_LED_TEST_PASS;
            tUI_Req.eComms_led_sequence = COMMS_LED_TEST_PASS;
            tUI_Req.ePower_led_sequence = POWER_LED_TEST_PASS;
            eIneedmd_UI_request(&tUI_Req);

            Task_sleep(1000);

            eEC = eSounder_Request_Params_Init(&tSounder_Req);
            tSounder_Req.eRequest = SOUNDER_REQ_HALT;
            eEC = eSounder_Request(&tSounder_Req);
            //if bad shutdown

            //once the green flashes are done

            //all good to go switch on the pairing
            if(eEC == ER_OK)
            {
              eIneedmd_UI_params_init(&tUI_Req);
              tUI_Req.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
              tUI_Req.eComms_led_sequence = COMMS_LED_BLUETOOTH_PAIRING;
              eIneedmd_UI_request(&tUI_Req);
            }

          }
          break;
        }
        case CMND_MSG_SLEEP:
          eIneedmd_radio_request_params_init (&tRadio_request);
          tRadio_request.eRequest = RADIO_REQUEST_POWER_OFF;
          tRadio_request.vSetup_state_callback = &vRadio_setup_callback;
          eEC = eIneedmd_radio_request(&tRadio_request);
          if(eEC == ER_NOT_READY)
          {
            Task_sleep(100);
            eIneedmd_cmnd_Proto_ReqNote_params_init(&tCmnd_Req_Notify);
            tCmnd_Req_Notify.eReq_Notify = CMND_REQUEST_SLEEP;
            eIneedmd_cmnd_Proto_Request_Notify(&tCmnd_Req_Notify);
          }
          break;
        case CMND_MSG_OPEN_PROTOCOL_CONNECTION:
        {
          if(bIs_radio_on == true)
          {
            vDEBUG("Proto cmndr, waiting for proto connection");
            eIneedmd_radio_request_params_init (&tRadio_request);
            tRadio_request.eRequest = RADIO_REQUEST_WAIT_FOR_CONNECTION;
            tRadio_request.uiTimeout = (PROTOCOL_CONN_TIMEOUT_1MIN * 10);
            if(tRadio_request.uiTimeout == PROTOCOL_CONN_TIMEOUT_FOREVER)
            {
              bWait_forever_for_protocol_connection = true;
            }
            else
            {
              bWait_forever_for_protocol_connection = false;
            }
            eEC = eIneedmd_radio_request(&tRadio_request);

            if(eEC == ER_OK)
            {
              eIneedmd_UI_params_init(&tUI_Req);
              tUI_Req.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
              tUI_Req.eComms_led_sequence = COMMS_LED_BLUETOOTH_PAIRING;
              eIneedmd_UI_request(&tUI_Req);
            }
          }
          else
          {
            tProtocol_msg.eMsg = CMND_MSG_WAKEUP;
            Mailbox_post(tTIRTOS_cmnd_protocol_mailbox, &tProtocol_msg, BIOS_WAIT_FOREVER);
          }
          break;
        }
        case CMND_MSG_PROTOCOL_FRAME:
          vDEBUG("Proto cmndr, got proto frame");
          eEC = eParse_Protocol_Frame(tProtocol_msg.uiCmnd_Frame, tProtocol_msg.uiCmnd_Frame_len);

          if(eEC == ER_OK)
          {
            eIneedmd_radio_request_params_init (&tRadio_request);
            tRadio_request.eRequest = RADIO_REQUEST_RECEIVE_FRAME;
            eIneedmd_radio_request(&tRadio_request);
          }

          break;
        case CMND_MSG_INTERFACE_ESTABLISHED:
          break;
        case CMND_MSG_PROTOCOL_INTERFACE_ESTABLISHED:
          //perform protocol interface established procedure
          //
          vDEBUG("Proto cmndr, proto intf estab");
          //Tell the radio to go into receive mode
          //

          eIneedmd_UI_params_init(&tUI_Req);
          tUI_Req.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
          tUI_Req.eComms_led_sequence = COMMS_LED_BLUETOOTH_CONNECTION_SUCESSFUL;
          eIneedmd_UI_request(&tUI_Req);

          eIneedmd_radio_request_params_init (&tRadio_request);
          tRadio_request.eRequest = RADIO_REQUEST_RECEIVE_FRAME;
          eIneedmd_radio_request(&tRadio_request);

          bIs_Status_Clock_Running = Clock_isActive(tProto_Cmndr_status_clock);
          if(bIs_Status_Clock_Running == false)
          {
            Clock_start(tProto_Cmndr_status_clock);
          }

          break;
        case CMND_MSG_PROTOCOL_INTERFACE_CLOSED:
          vDEBUG("Proto cmndr, proto conn closed");
          eIneedmd_radio_request_params_init (&tRadio_request);
          tRadio_request.eRequest = RADIO_REQUEST_WAIT_FOR_CONNECTION;
          tRadio_request.uiTimeout = (PROTOCOL_CONN_TIMEOUT_1MIN * 10);
          tRadio_request.vConnection_status_callback = &vRadio_connection_callback;
          eIneedmd_radio_request(&tRadio_request);
          vDEBUG("Proto cmndr, re-establishing proto conn");

          Clock_stop(tProto_Cmndr_status_clock);

          eIneedmd_UI_params_init(&tUI_Req);
          tUI_Req.uiUI_element = INMD_UI_ELEMENT_COMMS_LED;
          tUI_Req.eComms_led_sequence = COMMS_LED_BLUETOOTH_PAIRING_FAILIED;
          eIneedmd_UI_request(&tUI_Req);

          eIneedmd_UI_params_init(&tUI_Req);
          tUI_Req.uiUI_element = INMD_UI_ELEMENT_SOUNDER;
          tUI_Req.eAlert_sound = ALERT_SOUND_ALERT;
          eIneedmd_UI_request(&tUI_Req);

          break;
        case CMND_MSG_SEND_PERIODIC_STATUS:
          //check if the protocol connection is still active
          if(bIs_protocol_connection == true)
          {
            //the connection is active send a status
            eCommand_send_status();
          }
          else
          {
            //the conneciton is not active, stop the periodic status clock
            Clock_stop(tProto_Cmndr_status_clock);
          }
          break;
        default:
          break;
      }
    }
  }
}

#ifdef PrintCommand
void vCMND_ECHO_FRAME(const unsigned char * uiFrame, uint16_t uiFrame_len)
{
#ifdef DEBUG
  uint16_t uiIndex = 0, uiSend_index = 0;

//  uint8_t uiSend_Frame[256]; //todo: magic number warning!
  char * cSend_Frame = calloc((uiFrame_len * 3), sizeof(char));
  char cHex_format[] = "%.2x ";

  for(uiIndex = 0; uiIndex <= (uiFrame_len - 1); uiIndex++)
  {
    uiSend_index += snprintf((char *)&cSend_Frame[uiSend_index], 512, cHex_format, uiFrame[uiIndex]);
  }

  debug_out((char *)cSend_Frame);

  free(cSend_Frame);
#endif // DEBUG
}
#endif

#endif //__INEEDMD_COMMAND_PROTOCOL_C__
