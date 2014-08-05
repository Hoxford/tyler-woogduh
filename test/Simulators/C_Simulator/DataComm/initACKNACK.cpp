#pragma once
#include <stdio.h>
char NACK[10];
char ACK[5];
void initACKNACK()
{
	unsigned const char NAK[4] = { 0x9C, 0xFF, 0x04, 0xC9 };
	sprintf(NACK, "%X %X %X %X", NAK[0], NAK[1], NAK[2], NAK[3]);
	sprintf(ACK, "%X %X %X %X ", 0x9C, 0x00, 0x04, 0xC9);
}