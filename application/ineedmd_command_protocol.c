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
#include "board.h"
#include "app_inc/ineedmd_command_protocol.h"
#include "app_inc/ineedmd_waveform.h"
#include "app_inc/ineedmd_watchdog.h"
#include "ineedmd_bluetooth_radio.h"
#include "utils_inc/proj_debug.h"

//*****************************************************************************
// defines
//*****************************************************************************
#define INMD_FRAME_BUFF_SIZE    256
#define NACK_FRAME_SIZE  12
#define ACK_FRAME_SIZE   12
#define DWORD    unsigned long

#define COMMAND_FLAG      0x01
#define STATUS_FLAG       0x02
#define MEASURE_FLAG      0x03
#define STATUS_HIBERNATE  0x00
#define STATUS_SLEEP      0x20
#define STATUS_ON         0x40
#define STATUS_POWER      0x60

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
static uint16_t uiINMD_Protocol_frame_len = 0;

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

//*****************************************************************************
// enums
//*****************************************************************************

//*****************************************************************************
// structures
//*****************************************************************************

//*****************************************************************************
// external functions
//*****************************************************************************

//*****************************************************************************
// function declarations
//*****************************************************************************

//int debug_out(char * cOut_buff,...);
//int SearchArrCmd(char cArrCmd);
void initACKNACK(void);
int SearchArrCmd(const unsigned char dataPacketType, const unsigned char szRecCommand);
void writeDataToPort(char * cOut_buff);
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
void writeDataToPort(char * cOut_buff)
{
  uint16_t uiOut_buff_len = 0;
  uiOut_buff_len = strlen(cOut_buff);
//  iIneedmd_radio_que_frame((uint8_t *)cOut_buff, uiOut_buff_len);
  ineedmd_radio_send_frame((uint8_t *)cOut_buff, uiOut_buff_len);

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
// name: parseCommand
// description: The function is called if the data packet received is of a command.
// param description:
// return value description:
//*****************************************************************************
void parseCommand(unsigned char szCommand[],int cntCommand)
{
#ifdef USE_ME
  /* INPUT:
      The string that contains the command. This command string will be containing the command, the relevant data (if any).
     OUTPUT:
      This function sends back the ACK/NACK or the data (if applicable to any command)
     COMMANDS:
          refer arrCommand[][]
  */
  int dataFlag = 0;
  int dataCnt = 0;

  int indexCmd;
  int i; // just used for parsing the szCommand[]
  char data[6];
  char szRecCommand_data = data[3];
  char tempACKNACK[100];
  int retFlag = 0;
  static int fAsyncWrite;

  i = 0;//initialize all integers to begin
  //check the first "start byte of the arrived frame...
  while ((szCommand[i] != '\0')||(i<cntCommand)) // both the conditions will be true simultaneously
  {
    switch (szCommand[i]) //the main task of this switch is to validate the read data.
    {
    case 0x9C:
      if (0==i)
        printf("\nStart Frame OK!");
      else
      {
        if (i >= 4 && i < 9)
        {
          data[dataCnt++] = szCommand[i];
          dataFlag--;
        }
      }
      break;
    case 0x01: //represents that the incoming datagram represents a command.
      if (1 == i)
      {
        printf("\nIntermediary [0] OK!");
      }
      else
      {
        if (i >= 4 && i < 9)
        {
          data[dataCnt++] = szCommand[i];
          dataFlag--;
        }
      }
      break;
    case 0x0A:
      if (2 == i)
      {
        printf("\nIntermediary [1] OK!");
      }
      else
      {
        if (i >= 4 && i < 9)
        {
          data[dataCnt++] = szCommand[i];
          dataFlag--;
        }
      }
      break;
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
      if (3 == i)
      {
        indexCmd = SearchArrCmd(szCommand[i], szRecCommand_data);
        if (-1 == indexCmd)
        {
          printf("Error: %s -- Invalid Command!");
          printf(" Sending NACK...");
          fAsyncWrite = 1;
        }
        else
        {
          printf("\nCommand -- %02X OK!", szCommand[i]);
          if (dataApplicable[indexCmd])
          {
            printf(" Data = YES");
            dataFlag = 5;// now, the next 5 bytes will be data. Enter default case, for every data encountered decrement dataFlag and store read char in data[]
          }
          else
            printf(" Data = NO");
        }
      }
      else
      {
        if (i >= 4 && i < 9)
        {
          data[dataCnt++] = szCommand[i];
          dataFlag--;
        }
      }
      break;
    case 0xC9:
      if (('\0' == szCommand[i + 1]) && (!dataFlag) && (9==i))
        printf("\nEnd Frame OK!");
      else
      {
        if ('\0' != szCommand[i + 1])
        {
          data[dataCnt++] = szCommand[i]; //the data byte is same as the finish byte
          dataFlag--;
        }
        else
        {
          //there are less number of data packets that should have been
          //send negative acknowledgement
          sprintf(tempACKNACK, "NACK -- %X", NACK);
          writeDataToPort(tempACKNACK);
          return;
        }
      }
      break;
    default:
      if (i < 4)
      {
        sprintf(tempACKNACK, "NACK -- %X", NACK);
        writeDataToPort(tempACKNACK);
        return;
      }
      if (dataFlag)
      {
        if (0x00!=szCommand[i])
          data[dataCnt++] = szCommand[i];
        dataFlag--;
      }
      break;
    }
    //increment all the integers necessary...
    i++;
  }
  data[dataCnt] = '\0';
  printf("\nCommand = %X\tData = %s\t",szCommand[3], data);
  //the following commands send out only ACK/NACK, 0x12,0x16
  switch (indexCmd)
  {
  case 1://0x12
    switch (szCommand[8])
    {
    case STATUS_HIBERNATE:
      sprintf(tempACKNACK, "ACK -- %s", ACK);
      writeDataToPort(tempACKNACK);
      break;
    case STATUS_SLEEP:
      sprintf(tempACKNACK, "ACK -- %s", ACK);
      writeDataToPort(tempACKNACK);
      break;
    case STATUS_ON:
      sprintf(tempACKNACK, "ACK -- %s", ACK);
      writeDataToPort(tempACKNACK);
      break;
    case STATUS_POWER:
      sprintf(tempACKNACK, "ACK -- %s", ACK);
      writeDataToPort(tempACKNACK);
      sprintf(tempACKNACK, "STATUS POWER -- %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
        status0X11[0], status0X11[1], status0X11[2], status0X11[3],
        status0X11[4], status0X11[5], status0X11[6], status0X11[7],
        status0X11[8], status0X11[9], status0X11[10], status0X11[11],
        status0X11[12], status0X11[13], status0X11[14], status0X11[15],
        status0X11[16]);
      writeDataToPort(tempACKNACK);
      break;

    }
    break;
  case 5://0x16
    sprintf(tempACKNACK, "ACK -- %s", ACK);
    writeDataToPort(tempACKNACK);
    break;
  case 0://0x11 .. starting command
    sprintf(tempACKNACK, "STATUS RECORD -- %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
      status0X11[0], status0X11[1], status0X11[2], status0X11[3],
      status0X11[4], status0X11[5], status0X11[6], status0X11[7],
      status0X11[8], status0X11[9], status0X11[10], status0X11[11],
      status0X11[12], status0X11[13],status0X11[14], status0X11[15],
      status0X11[16]);
    writeDataToPort(tempACKNACK);
    break;
  case 2: //0x13
    sprintf(tempACKNACK, "STORED INFO BLOCK -- %X %X %X %X %X %X ",
      status0x13[0],status0x13[1],status0x13[2],
      status0x13[3], status0x13[4], status0x13[5]);
    writeDataToPort(tempACKNACK);
    break;
  case 3://0x14
    if (0 == strlen((char*)data))
    {
      sprintf(tempACKNACK, "NACK -- %s", NACK);
      writeDataToPort(tempACKNACK);
      return;
    }
    sprintf(tempACKNACK, "REPLY FOR 0x14 -- %X %X %X %X %X %X ",
      status0x14[0], status0x14[1], status0x14[2],
      status0x14[3], status0x14[4], status0x14[5]);
    writeDataToPort(tempACKNACK);
    break;
  case 4: //0x15
    if (0 == strlen((char*)data))
    {
      sprintf(tempACKNACK, "NACK -- %s", NACK);
      writeDataToPort(tempACKNACK);
      return;
    }
    sprintf(tempACKNACK, "REPLY FOR 0x15 -- %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
      status0x15[0], status0x15[1], status0x15[2], status0x15[3],
      status0x15[4], status0x15[5], status0x15[6], status0x15[7],
      status0x15[8], status0x15[9], status0x15[10], status0x15[11],
      status0x15[12], status0x15[13]);
    writeDataToPort(tempACKNACK);
    break;
  case 6://0x17
    if (0 == strlen((char*)data))
    {
      sprintf(tempACKNACK, "NACK -- %s", NACK);
      writeDataToPort(tempACKNACK);
      return;
    }
    sprintf(tempACKNACK, "REPLY FOR 0x17 -- %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
      status0x15[0], status0x15[1], status0x15[2], status0x15[3],
      status0x15[4], status0x15[5], status0x15[6], status0x15[7],
      status0x15[8], status0x15[9], status0x15[10], status0x15[11],
      status0x15[12], status0x15[13]);
    writeDataToPort(tempACKNACK);
    break;
  }
#endif //0
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
  const unsigned char status0X11[] = { 0x9C, 0x02, 0x11, 0x01, 0x64, 0x50, 0x0B, 0xB8, 0xD7, 0x68, 0x56, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xC9 };
  const unsigned char status0x17[] = { 0x9c, 0x03, 0x18, 0x16, 0x01, 0x03, 0x1B, 0x10, 0x00, 0x0A, 0x10, 0x9C, 0x22, 0xC9, 0x3C, 0xC9, 0xA5, 0x0E, 0x0B, 0x2C, 0x8C, 0x9C, 0x77, 0xC9 };
  const unsigned char status0x15_Temp[] = { 0x9c, 0x05, 0x17, 0x02, 0x00, 0x01, 0xd7, 0x68, 0x56, 0x00, 0x00, 0x00, 0x14, 0x00, 0x02, 0xd7, 0x68, 0xAA, 0x00, 0x00, 0x00, 0x60, 0xc9 };

  unsigned char * ucRaw_Frame = pt;
  unsigned char Frame[300];
  int frameCnt;
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
  static int isRealStream;
  int counter;//temporary for 0x17 case
//  DWORD dwBytesWritten;

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
      writeDataToPort(NACK);
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
          writeDataToPort(ACK);
          //reset the ackFlag to 0 for further processing...
          ackFlag = 0;
        }
        break;
      case 0x16: //capture data set
//        printf("\nCapture dataset... \nSending Acknowledgement...");
        printf("Capture dataset...");
        printf("Sending Acknowledgement...");
        writeDataToPort(ACK);
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
        writeDataToPort(replyToSend);
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
          writeDataToPort(NACK);
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

          writeDataToPort(replyToSend);
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
          writeDataToPort(NACK);
        }
        else
        {
          printf("\nReceived request for erasing dataset...\nSending Acknowledgement...");
          writeDataToPort(ACK);
          printf("\nErasing data set with ID: %X %X ", Frame[7], Frame[8]);
          printf("\nAfter Deletion, sending...");
          PrintCommand(status0x15_Temp, 23);
          sprintf(replyToSend, "%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X ",
            status0x15_Temp[0], status0x15_Temp[1], status0x15_Temp[2], status0x15_Temp[3], status0x15_Temp[4],
            status0x15_Temp[5], status0x15_Temp[6], status0x15_Temp[7], status0x15_Temp[8], status0x15_Temp[9],
            status0x15_Temp[10], status0x15_Temp[11], status0x15_Temp[12], status0x15_Temp[13], status0x15_Temp[14],
            status0x15_Temp[15], status0x15_Temp[16], status0x15_Temp[17], status0x15_Temp[18], status0x15_Temp[19],
            status0x15_Temp[20], status0x15_Temp[21], status0x15_Temp[22]);
          writeDataToPort(replyToSend);
        }
        break;
      case 0x11://get status.
