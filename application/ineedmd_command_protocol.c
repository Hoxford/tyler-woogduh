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
//*****************************************************************************
// includes
//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "driverlib/rom.h"
#include "utils_inc/error_codes.h"
#include "board.h"

#include "battery.h"

#include "app_inc/ineedmd_command_protocol.h"
#include "app_inc/ineedmd_waveform.h"
#include "app_inc/ineedmd_watchdog.h"
#include "app_inc/ineedmd_UI.h"
#include "ineedmd_bluetooth_radio.h"
#include "utils_inc/proj_debug.h"

#include "app_inc/ineedmd_power_modes.h"

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

//*****************************************************************************
// variables
//*****************************************************************************
static uint8_t uiINMD_Protocol_frame[INMD_FRAME_BUFF_SIZE];
//static uint16_t uiINMD_Protocol_frame_len = 0;

static bool bIs_protocol_frame = false;

//static const int dataApplicable[7] = { 0, 1, 0, 1, 1, 1, 1 };
static char NACK[NACK_FRAME_SIZE]; //this is a string
static char ACK[ACK_FRAME_SIZE]; //this too is a string
//static const char status0X11[] = { 0x9C, 0x01, 0x11, 0x01, 0x64, 0x50, 0x0B, 0xB8, 0xD7, 0x68, 0x56, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xC9 }; //reply for high voltage query.
//static const char status0x13[] = { 0x9C, 0x04, 0x20, 0x21, 0x22,0xC9 };
//static const char status0x14[] = { 0x9C, 0x04, 0x20, 0x21, 0x22, 0xC9 };
//static const char status0x15[] = { 0x9C, 0x03, 0x0E, 0x16, 0x03, 0x10, 0x0A, 0x9C, 0xC9, 0xC9, 0x0E, 0x2C, 0x9C, 0xC9 };

//*****************************************************************************
// external variables
//*****************************************************************************
extern unsigned char ledState;
//*****************************************************************************
// enums
//*****************************************************************************

//*****************************************************************************
// structures
//*****************************************************************************

//*****************************************************************************
// external functions
//*****************************************************************************
extern void check_for_update(void);
//*****************************************************************************
// function declarations
//*****************************************************************************

//int debug_out(char * cOut_buff,...);
//int SearchArrCmd(char cArrCmd);
void initACKNACK(void);
int SearchArrCmd(const unsigned char dataPacketType, const unsigned char szRecCommand);
void writeDataToPort(unsigned char * cOut_buff, uint16_t uiLen);
void CheckAlarmBytes(const unsigned char *Frame, int noPosh, int *ackFlag);
void CheckTimeBytes(const unsigned char *Frame, int noPosh, int *ackFlag);
int ValidatePacketType(const unsigned char dataPacketType);
void ParseFrame(void *pt);
#ifdef PrintCommand
  void vCMND_ECHO_FRAME(const unsigned char * uiFrame, uint16_t uiFrame_len);
#endif

//*****************************************************************************
// functions
//*****************************************************************************

void initACKNACK(void)
{
  unsigned const char NAK[4] = { 0x9C, 0xFF, 0x04, 0xC9 };
  memset(NACK, 0x00, NACK_FRAME_SIZE);
  memset(ACK, 0x00, ACK_FRAME_SIZE);
  sprintf(NACK, "%.2X %.2X %.2X %.2X", NAK[0], NAK[1], NAK[2], NAK[3]);
  sprintf(ACK, "%.2X %.2X %.2X %.2X", 0x9C, 0x00, 0x04, 0xC9);
}

int ineedmd_send_ack()
{
  uint8_t ACK_[4] = {0x9c, 0x00, 0x04, 0xC9};
  ineedmd_radio_send_frame(ACK_, 4);
  printf("ACK\n");
  //needed for android
  //ineedmd_radio_send_string("\r", strlen("\r"));
  return 1;
}


int ineedmd_send_nack()
{
  uint8_t NACK_[4] = {0x9c, 0xFF, 0x04, 0xC9};
  ineedmd_radio_send_frame(NACK_, 4);
  printf("NACK\n");
  //needed for android
  //ineedmd_radio_send_string("\r", strlen("\r"));
  return 1;
}
//todo: define this
//int SearchArrCmd(char cArrCmd)
//{
//  //todo: get devins fcn def in heres
//  return 1;
//}
int SearchArrCmd(const unsigned char dataPacketType, const unsigned char szRecCommand)
{
  const char arrCommand[] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 };
  int i;

  switch (dataPacketType)
  {
  case 0x01:
    //indicates this is the COMMAND packet and hence the szRecCommand is going to be holding the actual command.
    for (i = 0; i < 8; i++)
    {
      if (arrCommand[i] == szRecCommand)
        return i; //i will be holding the index.
    }
    break;
  }
  return -1;
}


