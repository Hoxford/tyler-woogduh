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
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "app_inc/ineedmd_command_protocol.h"

//*****************************************************************************
// defines
//*****************************************************************************
#define COMMAND_FLAG 0x01
#define STATUS_FLAG  0x02
#define MEASURE_FLAG 0x03
#define STATUS_HIBERNATE 0x00
#define STATUS_SLEEP 0x20
#define STATUS_ON 0x40
#define STATUS_POWER 0x60

#define printf  debug_out
#define sprintf  debug_out

//*****************************************************************************
// variables
//*****************************************************************************
static const int dataApplicable[7] = { 0, 1, 0, 1, 1, 1, 1 };
static char NACK[5]; //this is a string
static char ACK[5]; //this too is a string
static const char status0X11[] = { 0x9C, 0x01, 0x11, 0x01, 0x64, 0x50, 0x0B, 0xB8, 0xD7, 0x68, 0x56, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xC9 }; //reply for high voltage query.
static const char status0x13[] = { 0x9C, 0x04, 0x20, 0x21, 0x22,0xC9 };
static const char status0x14[] = { 0x9C, 0x04, 0x20, 0x21, 0x22, 0xC9 };
static const char status0x15[] = { 0x9C, 0x03, 0x0E, 0x16, 0x03, 0x10, 0x0A, 0x9C, 0xC9, 0xC9, 0x0E, 0x2C, 0x9C, 0xC9 };

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

int debug_out(char * cOut_buff,...);
int SearchArrCmd(char cArrCmd);
void writeDataToPort(char * cOut_buff);
//*****************************************************************************
// functions
//*****************************************************************************

//todo: define this
int debug_out(char * cOut_buff,...)
{
  return 1;
}

//todo: define this
int SearchArrCmd(char cArrCmd)
{
  return 1;
}


//TODO: define this
void writeDataToPort(char * cOut_buff)
{
  return;
}
//*****************************************************************************
// name: parseCommand
// description: The function is called if the data packet received is of a command.
// param description:
// return value description:
//*****************************************************************************
void parseCommand(unsigned char szCommand[],int cntCommand)
{
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
        indexCmd = SearchArrCmd(szCommand[i]);
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
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
int iIneedmd_command_process(void)
{
  return 1;
}

#endif //__INEEDMD_COMMAND_PROTOCOL_C__