//        printf("\nReceived request for status...\nSending status record...");
        printf("Received request for status...");
        sprintf(replyToSend, "%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
          status0X11[0], status0X11[1], status0X11[2], status0X11[3],
          status0X11[4], status0X11[5], status0X11[6], status0X11[7],
          status0X11[8], status0X11[9], status0X11[10], status0X11[11],
          status0X11[12], status0X11[13], status0X11[14], status0X11[15],
          status0X11[16]);
        PrintCommand(status0X11, 17); //this is just to display on our side, what reply we are sending.
        printf("Sending status record...");
        writeDataToPort(replyToSend);
        break;
      case 0x17:
        switch (Frame[frameCnt - 1])
        {
        case 0xFF: //true
          isRealStream = 1;
          counter = 0;
          printf("\nBegin real-time streaming -- TRUE... ");
          //writeDataToPort(ACK);
          printf("\nReceived request for Real time DataSet Transfer... ");
          PrintCommand(status0x17, 24);
          sprintf(replyToSend, "%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X ",
            status0x17[0], status0x17[1], status0x17[2], status0x17[3],
            status0x17[4], status0x17[5], status0x17[6], status0x17[7],
            status0x17[8], status0x17[9], status0x17[10], status0x17[11],
            status0x17[12], status0x17[13], status0x17[14], status0x17[15], status0x17[16], status0x17[17],
            status0x17[18], status0x17[19], status0x17[20], status0x17[21],
            status0x17[22], status0x17[23]);
          counter = 0;
          while (isRealStream)
          {

            writeDataToPort(replyToSend);
            /*if (WriteFile(hSerial, replyToSend, 24, &dwBytesWritten, NULL))
            {
              printf("\n Sending Real time data (%d) to COM port... ", ++counter);
              printf("\nTotal Bytes Written (Live Streaming): %d", dwBytesWritten);
              continue;
            }
            else
            {
              printf("\nWriteFile failed: %d", GetLastError());
            }*/

            //Sleep(1500); //this thread will sleep and continue it's work.
          }
          break;
        case 0x00: //false
          isRealStream = 0;
          printf("\nStop real-time streaming....");
          //writeDataToPort(ACK);
          break;
          /*default:
          printf("\nMissing argument to indicate beginning of streaming real time! Sending NACK!");
          writeDataToPort(NACK);*/
          break;
        default:
          printf("\nMissing argument 'TRUE/FALSE'!");
          printf("\nSending NACK!");
          writeDataToPort(NACK);
          break;
        }
        break;

      case 0x18://request to send test signal.
        if(Frame[4] == 0x01)
        {
          printf("Received \"test\" request...");
          iIneedmd_waveform_enable_TestSignal();
        }
        else if(Frame[4] == 0x00)
        {
          printf("Received \"test\" terminate...");
          iIneedmd_waveform_disable_TestSignal();
        }
        else if(Frame[4] == 0x0F)
        {
          printf("Performing DFU...");
          ineedmd_watchdog_doorbell();
        }
        //the output expected is TEST DATA PATTERN 1
//        printf("\nReceived \"test\" request...");

//        printf("\nSending pattern 1...");
//        printf("Sending pattern 1...");
//        sprintf(replyToSend, "test data <pattern-1>");//this is temporary, and is going to be updated by the actual packet, which is not known to us at this point.
//        writeDataToPort(replyToSend);

        break;
      }
    }
  }
  else
  {
    printf("\nInvalid type of packet received!");
    printf("\nSending NACK!");
    writeDataToPort(NACK);
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
      uiINMD_Protocol_frame_len = uiCmnd_Frame_len;
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