//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void writeDataToPort(unsigned char * cOut_buff, uint16_t uiLen)
{

  ineedmd_radio_send_frame((uint8_t *)cOut_buff, uiLen);

  return;
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void CheckAlarmBytes(const unsigned char *Frame, int noPosh, int *ackFlag)
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
void CheckTimeBytes(const unsigned char *Frame, int noPosh, int *ackFlag)
{
  //bytes 9,10,11,12 indicate the time to be set (counting Frame from 0)
  printf("\nTime to set: %X %X %X %X", Frame[9], Frame[10], Frame[11], Frame[12]);
  //("\nSending Acknowledgement...");
  //writeDataToPort(ACK);
  *ackFlag = *ackFlag + 1;
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
int ValidatePacketType(const unsigned char dataPacketType)
{
  const unsigned char packetType[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
  int i;

  for (i = 0; i < 5; i++)
  {
    if (dataPacketType == packetType[i])
      return i;
  }
  return -1;
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void ParseFrame(void *pt)
{
  /*
  INPUT:
  Frame array that contains the packet and the total number of elements in that array.
  OUTPUT:
  Call to writeDataToPort() with the appropriate reply.

  DESC:
  Now, we are sending just one frame at a time. Hence, now we can say that we can harcode the value such that
  0 --> start byte
  1 --> datagram type

  ADDITIONAL WORK:
  Each command will have various test cases that will have to be covered. So keep on thinking on that.
  */
  const unsigned char status0x14[] = { 0x9C, 0x05, 0x17, 0x02, 0x00, 0x01, 0xD7, 0x68, 0x56, 0x00, 0x00, 0x00, 0x14, 0x00, 0x02, 0xD7, 0x68, 0xAA, 0x00, 0x00, 0x00, 0x60, 0xC9 }; //reply for high voltage query.
//  const char status0x13[] = { 0x9C, 0x04, 0x20, 0x21, 0x22, 0xC9 };
  //const unsigned char status0X11[] = { 0x9C, 0x02, 0x11, 0x01, 0x64, 0x50, 0x0B, 0xB8, 0xD7, 0x68, 0x56, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xC9 };
  const unsigned char status0x17[] = { 0x9c, 0x03, 0x18, 0x16, 0x01, 0x03, 0x1B, 0x10, 0x00, 0x0A, 0x10, 0x9C, 0x22, 0xC9, 0x3C, 0xC9, 0xA5, 0x0E, 0x0B, 0x2C, 0x8C, 0x9C, 0x77, 0xC9 };
  const unsigned char status0x15_Temp[] = { 0x9c, 0x05, 0x17, 0x02, 0x00, 0x01, 0xd7, 0x68, 0x56, 0x00, 0x00, 0x00, 0x14, 0x00, 0x02, 0xd7, 0x68, 0xAA, 0x00, 0x00, 0x00, 0x60, 0xc9 };

  unsigned char * ucRaw_Frame = pt;

  unsigned char OutGoingPacket[0x20];  //maximum length of an outgoing packet is 0x20
  uint16_t OutGoingPacket_len = 0;  //maximum length of an outgoing packet is 0x20
  unsigned char Frame[300];
//  int frameCnt;
  unsigned char lengthPacket;// = Frame[2]; //which is also frameCnt, probably.
  unsigned char actCommand;// = Frame[3];
  unsigned char dataPacketType;// = Frame[1];

  char replyToSend[100];
  int noPosh = 0; //this is activated to indicate that the datagram received with parent command as 0x12, has no request related to hib, pow, sleep, on.
  //it is just related to Off Alarm.
  int ackFlag = 0;
//  int i; //just for the for loop
  //total data bytes = lengthPacket - (1start + 1packetType + 1Length + 1actCommand + 1 stopbyte)
  unsigned char cntDataBytes;// = lengthPacket - 5;
//  static int isRealStream;
//  int counter;//temporary for 0x17 case
//  DWORD dwBytesWritten;
  INMD_LED_COMMAND eInmd_LED_cmnd = INMD_LED_CMND_LED_OFF;
  initACKNACK();

//  PARSE_THREAD *args = (PARSE_THREAD *)pt;
//  frameCnt = args->count-1;
//  for (i = 0; i < frameCnt; i++)
//  {
//    Frame[i] = args->Frame[i];
//  }
  lengthPacket = ucRaw_Frame[2];
  memcpy(Frame, pt, lengthPacket);

  lengthPacket = Frame[2]; //which is also frameCnt, probably.
  actCommand = Frame[3];
  dataPacketType = Frame[1];
  cntDataBytes = lengthPacket - 5;

  if (0 <= ValidatePacketType(dataPacketType))
  {
    if (-1 == SearchArrCmd(dataPacketType, actCommand))
    {
      printf("\nCommand - %X is invalid for DataPacketType - %X!", actCommand, dataPacketType);
      printf("\nSending NACK...");
      ineedmd_send_nack();
    }
    else
    {
      //when the control comes here, it is implicitly implied that the packetType and also the actual command are very much valid.
      switch (actCommand)
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
        switch (Frame[6])
        {
        case STATUS_HIBERNATE:
          printf("\nReceived request for HIBERNATE...");
          //PrintCommand(Frame, frameCnt+1);
          //writeDataToPort(ACK);
          break;
        case STATUS_SLEEP:
          printf("\nReceived request for SLEEP...");
          //PrintCommand(Frame, frameCnt+1);
          //writeDataToPort(ACK);
          break;
        case STATUS_ON:
          printf("\nReceived request for ON...");
          //PrintCommand(Frame, frameCnt+1);
          //writeDataToPort(ACK);
          break;
        case STATUS_POWER:
          printf("\nReceived request for POWER...");
          //PrintCommand(Frame, frameCnt+1);
          //writeDataToPort(ACK);
          break;
        default:
          /*printf("\nNot a valid status request! Sending NACK!");
          writeDataToPort(NACK);*/
          noPosh = 1;
          break;
        }
        //Processing the same frame for the alarm bytes
        CheckAlarmBytes(Frame, noPosh, &ackFlag);
        CheckTimeBytes(Frame, noPosh, &ackFlag); //in both these functions, ackFlag will act as __out__ parameter.
        if (2 == ackFlag) // ackFlag will be hardcoded 2, because both these above functions increment it.
        {
          printf("\nSending Acknowledgement...");
          ineedmd_send_ack();
          //reset the ackFlag to 0 for further processing...
          ackFlag = 0;
        }
        break;
      case 0x16: //capture data set
//        printf("\nCapture dataset... \nSending Acknowledgement...");
        printf("Capture dataset...");
        printf("Sending Acknowledgement...");
        ineedmd_send_ack();
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
          ineedmd_send_nack();
        }
        else
        {
          printf("\nReceived request for DataSet Transfer, with dataSetID: %X %X.\nSending Acknowledgement...", Frame[7], Frame[8]);
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
          ineedmd_send_nack();
        }
        else
        {
          printf("\nReceived request for erasing dataset...\nSending Acknowledgement...");
          ineedmd_send_ack();
          printf("\nErasing data set with ID: %X %X ", Frame[7], Frame[8]);
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
        OutGoingPacket[OutGoingPacket_len++] = ineedmd_get_battery_voltage();
        OutGoingPacket[OutGoingPacket_len++] = (char) (0xff & ineedmd_get_unit_tempoerature());

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
//        switch (Frame[frameCnt - 1])
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
        if(Frame[4] == EKG_TEST_PAT)
        {
          printf("Received \"test\" request...");
          iIneedmd_waveform_enable_TestSignal();
        }
        else if(Frame[4] == LED_TEST_PATTERN)
        {
          printf("Received LED test request");
          eInmd_LED_cmnd = (INMD_LED_COMMAND)Frame[5];
          switch(eInmd_LED_cmnd)
          {
            case INMD_LED_CMND_LED_OFF:
              printf("LED_OFF");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_OFF, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_POWER_ON_BATT_LOW:
              printf("POWER_ON_BATT_LOW");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_POWER_ON_BATT_LOW, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_POWER_ON_BATT_GOOD:
              printf("POWER_ON_BATT_GOOD");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_POWER_ON_BATT_GOOD, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_BATT_CHARGING:
              printf("BATT_CHARGING");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_BATT_CHARGING, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_BATT_CHARGING_LOW:
              printf("BATT_CHARGING_LOW");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_BATT_CHARGING_LOW, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_LEAD_LOOSE:
              printf("LEAD_LOOSE");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_LEAD_LOOSE, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_LEAD_GOOD_UPLOADING:
              printf("LEAD_GOOD_UPLOADING");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_LEAD_GOOD_UPLOADING, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_DIG_FLATLINE:
              printf("DIG_FLATLINE");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_DIG_FLATLINE, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_BT_CONNECTED:
              printf("BT_CONNECTED");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_BT_CONNECTED, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_BT_ATTEMPTING:
              printf("BT_ATTEMPTING");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_BT_ATTEMPTING, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_BT_FAILED:
              printf("BT_FAILED");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_BT_FAILED, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_USB_CONNECTED:
              printf("USB_CONNECTED");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_USB_CONNECTED, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_USB_FAILED:
              printf("USB_FAILED");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_USB_FAILED, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_DATA_TRANSFER:
              printf("DATA_TRANSFER");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_DATA_TRANSFER, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_TRANSFER_DONE:
              printf("TRANSFER_DONE");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_TRANSFER_DONE, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_STORAGE_WARNING:
              printf("STORAGE_WARNING");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_STORAGE_WARNING, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_ERASING:
              printf("ERASING");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_ERASING, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_ERASE_DONE:
              printf("ERASE_DONE");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_ERASE_DONE, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_DFU_MODE:
              printf("DFU_MODE");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_DFU_MODE, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_MV_CAL:
              printf("MV_CAL");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_MV_CAL, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_TRI_WVFRM:
              printf("TRI_WVFRM");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_TRI_WVFRM, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_REBOOT:
              printf("REBOOT");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_REBOOT, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_HIBERNATE:
              printf("HIBERNATE");
              ineedmd_send_ack();
              //TODO: add battery state check
              //ineedmd_led_pattern(HIBERNATE);
              //eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_HIBERNATE, SPEAKER_SEQ_NONE, false);
              check_battery();
              break;
            case INMD_LED_CMND_LEADS_ON:
              printf("LEADS_ON");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_LEADS_ON, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_MEMORY_TEST:
              printf("MEMORY_TEST");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_MEMORY_TEST, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_COM_BUS_TEST:
              printf("COM_BUS_TEST");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_COM_BUS_TEST, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_CPU_CLOCK_TEST:
              printf("CPU_CLOCK_TEST");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_CPU_CLOCK_TEST, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_FLASH_TEST:
              printf("FLASH_TEST");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_FLASH_TEST, SPEAKER_SEQ_NONE, false);
              break;
            case INMD_LED_CMND_TEST_PASS:
              printf("TEST_PASS");
              ineedmd_send_ack();
              eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_POWER_UP_GOOD, SPEAKER_SEQ_NONE, false);
              break;
            default:
              printf("Unrecongnized Sequence - No Soup For You");
              ineedmd_send_nack();
              break;
          }
        }
        else if(Frame[4] == STOP_EKG_TEST_PAT)
        {
          printf("Received \"test\" terminate...");
          iIneedmd_waveform_disable_TestSignal();
        }
        else if(Frame[4] == REQ_FOR_DFU)
        {
          printf("Performing DFU...");
          ineedmd_send_ack();
          eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_ACTUAL_DFU, SPEAKER_SEQ_NONE, true);
          iHW_delay(500); //todo MAGIC NUMBER

          check_for_update();
          eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_REBOOT, SPEAKER_SEQ_NONE, true);
          iHW_delay(500); //todo MAGIC NUMBER
          ineedmd_watchdog_doorbell();
          //TODO: add dfu entry
        }
        else if(Frame[4] == REQ_FOR_RESET)
        {
          printf("Performing RESET..");
          ineedmd_send_ack();
          eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_REBOOT, SPEAKER_SEQ_NONE, true);

          //set the radio to factory defaults
          eIneedmd_radio_rfd();
          //todo proper delay sleep_for_tenths(50);

          //Reset the system
          ineedmd_watchdog_doorbell();

          //Should never get there, loop just in case
          while(1){};

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
    ineedmd_send_nack();
  }
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
int iIneedmd_Rcv_cmnd_frame(uint8_t * uiCmnd_Frame, uint16_t uiCmnd_Frame_len)
{
  int iEC = 0;
  void * vDid_copy = NULL;
  memset(uiINMD_Protocol_frame, 0x00, INMD_FRAME_BUFF_SIZE);

  if(uiCmnd_Frame_len >= INMD_FRAME_BUFF_SIZE)
  {
    iEC = -1;
  }
  else
  {
    vDid_copy = memcpy(uiINMD_Protocol_frame, uiCmnd_Frame, uiCmnd_Frame_len);
    if(vDid_copy != NULL)
    {
      bIs_protocol_frame = true;
//      uiINMD_Protocol_frame_len = uiCmnd_Frame_len;
    }
    else
    {
      iEC = -2;
    }
  }
  return iEC;
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
int iIneedmd_Send_cmnd_frame(uint8_t * uiCmnd_Frame, uint16_t uiCmnd_Frame_len)
{
  return 1;
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
bool iIneedmd_is_protocol_frame(void)
{
  bool bWas_frame;

  bWas_frame = bIs_protocol_frame;
  bIs_protocol_frame = false;

  return bWas_frame;
}
//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
int iIneedmd_command_process(void)
{
  bool bIs_frame = false;

  bIs_frame = iIneedmd_is_protocol_frame();

  if(bIs_frame == true)
  {
//    parseCommand(uiINMD_Protocol_frame, uiINMD_Protocol_frame_len);
    ParseFrame(uiINMD_Protocol_frame);
  }
  return 1;
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
