#pragma once
#include <stdio.h>
#include "user_defined_functions.h"
#include <Windows.h>
#define STATUS_HIBERNATE 0x00
#define STATUS_SLEEP 0x20
#define STATUS_ON 0x40
#define STATUS_POWER 0x60

extern char ACK[5];
extern char NACK[10]; // both of these have been defined in the initACKNACK.cppS
extern HANDLE hSerial; //From the file DataComm.cpp

//void ParseFrame(const unsigned char *Frame, int frameCnt)
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
	const char status0x13[] = { 0x9C, 0x04, 0x20, 0x21, 0x22, 0xC9 };
	const unsigned char status0X11[] = { 0x9C, 0x02, 0x11, 0x01, 0x64, 0x50, 0x0B, 0xB8, 0xD7, 0x68, 0x56, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xC9 };
	const unsigned char status0x17[] = { 0x9c, 0x03, 0x18, 0x16, 0x01, 0x03, 0x1B, 0x10, 0x00, 0x0A, 0x10, 0x9C, 0x22, 0xC9, 0x3C, 0xC9, 0xA5, 0x0E, 0x0B, 0x2C, 0x8C, 0x9C, 0x77, 0xC9 };
	const unsigned char status0x15_Temp[] = { 0x9c, 0x05, 0x17, 0x02, 0x00, 0x01, 0xd7, 0x68, 0x56, 0x00, 0x00, 0x00, 0x14, 0x00, 0x02, 0xd7, 0x68, 0xAA, 0x00, 0x00, 0x00, 0x60, 0xc9 };

	unsigned char Frame[300];
	int frameCnt;
	unsigned char lengthPacket;// = Frame[2]; //which is also frameCnt, probably.
	unsigned char actCommand;// = Frame[3];
	unsigned char dataPacketType;// = Frame[1];

	char replyToSend[100];
	int noPosh = 0; //this is activated to indicate that the datagram received with parent command as 0x12, has no request related to hib, pow, sleep, on.
	//it is just related to Off Alarm.
	int ackFlag = 0;
	int i; //just for the for loop 
	//total data bytes = lengthPacket - (1start + 1packetType + 1Length + 1actCommand + 1 stopbyte)
	unsigned char cntDataBytes;// = lengthPacket - 5;
	static int isRealStream;
	int counter;//temporary for 0x17 case
	DWORD dwBytesWritten;

	PARSE_THREAD *args = (PARSE_THREAD *)pt;
	frameCnt = args->count-1;
	for (i = 0; i < frameCnt; i++)
	{
		Frame[i] = args->Frame[i];
	}

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
				printf("\nCapture dataset... \nSending Acknowledgement...");
				writeDataToPort(ACK);
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
				printf("\nReceived request for status...\nSending status record...");
				sprintf(replyToSend, "%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
					status0X11[0], status0X11[1], status0X11[2], status0X11[3],
					status0X11[4], status0X11[5], status0X11[6], status0X11[7],
					status0X11[8], status0X11[9], status0X11[10], status0X11[11],
					status0X11[12], status0X11[13], status0X11[14], status0X11[15],
					status0X11[16]);
				PrintCommand(status0X11, 17); //this is just to display on our side, what reply we are sending.
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
				//the output expected is TEST DATA PATTERN 1
				printf("\nReceived \"test\" request...");
				printf("\nSending pattern 1...");
				sprintf(replyToSend, "test data <pattern-1>");//this is temporary, and is going to be updated by the actual packet, which is not known to us at this point.
				writeDataToPort(replyToSend);
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
