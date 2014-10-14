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
#include "driverlib/rom.h"
#include "utils_inc/error_codes.h"
#include "board.h"

#include "drivers_inc/battery.h"

#include "app_inc/ineedmd_command_protocol.h"
#include "app_inc/ineedmd_waveform.h"
#include "app_inc/ineedmd_watchdog.h"
#include "app_inc/ineedmd_UI.h"
#include "drivers_inc/ineedmd_bluetooth_radio.h"
#include "utils_inc/proj_debug.h"

#include "app_inc/ineedmd_power_modes.h"

#include "utils_inc/osal.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//Protocol frame defines
#define PROTOCOL_FRAME_MAX_LEN          128
#define PROTOCOL_FRAME_START_BYTE       0xC9
#define PROTOCOL_FRAME_END_BYTE         0x9C
#define PROTOCOL_FRAME_ACK_LEN          4
#define PROTOCOL_FRAME_NACK_LEN         4
#define PROTOCOL_FRAME_CMND_TYPE_INDEX  1
#define PROTOCOL_FRAME_CMND_LEN_INDEX   2
#define PROTOCOL_FRAME_CMND_ID_INDEX    3

//protocol connection defines
#define PROTOCOL_CONN_TIMEOUT_1MIN    60000

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

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
bool bIs_protocol_connection = false;
bool bDid_radio_rfd = false;
bool bDid_radio_send_frame = false;

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
  CMND_MSG_PROTOCOL_FRAME,
  CMND_MSG_INTERFACE_ESTABLISHED,
  CMND_MSG_PROTOCOL_INTERFACE_ESTABLISHED,
  CMND_MSG_PROTOCOL_INTERFACE_CLOSED,
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
  COMMAND_ID_REQUEST_TO_TEST = 0x18
}eINMD_protocol_command_id;

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
extern Mailbox_Handle tTIRTOS_cmnd_protocol_mailbox;

typedef struct tProtocol_msg_struct
{
  eINDM_Cmnd_Proto_msg eMsg;
  uint8_t   uiCmnd_Frame[PROTOCOL_FRAME_MAX_LEN];
  uint32_t  uiCmnd_Frame_len;
}tProtocol_msg_struct;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

//extern void check_for_update(void);
/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
void vRadio_connection_callback(eRadio_connection_state eRadio_conn);
void vRadio_change_settings_callback(eRadio_Settings eRadio_Setting);
void vRadio_sent_frame(uint32_t uiCount);
ERROR_CODE eIneedmd_send_ack(void);  //send protocol acknowledgement
ERROR_CODE eIneedmd_send_nack(void);  //send protocol fail acknowledgement
ERROR_CODE eParse_Protocol_Frame(void *pt);
ERROR_CODE eValidatePacketType(eINMD_Cmnd_packet_type eProtocolFrameType);
ERROR_CODE eValidateCommandID(eINMD_Cmnd_packet_type eProtocolFrameType, eINMD_protocol_command_id szRecCommand);

////int debug_out(char * cOut_buff,...);
//void initACKNACK(void);

//void writeDataToPort(unsigned char * cOut_buff, uint16_t uiLen);
ERROR_CODE eCheckAlarmBytes(const unsigned char *Frame, int noPosh, int *ackFlag);
ERROR_CODE eCheckTimeBytes(const unsigned char *Frame, int noPosh, int *ackFlag);

//void ParseFrame(void *pt);
//#ifdef PrintCommand
//  void vCMND_ECHO_FRAME(const unsigned char * uiFrame, uint16_t uiFrame_len);
//#endif

/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
void vRadio_connection_callback(eRadio_connection_state eRadio_conn)
{
  tINMD_protocol_req_notify tCmnd_Req_Notify;
  ERROR_CODE eEC = ER_FAIL;
  eIneedmd_cmnd_Proto_ReqNote_params_init(&tCmnd_Req_Notify);

  if(eRadio_conn == RADIO_CONN_NONE)
  {
    tCmnd_Req_Notify.eReq_Notify = CMND_REQUEST_WAKEUP;
    eEC = ER_OK;
  }
  else if(eRadio_conn == RADIO_CONN_CONNECTED)
  {
    bIs_protocol_connection = true;
  }

  if(eEC == ER_OK)
  {
    eIneedmd_cmnd_Proto_Request_Notify(&tCmnd_Req_Notify);
  }
  return;
}

