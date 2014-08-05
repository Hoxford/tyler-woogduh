#pragma once
#include <stdio.h>

void PrintCommand(const unsigned char *strCommand, int count)
{
	int i;
	//static int serNo;
	printf("\n");
	for (i = 0; i < count; i++)
		printf(" %02X ", strCommand[i]);
	//printf("\n");
}