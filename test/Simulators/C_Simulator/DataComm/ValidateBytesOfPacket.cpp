#pragma once


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