void vRadio_change_settings_callback(eRadio_Settings eRadio_Setting)
{
  if(eRadio_Setting == RADIO_SETTINGS_RFD)
  {
    bDid_radio_rfd = true;
  }else{/*do nothing*/}

  return;
}

void vRadio_sent_frame(uint32_t uiCount)
{
  bDid_radio_send_frame = true;

  return;
}

//void initACKNACK(void)
//{
//  unsigned const char NAK[4] = { 0x9C, 0xFF, 0x04, 0xC9 };
//  memset(NACK, 0x00, NACK_FRAME_SIZE);
//  memset(ACK, 0x00, ACK_FRAME_SIZE);
//  sprintf(NACK, "%.2X %.2X %.2X %.2X", NAK[0], NAK[1], NAK[2], NAK[3]);
//  sprintf(ACK, "%.2X %.2X %.2X %.2X", 0x9C, 0x00, 0x04, 0xC9);
//}

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
  eEC = eIneedmd_radio_request_params_init (&tRequest);

  if(eEC == ER_OK)
  {
    bDid_radio_send_frame = false;

    tRequest.eRequest = RADIO_REQUEST_SEND_FRAME;
    tRequest.vBuff = ACK_;
    tRequest.uiBuff_size = PROTOCOL_FRAME_ACK_LEN;
    tRequest.eTX_Priority = TX_PRIORITY_QUEUE;
    tRequest.vBuff_sent_callback = vRadio_sent_frame;
    eEC = eIneedmd_radio_request(&tRequest);
  }else{/*do nothing*/}

  while(bDid_radio_send_frame == false)
  {
    Task_sleep(100);
  }

  vDEBUG("ACK\n");

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
  uint8_t NACK_[4] = {0x9c, 0xFF, 0x04, 0xC9};
  tRadio_request tRequest;
  eEC = eIneedmd_radio_request_params_init (&tRequest);

  if(eEC == ER_OK)
  {
    tRequest.eRequest = RADIO_REQUEST_SEND_FRAME;
    tRequest.vBuff = NACK_;
    tRequest.uiBuff_size = PROTOCOL_FRAME_NACK_LEN;
    tRequest.eTX_Priority = TX_PRIORITY_QUEUE;
    tRequest.vBuff_sent_callback = vRadio_sent_frame;
    eEC = eIneedmd_radio_request(&tRequest);

    if(eEC == ER_OK)
    while(bDid_radio_send_frame == false)
    {
      //todo: need timeout
      Task_sleep(1);
    }

  }else{/*do nothing*/}

  printf("NACK\n");

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
    tRequest.vBuff = cOut_buff;
    tRequest.uiBuff_size = uiLen;
    tRequest.eTX_Priority = TX_PRIORITY_QUEUE;
    tRequest.vBuff_sent_callback = vRadio_sent_frame;
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

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
ERROR_CODE eCheckAlarmBytes(const unsigned char *Frame, int noPosh, int *ackFlag)
{
  if (noPosh)
  {
    if ((0x00 == Frame[13]) && (0x00 == Frame[14]))
    {
      //indicates the alarm off combination.
      //send the acknowledgement for that.
      printf("\nReceived EXCLUSIVE request for ALARM OFF...");
      //writeDataToPort(ACK);
      (*ackFlag)++;
    }
    else
    {
      //when the interpretation of further combinations is procured, this will be filled.
    }
  }
  else
  {
    //indicates that this is a combined request for 1 of POSH and alarm
    if ((0x00 == Frame[13]) && (0x00 == Frame[14]))
    {
      //indicates the alarm off combination.
      //send the acknowledgement for that.
      printf("\nReceived COMBINED request for ALARM OFF...");
      //writeDataToPort(ACK);
      (*ackFlag)++;
    }
    else
    {
      //when the interpretation of further combinations is procured, this will be filled.
    }

  }
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
ERROR_CODE eCheckTimeBytes(const unsigned char *Frame, int noPosh, int *ackFlag)
{
  //bytes 9,10,11,12 indicate the time to be set (counting Frame from 0)
  printf("\nTime to set: %X %X %X %X", Frame[9], Frame[10], Frame[11], Frame[12]);
  //("\nSending Acknowledgement...");
  //writeDataToPort(ACK);
  *ackFlag = *ackFlag + 1;
}

/******************************************************************************
* name: eParse_Protocol_Frame
* description:
* param description: uint8_t - pointer: pointer to the buffer containing the protocol frame
* return value description: ERROR_CODE - ER_OK:
******************************************************************************/
ERROR_CODE eParse_Protocol_Frame(void *pt)
{
  ERROR_CODE eEC = ER_FAIL;
  const unsigned char status0x14[] = { 0x9C, 0x05, 0x17, 0x02, 0x00, 0x01, 0xD7, 0x68, 0x56, 0x00, 0x00, 0x00, 0x14, 0x00, 0x02, 0xD7, 0x68, 0xAA, 0x00, 0x00, 0x00, 0x60, 0xC9 }; //reply for high voltage query.
  const unsigned char status0x17[] = { 0x9c, 0x03, 0x18, 0x16, 0x01, 0x03, 0x1B, 0x10, 0x00, 0x0A, 0x10, 0x9C, 0x22, 0xC9, 0x3C, 0xC9, 0xA5, 0x0E, 0x0B, 0x2C, 0x8C, 0x9C, 0x77, 0xC9 };
  const unsigned char status0x15_Temp[] = { 0x9c, 0x05, 0x17, 0x02, 0x00, 0x01, 0xd7, 0x68, 0x56, 0x00, 0x00, 0x00, 0x14, 0x00, 0x02, 0xd7, 0x68, 0xAA, 0x00, 0x00, 0x00, 0x60, 0xc9 };
  unsigned char * ucRaw_Frame = pt;
  unsigned char OutGoingPacket[0x20];  //maximum length of an outgoing packet is 0x20
  uint16_t OutGoingPacket_len = 0;  //maximum length of an outgoing packet is 0x20
  unsigned char             ucProtocol_Frame[256];
  eINMD_Cmnd_packet_type    eProtocolFrameType;//dataPacketType;// = ucProtocol_Frame[1];
  unsigned char             ucProtocolFrameLength;//lengthPacket;  // = ucProtocol_Frame[2];
  eINMD_protocol_command_id eProtocolFrameCommandID;//actCommand;    // = ucProtocol_Frame[3];
  char replyToSend[100];
  int noPosh = 0; //this is activated to indicate that the datagram received with parent command as 0x12, has no request related to hib, pow, sleep, on. it is just related to Off Alarm.
  int ackFlag = 0;
  unsigned char cntDataBytes;// = lengthPacket - 5;
  INMD_LED_COMMAND eInmd_LED_cmnd = INMD_LED_CMND_LED_OFF;
  tUI_request tUI_request_params;
  tRadio_request tRadio_request_Params;

  //Copy the recieved protocol frame into a local buffer
  ucProtocolFrameLength = ucRaw_Frame[PROTOCOL_FRAME_CMND_LEN_INDEX];
  memcpy(ucProtocol_Frame, pt, ucProtocolFrameLength);

  eProtocolFrameType      = (eINMD_Cmnd_packet_type)   ucProtocol_Frame[PROTOCOL_FRAME_CMND_TYPE_INDEX];
  eProtocolFrameCommandID = (eINMD_protocol_command_id)ucProtocol_Frame[PROTOCOL_FRAME_CMND_ID_INDEX];

  cntDataBytes = ucProtocolFrameLength - 5;

  eEC = eValidatePacketType(eProtocolFrameType);
//  if (0 <= ValidatePacketType(eProtocolFrameType))
  if(eEC == ER_OK)
  {
    eEC = eValidateCommandID(eProtocolFrameType, eProtocolFrameCommandID);
//    if (-1 == SearchArrCmd(eProtocolFrameType, actCommand))
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
      //when the control comes here, it is implicitly implied that the packetType and also the actual command are very much valid.
      switch (eProtocolFrameCommandID)
      {
      case 0x12: //status
        /*
        0th byte --> start Frame
        1st byte --> datagram packet
        2nd byte --> length
        3rd byte --> actual command, which in this case is 0x12
        4th byte --> Firmware version
        5th byte --> battery voltage
        6th byte --> operating mode (hibernate, sleep, on or highpower) all have been defined as macros.
        */
        switch (ucProtocol_Frame[6])
        {
        case STATUS_HIBERNATE:
          printf("\nReceived request for HIBERNATE...");
          //PrintCommand(ucProtocol_Frame, frameCnt+1);
          //writeDataToPort(ACK);
          break;
        case STATUS_SLEEP:
          printf("\nReceived request for SLEEP...");
          //PrintCommand(ucProtocol_Frame, frameCnt+1);
          //writeDataToPort(ACK);
          break;
        case STATUS_ON:
          printf("\nReceived request for ON...");
          //PrintCommand(ucProtocol_Frame, frameCnt+1);
          //writeDataToPort(ACK);
          break;
        case STATUS_POWER:
          printf("\nReceived request for POWER...");
          //PrintCommand(ucProtocol_Frame, frameCnt+1);
          //writeDataToPort(ACK);
          break;
        default:
          /*printf("\nNot a valid status request! Sending NACK!");
          writeDataToPort(NACK);*/
          noPosh = 1;
          break;
        }
        //Processing the same frame for the alarm bytes
        eCheckAlarmBytes(ucProtocol_Frame, noPosh, &ackFlag);
        eCheckTimeBytes(ucProtocol_Frame, noPosh, &ackFlag); //in both these functions, ackFlag will act as __out__ parameter.
        if (2 == ackFlag) // ackFlag will be hardcoded 2, because both these above functions increment it.
        {
          printf("\nSending Acknowledgement...");
          eIneedmd_send_ack();
          //reset the ackFlag to 0 for further processing...
          ackFlag = 0;
        }
        break;
      case 0x16: //capture data set
//        printf("\nCapture dataset... \nSending Acknowledgement...");
        printf("Capture dataset...");
        printf("Sending Acknowledgement...");
        eIneedmd_send_ack();
        //todo: need capture code
        break;
      case 0x13: //get data set info
        printf("\nReceived request for data set info...");
        sprintf(replyToSend, "%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X ",
          status0x14[0], status0x14[1], status0x14[2],
          status0x14[3], status0x14[4], status0x14[5],
          status0x14[6], status0x14[7], status0x14[8],
          status0x14[9], status0x14[10], status0x14[11],
          status0x14[12], status0x14[13], status0x14[14],
          status0x14[15], status0x14[16], status0x14[17],
          status0x14[18], status0x14[19], status0x14[20],
          status0x14[21], status0x14[22]);
        PrintCommand(status0x14, 23);
        printf("\nSending information about Data Sets...");
        //todo: this needs to send hex not hex converted to ascii
        //writeDataToPort(replyToSend);


        break;
      case 0x14://data set transfer command  --  NAK or (ACK + respective data set corresponding to the id sent). For now, ACK.
        //have to remove this hardcoded method in printing though.. later..
        //we are sending the measurement dataset as a response to this command. Length of reply packet is 0x18 ie. 24
        //hence we have to send status0x17[] as reply.
        //minimum data of 4 bytes assumed. Confirm this later.
        if (cntDataBytes<4)
        {
          printf("\nData Set to transfer not mentioned!");
          printf("\nSending NACK!");
          eIneedmd_send_nack();
        }
        else
        {
          printf("\nReceived request for DataSet Transfer, with dataSetID: %X %X.\nSending Acknowledgement...", ucProtocol_Frame[7], ucProtocol_Frame[8]);
          //writeDataToPort(ACK);
          /*sprintf(replyToSend, "%X %X %X %X %X %X %X %X %X %X %X %X %X %X",
          status0x15[0], status0x15[1], status0x15[2], status0x15[3],
          status0x15[4], status0x15[5], status0x15[6], status0x15[7],
          status0x15[8], status0x15[9], status0x15[10], status0x15[11],
          status0x15[12], status0x15[13]);*/

          sprintf(replyToSend, "%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X ",
            status0x17[0], status0x17[1], status0x17[2], status0x17[3],
            status0x17[4], status0x17[5], status0x17[6], status0x17[7],
            status0x17[8], status0x17[9], status0x17[10], status0x17[11],
            status0x17[12], status0x17[13], status0x17[14], status0x17[15], status0x17[16], status0x17[17],
            status0x17[18], status0x17[19], status0x17[20], status0x17[21], status0x17[22], status0x17[23]);

          //todo: this needs to send hex not hex converted to ascii
//          writeDataToPort(replyToSend);

          printf("\nSending transfer response...");
          PrintCommand(status0x17, 24);
        }
        break;
      case 0x15://erase data set
        //minimum data of say around 4 bytes is assumed. Confirm this.
        if (cntDataBytes<4)
        {
          printf("\nData Set to transfer not mentioned!");
          printf("\nSending NACK!");
          eIneedmd_send_nack();
        }
        else
        {
          printf("\nReceived request for erasing dataset...\nSending Acknowledgement...");
          eIneedmd_send_ack();
          printf("\nErasing data set with ID: %X %X ", ucProtocol_Frame[7], ucProtocol_Frame[8]);
          printf("\nAfter Deletion, sending...");
          PrintCommand(status0x15_Temp, 23);
          sprintf(replyToSend, "%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X ",
            status0x15_Temp[0], status0x15_Temp[1], status0x15_Temp[2], status0x15_Temp[3], status0x15_Temp[4],
            status0x15_Temp[5], status0x15_Temp[6], status0x15_Temp[7], status0x15_Temp[8], status0x15_Temp[9],
            status0x15_Temp[10], status0x15_Temp[11], status0x15_Temp[12], status0x15_Temp[13], status0x15_Temp[14],
            status0x15_Temp[15], status0x15_Temp[16], status0x15_Temp[17], status0x15_Temp[18], status0x15_Temp[19],
            status0x15_Temp[20], status0x15_Temp[21], status0x15_Temp[22]);
          //todo: this needs to send hex not hex converted to ascii
//          writeDataToPort(replyToSend);
        }
        break;
      case 0x11://get status.
//        printf("\nReceived request for status...\nSending status record...");
        printf("Received request for status...");
        printf("Building Status Packet...");
        memset(OutGoingPacket, 0x00, 0x20); //todo: magic numbers!
        OutGoingPacket_len = 0;
        OutGoingPacket[OutGoingPacket_len++] = 0x9c;
        OutGoingPacket[OutGoingPacket_len++] = 0x02;
        OutGoingPacket[OutGoingPacket_len++] = 0x12;
        OutGoingPacket[OutGoingPacket_len++] = 0x04; // hard coding the version number
        OutGoingPacket[OutGoingPacket_len++] = 0x00; //todo: implement battery voltage function ineedmd_get_battery_voltage();
        OutGoingPacket[OutGoingPacket_len++] = (char) (0xff & 0x00);//todo: implement unit temp function ineedmd_get_unit_tempoerature());

        OutGoingPacket[OutGoingPacket_len++] = 0x00;  // operating mode  - no information to add yet
        OutGoingPacket[OutGoingPacket_len++] = 0x00;  // capture settings 1  - no information to add yet
        OutGoingPacket[OutGoingPacket_len++] = 0x00;  // capture settings 2  - no information to add yet
        OutGoingPacket[OutGoingPacket_len++] = 0x00;  // Time 4 - no information to add yet
        OutGoingPacket[OutGoingPacket_len++] = 0x00;  // Time 3 - no information to add yet
        OutGoingPacket[OutGoingPacket_len++] = 0x00;  // Time 2 - no information to add yet
        OutGoingPacket[OutGoingPacket_len++] = 0x00;  // Time 1 - no information to add yet
        OutGoingPacket[OutGoingPacket_len++] = 0x00;  // Alarm 4  - no information to add yet
        OutGoingPacket[OutGoingPacket_len++] = 0x00;  // Alarm 3  - no information to add yet
        OutGoingPacket[OutGoingPacket_len++] = 0x00;  // Alarm 2  - no information to add yet
        OutGoingPacket[OutGoingPacket_len++] = 0x00;  // Alarm 1  - no information to add yet

        OutGoingPacket[OutGoingPacket_len++] = 0xc9;

        sprintf(replyToSend, "%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %x",
            OutGoingPacket[0x00],
              OutGoingPacket[0x01],
              OutGoingPacket[0x02],
              OutGoingPacket[0x03],
              OutGoingPacket[0x04],
              OutGoingPacket[0x05],

              OutGoingPacket[0x06],
              OutGoingPacket[0x07],
              OutGoingPacket[0x08],
              OutGoingPacket[0x09],
              OutGoingPacket[0x0a],
              OutGoingPacket[0x0b],
              OutGoingPacket[0x0c],
              OutGoingPacket[0x0d],
              OutGoingPacket[0x0e],
              OutGoingPacket[0x0f],
              OutGoingPacket[0x10],

              OutGoingPacket[0x11]);
        PrintCommand(OutGoingPacket, OutGoingPacket[2]); //this is just to display on our side, what reply we are sending.
        printf("Sending status record...");

        if(OutGoingPacket_len > 0x20)
        {
          printf("Packet len > 0x20! SYS HALT");
          while(1){};
        }else{/*do nothing*/}
        writeDataToPort(OutGoingPacket, OutGoingPacket_len);  //todo magic number!
        break;
      case 0x17:
//todo: frameCnt was not set properly, this code to be implemented later
//        switch (ucProtocol_Frame[frameCnt - 1])
//        {
//        case 0xFF: //true
//          isRealStream = 1;
//          counter = 0;
//          printf("\nBegin real-time streaming -- TRUE... ");
//          //writeDataToPort(ACK);
//          printf("\nReceived request for Real time DataSet Transfer... ");
//          PrintCommand(status0x17, 24);
//          sprintf(replyToSend, "%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X ",
//            status0x17[0], status0x17[1], status0x17[2], status0x17[3],
//            status0x17[4], status0x17[5], status0x17[6], status0x17[7],
//            status0x17[8], status0x17[9], status0x17[10], status0x17[11],
//            status0x17[12], status0x17[13], status0x17[14], status0x17[15], status0x17[16], status0x17[17],
//            status0x17[18], status0x17[19], status0x17[20], status0x17[21],
//            status0x17[22], status0x17[23]);
//          counter = 0;
//          while (isRealStream)
//          {
//
//            writeDataToPort(replyToSend);
//            /*if (WriteFile(hSerial, replyToSend, 24, &dwBytesWritten, NULL))
//            {
//              printf("\n Sending Real time data (%d) to COM port... ", ++counter);
//              printf("\nTotal Bytes Written (Live Streaming): %d", dwBytesWritten);
//              continue;
//            }
//            else
//            {
//              printf("\nWriteFile failed: %d", GetLastError());
//            }*/
//
//            //Sleep(1500); //this thread will sleep and continue it's work.
//          }
//          break;
//        case 0x00: //false
//            isRealStream = 0;
//            printf("\nStop real-time streaming....");
//          //writeDataToPort(ACK);
//            break;
//          /*default:
//          printf("\nMissing argument to indicate beginning of streaming real time! Sending NACK!");
//          writeDataToPort(NACK);*/
//            break;
//        default:
//            printf("\nMissing argument 'TRUE/FALSE'!");
//            printf("\nSending NACK!");
//            writeDataToPort(NACK);
//            break;
//        }
        printf("Parse cmnd SYS HALT, ACT command 0x17 not implemented yet");
        while(1){};
        break;

      case REQUEST_TO_TEST://request to send test signal.
        //printf("Received Request to Test");
        if(ucProtocol_Frame[4] == EKG_TEST_PAT)
        {
          printf("Received \"test\" request...");
          //todo: implement enable test signal function
//          iIneedmd_waveform_enable_TestSignal();
        }
        else if(ucProtocol_Frame[4] == LED_TEST_PATTERN)
        {
          eEC = eIneedmd_UI_params_init(&tUI_request_params);
          printf("Received LED test request");
          eInmd_LED_cmnd = (INMD_LED_COMMAND)ucProtocol_Frame[5];
          switch(eInmd_LED_cmnd)
          {
            case INMD_LED_CMND_LED_OFF:
              printf("LED_OFF");
              tUI_request_params.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED | INMD_UI_ELEMENT_POWER_LED);
              tUI_request_params.eHeart_led_sequence = HEART_LED_UI_OFF;
              tUI_request_params.eComms_led_sequence = COMMS_LED_UI_OFF;
              tUI_request_params.ePower_led_sequence = POWER_LED_UI_OFF;
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_POWER_ON_BATT_LOW:
              printf("POWER_ON_BATT_LOW");

              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_POWER_ON_BATT_GOOD:
              printf("POWER_ON_BATT_GOOD");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_BATT_CHARGING:
              printf("BATT_CHARGING");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_BATT_CHARGING_LOW:
              printf("BATT_CHARGING_LOW");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_LEAD_LOOSE:
              printf("LEAD_LOOSE");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_LEAD_GOOD_UPLOADING:
              printf("LEAD_GOOD_UPLOADING");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_DIG_FLATLINE:
              printf("DIG_FLATLINE");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_BT_CONNECTED:
              printf("BT_CONNECTED");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_BT_ATTEMPTING:
              printf("BT_ATTEMPTING");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_BT_FAILED:
              printf("BT_FAILED");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_USB_CONNECTED:
              printf("USB_CONNECTED");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_USB_FAILED:
              printf("USB_FAILED");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_DATA_TRANSFER:
              printf("DATA_TRANSFER");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_TRANSFER_DONE:
              printf("TRANSFER_DONE");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_STORAGE_WARNING:
              printf("STORAGE_WARNING");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_ERASING:
              printf("ERASING");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_ERASE_DONE:
              printf("ERASE_DONE");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_DFU_MODE:
              printf("DFU_MODE");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_MV_CAL:
              printf("MV_CAL");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_TRI_WVFRM:
              printf("TRI_WVFRM");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_REBOOT:
              printf("REBOOT");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_HIBERNATE:
              printf("HIBERNATE");
              //TODO: add battery state check
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              //todo: implement battery check function
//              check_battery();
              break;
            case INMD_LED_CMND_LEADS_ON:
              printf("LEADS_ON");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_MEMORY_TEST:
              printf("MEMORY_TEST");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_COM_BUS_TEST:
              printf("COM_BUS_TEST");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_CPU_CLOCK_TEST:
              printf("CPU_CLOCK_TEST");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_FLASH_TEST:
              printf("FLASH_TEST");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            case INMD_LED_CMND_TEST_PASS:
              printf("TEST_PASS");
              eEC =  eIneedmd_UI_request(&tUI_request_params);
              break;
            default:
              printf("Unrecongnized Sequence - No Soup For You");
              eEC = ER_FAIL;
              break;
          }

          //Check if the UI request was successful
          if(eEC == ER_OK)
          {
            eIneedmd_send_ack();
          }
          else
          {
            eIneedmd_send_nack();
          }
        }
        else if(ucProtocol_Frame[4] == STOP_EKG_TEST_PAT)
        {
          printf("Received \"test\" terminate...");
          //todo implement disable waveform test signal
//          iIneedmd_waveform_disable_TestSignal();
        }
        else if(ucProtocol_Frame[4] == REQ_FOR_DFU)
        {
          eEC = eIneedmd_UI_params_init(&tUI_request_params);
          eIneedmd_UI_params_init(&tUI_request_params);
          printf("Performing DFU...");
          eIneedmd_send_ack();
          //todo: add new DFU sequence enum to the params
          eIneedmd_UI_request(&tUI_request_params);
          Task_sleep(500);

          //todo: implement check for update function
//          check_for_update();
          //todo: add new REBOOT sequence enum to the params
          eIneedmd_UI_request(&tUI_request_params);
          Task_sleep(500);
          ineedmd_watchdog_doorbell();
        }
        else if(ucProtocol_Frame[4] == REQ_FOR_RESET)
        {
          eIneedmd_radio_request_params_init(&tRadio_request_Params);
          printf("Performing RESET..");
          //give the watchdog a long timeout timer
          ineedmd_watchdog_feed();

          //todo: add new RESTART sequence enum to the params
          eIneedmd_UI_request(&tUI_request_params);

          //set the radio to factory defaults
          tRadio_request_Params.eRequest = RADIO_REQUEST_CHANGE_SETTINGS;
          tRadio_request_Params.eSetting = RADIO_SETTINGS_RFD;
          tRadio_request_Params.vChange_setting_callback = vRadio_change_settings_callback;
          eEC = eIneedmd_radio_request(&tRadio_request_Params);
          if(eEC == ER_OK)
          {
            while(bDid_radio_rfd == false)
            {
              Task_sleep(500);
            }
          }else{/*do nothing*/}

          if(eEC == ER_OK)
          {
            eIneedmd_send_ack();
            //Reset the system
            ineedmd_watchdog_doorbell();

            //Should never get there, loop just in case
            while(1){};
          }
          else
          {
            eIneedmd_send_nack();
          }
        }
        //the output expected is TEST DATA PATTERN 1
//        printf("\nReceived \"test\" request...");

//        printf("\nSending pattern 1...");
//        printf("Sending pattern 1...");
//        sprintf(replyToSend, "test data <pattern-1>");//this is temporary, and is going to be updated by the actual packet, which is not known to us at this point.
//        writeDataToPort(replyToSend);
        else
        {
          break;
        }
      printf("Done");
      }
    }
  }
  else
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
      if((szRecCommand == COMMAND_ID_REQUEST_STATUS)    |\
         (szRecCommand == COMMAND_ID_SET_STATUS)        |\
         (szRecCommand == COMMAND_ID_GET_DATA_SET_INFO) |\
         (szRecCommand == COMMAND_ID_DATA_SET_TRANSFER) |\
         (szRecCommand == COMMAND_ID_ERASE_DATA_SET)    |\
         (szRecCommand == COMMAND_ID_CAPTURE_DATA_SET)  |\
         (szRecCommand == COMMAND_ID_STREAM_DATA)       |\
         (szRecCommand == COMMAND_ID_REQUEST_TO_TEST))
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
     (ptParams->uiCmnd_Frame != NULL)             |\
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
        break;
      case CMND_REQUEST_RCV_PROTOCOL_FRAME:
        //Check if the frame length is greather then the max frame length size
        if(ptParams->uiFrame_Len > PROTOCOL_FRAME_MAX_LEN)
        {
          //Frame length exceedes limit
          //
          eEC = ER_SIZE;
#ifdef DEBUG
          vDEBUG("eIneedmd_cmnd_Proto_Request_Notify SYS_HALT, proto frame size to large");
          while(1){};
#endif
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
        break;
      case CMND_NOTIFY_PROTOCOL_INTERFACE_CLOSED:
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
  }else{/*do nothing*/}

  return eEC;
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
  uint32_t uiMsg_size = sizeof(tProtocol_msg_struct);
  uint32_t uiMbox_size = 0;
  tINMD_protocol_req_notify tCmnd_Req_Notify;

  uiMbox_size = Mailbox_getMsgSize(tTIRTOS_cmnd_protocol_mailbox);
  if(uiMsg_size != uiMbox_size)
  {
    vDEBUG("vIneedmd_command_task SYS HALT, invalid mailbox msg size!");
    while(1){};
  }else{/*do nothing*/}

  eIneedmd_cmnd_Proto_ReqNote_params_init(&tCmnd_Req_Notify);
  tCmnd_Req_Notify.eReq_Notify = CMND_REQUEST_WAKEUP;
  eIneedmd_cmnd_Proto_Request_Notify(&tCmnd_Req_Notify);

  while(1)
  {
    if(Mailbox_pend(tTIRTOS_cmnd_protocol_mailbox, &tProtocol_msg, BIOS_WAIT_FOREVER) == true)
    {
      switch(tProtocol_msg.eMsg)
      {
        case CMND_MSG_WAKEUP:
          eIneedmd_radio_request_params_init (&tRadio_request);
          tRadio_request.eRequest = RADIO_REQUEST_WAIT_FOR_CONNECTION;
          tRadio_request.uiTimeout = PROTOCOL_CONN_TIMEOUT_1MIN;
          tRadio_request.vConnection_status_callback = &vRadio_connection_callback;
          eEC = eIneedmd_radio_request(&tRadio_request);
          if(eEC == ER_NOT_READY)
          {
            Task_sleep(500);
            eIneedmd_cmnd_Proto_ReqNote_params_init(&tCmnd_Req_Notify);
            tCmnd_Req_Notify.eReq_Notify = CMND_REQUEST_WAKEUP;
            eIneedmd_cmnd_Proto_Request_Notify(&tCmnd_Req_Notify);
          }
          break;
        case CMND_MSG_SLEEP:
          break;
        case CMND_MSG_PROTOCOL_FRAME:
          eParse_Protocol_Frame(&tProtocol_msg.uiCmnd_Frame);
          break;
        case CMND_MSG_INTERFACE_ESTABLISHED:
          break;
        case CMND_MSG_PROTOCOL_INTERFACE_ESTABLISHED:
          break;
        case CMND_MSG_PROTOCOL_INTERFACE_CLOSED:
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

  uint8_t uiSend_Frame[512]; //todo: magic number warning!
  char cHex_format[] = "%.2x ";
  if((uiFrame_len * 4) > 512)
  {
    return;
  }

  memset(uiSend_Frame, 0x00, 512);

  for(uiIndex = 0; uiIndex <= (uiFrame_len - 1); uiIndex++)
  {
    uiSend_index += snprintf((char *)&uiSend_Frame[uiSend_index], 512, cHex_format, uiFrame[uiIndex]);
  }

  debug_out((char *)uiSend_Frame);
#endif // DEBUG
}
#endif

#endif //__INEEDMD_COMMAND_PROTOCOL_C__
