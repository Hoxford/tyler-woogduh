#pragma once
#include "user_defined_functions.h"
#include <stdio.h>

extern char NACK[10]; //defined in  initACKNACK.cpp

void ConsolidateCommands(unsigned char *szCommand, int cntCommand)
{
	/*
	In this function,
	INPUT:
	Read string from the serial port, that contains many different packets.
	OUTPUT:
	Parse the string received, segregate each packet and create a thread that calls the parseCommand() function for each packet.
	*/
	int i, copyToFrameArr;
	int cntTotalPacketsRead = 0;
	int cntLengthPacket = 0;
	int startFrame = 0, endFrame = 0;
	unsigned char data[260]; //data can be of maximum 251 bytes.
	unsigned char Frame[300];
	unsigned char lengthPacket = 0;
	int frameCnt = 0;
	int dataCnt = 0;
	i = 0;
	while (i<cntCommand)
	{
		switch (szCommand[i])
		{
		case 0x9C:
			if (0 == startFrame)
			{
				//we have come across the start byte, we call the start frame.
				// mark this as the beginning of the packet.
				startFrame = 1;
				cntLengthPacket = 0; //indicates that we are counting bytes of a new packet. Will be over-written.
				copyToFrameArr = 1;
			}
			else
			{
				//this indicates that the we have already initially encountered the start frame and whatever byte we are encountering now is a part of data mostly
				//i am using the word 'mostly' because, this should not be occuring as the 2nd,3rd,4th byte ( cntLengthPacket should be > 4 )
				if (cntLengthPacket >= 4)
				{
					//now 0x9C is a part of data.
					//copy it into the data array
					data[dataCnt++] = szCommand[i];
				}
			}
			break;
		case 0xC9:
			//this should happen only if startFrame is previously 1
			if ((lengthPacket - 1 == cntLengthPacket) && (1 == startFrame))
			{
				//indicates end of one packet. This byte is the end Frame.
				endFrame = 1;
				startFrame = 0;
			}
			else
			{
				//this is a part where this byte has occurred as data.
			}
			break;
		default:
			if (3 == cntLengthPacket + 1)
			{
				//indicates that we have reached the byte which gives us the length of the whole packet.
				lengthPacket = szCommand[i];
			}
			break;
		}//switch case ends here
		if (copyToFrameArr)
			Frame[frameCnt++] = szCommand[i];
		cntLengthPacket++;
		i++;
		if (1 == endFrame)
		{
			//create a thread for this packet and send this for processing.
			//for sending this for processing, don't terminate it with \0 and send both the count and array to the parseCommand thread.	
			//But for the time-being just print the Frame.
			cntTotalPacketsRead++;
			printf("\n\nDatagram read -> ");
			PrintCommand(Frame, frameCnt);
			//ParseFrame(Frame, frameCnt - 1);  //sending the total number of elements in the Frame array as the 2nd parameter.
			frameCnt = 0; //reset the frame count to read the next frame.
			cntLengthPacket = 0;//reset for counting the bytes of next frame.
			lengthPacket = 0;//reset this for the next packet.
			copyToFrameArr = 0;
			endFrame = 0; //no need to keep the flag set once you have processed this particular frame.
		}
	}
	if ((1 == startFrame) || (0 == cntTotalPacketsRead))
	{
		//send a negative acknowledgement as the user has sent a command that is not in accordance with the length and other protocol standards.
		printf("\nErroneous bytes encountered! Sending NACK!");
		writeDataToPort(NACK);
	}
}