#pragma once
#include <stdio.h>

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

void CheckTimeBytes(const unsigned char *Frame, int noPosh, int *ackFlag)
{
	//bytes 9,10,11,12 indicate the time to be set (counting Frame from 0)
	printf("\nTime to set: %X %X %X %X", Frame[9], Frame[10], Frame[11], Frame[12]);
	//("\nSending Acknowledgement...");
	//writeDataToPort(ACK);
	*ackFlag = *ackFlag + 1;
}
