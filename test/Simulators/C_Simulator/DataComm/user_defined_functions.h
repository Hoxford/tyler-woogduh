#pragma once
#include "user_defined_functions.h"
extern void unsigned_strcpy(unsigned char *, const unsigned char *);
extern void CheckAlarmBytes(const unsigned char *, int, int*); //
extern void CheckTimeBytes(const unsigned char *, int, int*);// BOTH IN CheckAlarmAndTimeBytes.cpp
extern void writeDataToPort(char *); //DataComm.cp
extern void initACKNACK(); //intACKNACK.cpp
extern void PrintCommand(const unsigned char *, int); //PrintComamnd.cpp
//extern void ParseFrame(const unsigned char *, int); //ParseCommand.cpp
extern void ParseFrame(void *);
extern int SearchArrCmd(const unsigned char, const unsigned char); //defined in ValidateBytesOfPacket.cpp
extern int ValidatePacketType(const unsigned char); //defined in ValidateBytesOfPacket.cpp
typedef struct parse_thread
{
	unsigned char Frame[300];
	int count;
}PARSE_THREAD;