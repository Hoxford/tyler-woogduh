#pragma once
//#include "user_defined_functions.h"
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <BluetoothAPIs.h>
//include <Winsock2.h>
#include <Ws2bth.h>
#include <fileapi.h>

#define COMMAND_FLAG 0x01
#define STATUS_FLAG  0x02
#define MEASURE_FLAG 0x03

//extern void ParseFrame(const unsigned char *, int); //ParseCommand.cpp
extern void ParseFrame(void *);
extern void ConsolidateCommands(unsigned char *, int); //all these functions are defined in their respective files.
extern void initACKNACK();
extern void PrintCommand(const unsigned char *, int);
extern char NACK[10]; //this is a string
extern char ACK[5]; //this too is a string

 
 // Declare variables and structures

typedef struct parse_thread
{
	unsigned char Frame[300];
	int count;
}PARSE_THREAD;

    HANDLE hSerial;
	HANDLE hEvent;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
    DWORD dwBytesWritten = 0;
	DWORD dwBytesRead;
	DWORD error;
	char remoteDevice [100];
	
	//OVERLAPPED ovRead = {0}; declaring this in the function itself.
	DWORD dwRes;
	DWORD dwMask;
	bool inLoop = false;
 
    char dev_name[MAX_PATH];
    char text_to_send[MAX_PATH];
	char text_to_read[MAX_PATH];
	char tmp[MAX_PATH];	
	char isLoopingEnabled[10];
	int Tx=0;
	int Rx=0;

	//string recData;

	DWORD eventFlags = EV_RXCHAR|EV_TXEMPTY;

//Bluetooth variables
BLUETOOTH_FIND_RADIO_PARAMS m_bt_find_radio = {sizeof(BLUETOOTH_FIND_RADIO_PARAMS)};
 
BLUETOOTH_RADIO_INFO m_bt_info = {sizeof(BLUETOOTH_RADIO_INFO),0,};
 
BLUETOOTH_DEVICE_SEARCH_PARAMS m_search_params = {
  sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS),
  1,
  1,
  1,
  1,
  1,
  15,
  NULL
};
 
BLUETOOTH_DEVICE_INFO m_device_info = {sizeof(BLUETOOTH_DEVICE_INFO),0,};
BLUETOOTH_DEVICE_INFO m_device_of_interest = {sizeof(BLUETOOTH_DEVICE_INFO),0,};

HANDLE m_radio = NULL;
HBLUETOOTH_RADIO_FIND m_bt = NULL;
HBLUETOOTH_DEVICE_FIND m_bt_dev = NULL;
int m_radio_id;
int m_device_id;
DWORD mbtinfo_ret;
static int fAsyncWrite;

//Function declaration

int Init();
void initParseStruct(PARSE_THREAD *, unsigned char *, int);
void readDataFromPort(void*);
void writeDataToPort(char *text_to_send);
//void operate(char &operation);
void openBluetoothCOMPort(wchar_t *deviceName);
void getCOMPort();
void PrintCommand(const unsigned char *,int);
GUID serialPortService = SerialPortServiceClass_UUID;
//void ParseFrame(const unsigned char *, int);
//void CheckAlarmBytes(const unsigned char *, int, int *);
//void CheckTimeBytes(const unsigned char *, int, int *);
//static const char startFrame = 0x9C;
//static const char interFrame[2] = { 0x02, 0x0A };
//static const char endFrame = 0xC9;

static int newThreadSwitch = 1;


int main(int argc, char *argv[])
{
	int choice;
	int KeyInfo;
	//char strToSend[50];
	int cToSend; //making it 32 bit
	int prevChoice; //to induce sleep for correct formatting of output.
	char szStrFromDongle[1000];
	void initACKNACK(void);
	static int idThread;



	//Copy the remote device name to local variable 
	strcpy_s(remoteDevice, argv[1]);
	initACKNACK();
	printf("Attempting to connect to %s\n", argv[1]);

	//Convert the data type from char to wchar_t
	size_t size = strlen(remoteDevice) + 1;
	wchar_t* deviceToConnectTo = new wchar_t[size];
	mbstowcs(deviceToConnectTo, remoteDevice, size);

	//Creating a COM port over Bluetooth
	openBluetoothCOMPort(deviceToConnectTo);

	//Give Windows some time to finish off installing the COM driver
	Sleep(5000); //initial value was 20000

	//Get COM port here
	getCOMPort();

	//Initialize the COM port 
	if (Init() > 0)
	{
		printf("\nPress q and enter to exit ");
		scanf("%d");
		BluetoothSetServiceState(m_radio, &m_device_of_interest, &serialPortService, BLUETOOTH_SERVICE_DISABLE);
		exit(1);
	}

	//if (!SetCommMask(hSerial, eventFlags)) // here, it is made clear for what events the thread is going to wait.....
	//{
	//	printf("Error in setting the event mask with error: %d \n", GetLastError());
	//}
	_beginthread(*readDataFromPort, 0, NULL);
	while (true)
	{
		printf("\n\Press.. \n'w' Write\n'q' Quit ");
		choice = _getch();
		switch (choice)
		{
			//case 1:
			//	printf("\nEntered READ mode!\n");
			//	/*while (iNewReadThread)
			//	{
			//		Sleep(5000);*/
			//		_beginthread(readDataFromPort, 0, NULL);
			//	//}
			//	prevChoice = 1;
			//	iNewReadThread = 0;//this will be made one by the existing & exiting thread
			//	break;
		case 'w':
			printf("\nEnter data to write: ");
			//scanf_s("%i", &cToSend);
			scanf("%s", szStrFromDongle);
			//wsprintf(szStrFromDongle, "%X%X%c%c%c", startFrame, interFrame[0], interFrame[1], cToSend, endFrame);
			printf("\nThe string to be sent is: %s", szStrFromDongle);
			writeDataToPort(szStrFromDongle);
			prevChoice = 2;
			break;
		case 'q':
			printf("\nExiting...");
			exit(0);
		default:
			printf("\nEnter valid choice!");
			break;
		}
	}
	return 0;
}

void writeDataToPort(char *text_to_send)
{
	OVERLAPPED ovWrite = {0};
	memset(&ovWrite,0,sizeof(ovWrite));
	ovWrite.hEvent = CreateEvent(0,TRUE,0,0);
	if(ovWrite.hEvent == NULL)
	{
		fprintf(stderr,"Error creating overlapped event writing");
	}

	//fprintf(stderr, "Sending text: %s\n", &text_to_send);
	if (!WriteFile(hSerial, text_to_send, strlen(text_to_send),&dwBytesWritten, &ovWrite))
	{
		//fprintf(stderr, "Error writing text to %s\n", dev_name);
		if(!GetLastError()==ERROR_IO_PENDING)
		{
			fprintf(stderr,"Error in writing\n");
			printf("%d",GetLastError());
		}
		else
		{
			//write is pending
			//Changed the timeout from 10000 to INIFINITE
			dwRes = WaitForSingleObject(ovWrite.hEvent, INFINITE);
			switch (dwRes)
			{
				//overlapped structure (ovWrite) hEvent member has been signalled.
			case WAIT_OBJECT_0:
				if (!GetOverlappedResult(hSerial, &ovWrite, &dwBytesWritten, TRUE)) //This function returns true if write operation 
					printf("Something is not correct\n");
				else
				{ //write operation completed successfully.
					++Tx;
					printf("\nTx - %d: Rx - %d",Tx,Rx);
					if(GetCommMask(hSerial,&dwMask))
					{
					ResetEvent(ovWrite.hEvent); //hEvent member is back to the non signalled state.
					}
				}
				break;
			default: //error has occurred in the waitforsingleobject function and indicates that something is wrong with the overlapped structure
				break;
			}
		}
	}
	else
	{
		fprintf(stderr, "\n%d bytes written to %s\n",dwBytesWritten, dev_name);
	}	
	//memset(text_to_send, 0x00, strlen(text_to_send));
	CloseHandle(ovWrite.hEvent);
}

int Init()
{
	char port_name[260]; //added to insert \\\\.\\%s
	fprintf(stderr, "Opening serial port %s...\n", dev_name);
	wsprintf(port_name, "\\\\.\\%s", dev_name);
	
	hSerial = CreateFile(port_name, GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	
    if (hSerial==INVALID_HANDLE_VALUE)
    {
		error = GetLastError();
        fprintf(stderr, "Error opening port\n");		
		printf("%d",error);
		CloseHandle(hSerial);
        return 1;
    }
	else
	{
		return 0;
	}    
 
    // Set device parameters (115200 baud, 1 start bit,1 stop bit, no parity)
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error getting device state\n");
        CloseHandle(hSerial);
        return 1;
    }
	dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if(SetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error setting device parameters\n");
        CloseHandle(hSerial);
        return 1;
    }
	else
	{
		return 0;
	}
 
    // Set COM port timeout settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if(SetCommTimeouts(hSerial, &timeouts) == 0)
    {
        fprintf(stderr, "Error setting timeouts\n");
        CloseHandle(hSerial);
        return 1;
    }
	else
	{
		return 0;
	}	
}

void openBluetoothCOMPort(wchar_t *deviceName)
{
	 while(TRUE)
    {
        m_bt = BluetoothFindFirstRadio(&m_bt_find_radio, &m_radio);
 
        if(m_bt != NULL)
                    printf("BluetoothFindFirstRadio() is OK!\n");
        else
                    printf("BluetoothFindFirstRadio() failed with error code %d\n", GetLastError());
 
        m_radio_id = 0;
 
        do {
            // Then get the radio device info....
            mbtinfo_ret = BluetoothGetRadioInfo(m_radio, &m_bt_info);
            if(mbtinfo_ret == ERROR_SUCCESS)
                        printf("BluetoothGetRadioInfo() looks fine!\n");
            else
                        printf("BluetoothGetRadioInfo() failed with error code %d\n", mbtinfo_ret);
 
            wprintf(L"Radio %d:\r\n", m_radio_id);
            wprintf(L"\tInstance Name: %s\r\n", m_bt_info.szName);
            wprintf(L"\tAddress: %02X:%02X:%02X:%02X:%02X:%02X\r\n", m_bt_info.address.rgBytes[5],
                        m_bt_info.address.rgBytes[4], m_bt_info.address.rgBytes[3], m_bt_info.address.rgBytes[2],
                            m_bt_info.address.rgBytes[1], m_bt_info.address.rgBytes[0]);
            wprintf(L"\tClass: 0x%08x\r\n", m_bt_info.ulClassofDevice);
            wprintf(L"\tManufacturer: 0x%04x\r\n", m_bt_info.manufacturer);
 
            m_search_params.hRadio = m_radio;
            ZeroMemory(&m_device_info, sizeof(BLUETOOTH_DEVICE_INFO));
            m_device_info.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);

			ZeroMemory(&m_device_of_interest,sizeof(BLUETOOTH_DEVICE_INFO));
			m_device_of_interest.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
 
            // Next for every radio, get the device
            m_bt_dev = BluetoothFindFirstDevice(&m_search_params, &m_device_info);
 
            if(m_bt_dev != NULL)
                        printf("\nBluetoothFindFirstDevice() is working!\n");
            else
                        printf("\nBluetoothFindFirstDevice() failed with error code %d\n", GetLastError());
 
            m_radio_id++;
            m_device_id = 0;
 
            // Get the device info
            do
            {
                if(wcscmp(m_device_info.szName,deviceName)==0)
				{
					wprintf(L"\n\tDevice %d:\r\n", m_device_id);
					wprintf(L"  \tInstance Name: %s\r\n", m_device_info.szName);
					wprintf(L"  \tAddress: %02X:%02X:%02X:%02X:%02X:%02X\r\n", m_device_info.Address.rgBytes[5],
                            m_device_info.Address.rgBytes[4], m_device_info.Address.rgBytes[3], m_device_info.Address.rgBytes[2],
                            m_device_info.Address.rgBytes[1], m_device_info.Address.rgBytes[0]);
					wprintf(L"  \tClass: 0x%08x\r\n", m_device_info.ulClassofDevice);
					wprintf(L"  \tConnected: %s\r\n", m_device_info.fConnected ? L"true" : L"false");
					wprintf(L"  \tAuthenticated: %s\r\n", m_device_info.fAuthenticated ? L"true" : L"false");
					wprintf(L"  \tRemembered: %s\r\n", m_device_info.fRemembered ? L"true" : L"false");
					m_device_id++;
					m_device_of_interest = m_device_info;
					BluetoothSetServiceState(m_radio,&m_device_of_interest,&serialPortService,BLUETOOTH_SERVICE_ENABLE);
					//This needs to be handled via Windows notification
					Sleep(2000);
					
					break;
				}
                        
                // If no more device, exit the loop
                /*if(!BluetoothFindNextDevice(m_bt_dev, &m_device_info))
					break;*/
 
            } while(BluetoothFindNextDevice(m_bt_dev, &m_device_info));
 
            // NO more device, close the device handle
            if(BluetoothFindDeviceClose(m_bt_dev) == TRUE)
                        printf("\nBluetoothFindDeviceClose(m_bt_dev) is OK!\n");
            else
			{
				printf("\nBluetoothFindDeviceClose(m_bt_dev) failed with error code %d\n", GetLastError());
				break;
			}
 
        } while(BluetoothFindNextRadio(&m_bt_find_radio, &m_radio));
 
        // No more radio, close the radio handle
        if(BluetoothFindRadioClose(m_bt) == TRUE)
            printf("Closing the Bluetooth radio\n");
        else
		{
            printf("BluetoothFindRadioClose(m_bt) failed with error code %d\n", GetLastError());
			break;
		}
 
        // Exit the outermost WHILE and BluetoothFindXXXXRadio loops if there is no more radio
        if(!BluetoothFindNextRadio(&m_bt_find_radio, &m_radio))
                    break;
		
		// Give some time for the 'signal' which is a typical for crap wireless devices
        Sleep(2000); //initial value was 10000
    }	
}

void getCOMPort()
{
	HKEY COMPORTS;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),0,KEY_READ,&COMPORTS)==ERROR_SUCCESS)
	{
		char port [255];
		int portChoice;
		DWORD portSize = sizeof(port);
		//TCHAR    achKey[255];   // buffer for subkey name
		//DWORD    cbName;                   // size of name string 
		TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
		DWORD    cchClassName = MAX_PATH;  // size of class string 
		DWORD    cSubKeys=0;               // number of subkeys 
		//DWORD    cbMaxSubKey;              // longest subkey size 
	    //DWORD    cchMaxClass;              // longest class string 
	    DWORD    cValues;              // number of values for key 
		//DWORD    cchMaxValue;          // longest value name 
		//DWORD    cbMaxValueData;       // longest value data 
		//DWORD    cbSecurityDescriptor; // size of security descriptor 
		//FILETIME ftLastWriteTime;      // last write time 
 
		DWORD i, retCode; 
 
	    TCHAR  achValue[16383]; 
	    DWORD cchValue = 16383; 

		retCode = RegQueryInfoKey(
		COMPORTS,                // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        NULL,					 // longest subkey size 
        NULL,					 // longest class string 
        &cValues,                // number of values for this key 
        NULL,                    // longest value name 
        NULL,         // longest value data 
        NULL,   // security descriptor 
        NULL);       // last write time 
 
    // Enumerate the key values. 

		if (cValues) 
		{
			printf( "\nApplication has detected %d COM port. Please select the port of your choice from the following ports\n", cValues);
			char detectedPorts[255][100];
			for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
			{ 
				cchValue = 1638; 
				achValue[0] = '\0'; 
				retCode = RegEnumValue(COMPORTS, i, 
					achValue, 
					&cchValue, 
					NULL, 
					NULL,
					NULL,
					NULL);
 
				if (retCode == ERROR_SUCCESS ) 
				{ 
					if(RegGetValue(HKEY_LOCAL_MACHINE,TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),TEXT(achValue),RRF_RT_REG_SZ,NULL,detectedPorts[i+1],&portSize)==ERROR_SUCCESS)
						printf("%d) %s\n",i+1, detectedPorts[i+1]);
				}
			}
			printf("Select the number against port of your choice:");
			scanf("%d",&portChoice);
			strcpy_s(dev_name,detectedPorts[portChoice]);
			printf("Selected port is %s\n",dev_name);
		}
	}
	//Close the Registry handle
	RegCloseKey(COMPORTS);
}

//backup of read function..
void readDataFromPort(void *ptr)
{
	//In this function, we are just reading byte by byte from the serial port and hence it is expected to be fast.
	//This function is given to _beginthread, as a parameter.

	PARSE_THREAD *pt = NULL;
	DWORD dwRead, dwRes;
	DWORD dwWaitFlag=EV_RXCHAR; //as an out parameter to the waitcommevent function.
	DWORD dwMask;
	int breakFlag = 1;//set
	unsigned char strCommand[300];
	int cntCommand = 0; //auto-init on 0, but i am doing it anyways.
	BOOL fWaitingOnRead = FALSE;
	OVERLAPPED osReader = { 0 };
	unsigned char bufRead[2] = { 0 };
	unsigned char lengthPacket;
	int startFrame = 0, endFrame = 0;
	int cntTotalPackets = 0;
	unsigned char actCommand;
	int liveStream = 0;
	unsigned int threadHandle;
	int rubbishData = 0;
	COMMTIMEOUTS com_timeout = { 0 };

	pt = (PARSE_THREAD *)malloc(sizeof(PARSE_THREAD));
	if (NULL == pt)
	{
		printf("\nError in allocating dynamic memory for parsing thread struct! Aborting...");
		exit(1);
	}
	//now we create an overlapped event and close the handle before we exit to avoid handle leak.
	if (!SetCommMask(hSerial, dwWaitFlag))
	{
		printf("\nError in setting event flags!");
		exit(1);
	}
	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (NULL == osReader.hEvent)
	{
		printf("\nError encountered while creating overlapped event! Aborting...\n");
		exit(1);
	}
	com_timeout.ReadIntervalTimeout = MAXDWORD; //this is the value that should get the blocked call to ReadFile -- out of the BLOCK!
	com_timeout.ReadTotalTimeoutMultiplier = MAXDWORD;
	com_timeout.ReadTotalTimeoutConstant = 500;
	if (!SetCommTimeouts(hSerial, &com_timeout))
	{
		printf("\nError in setting TIMEOUT for serial port!\nAborting...");
		exit(1);
	}
	while (1)
	{
		cntTotalPackets = 0;
		if (WaitCommEvent(hSerial, &dwMask, NULL))
		{
			/*if (!ResetEvent(osReader.hEvent))
			{
				printf("\nFailed in resetting event: %d", GetLastError());
			}*/
			/*if (GetCommMask(hSerial, &dwMask))
			{
			*/
			if (EV_RXCHAR & dwMask)
			{
				//printf("Character Arrived");
				//		breakFlag = 1; //set the flag here, so that it enters the loop when it is supposed to.
			}
			//}
			/*if (!fWaitingOnRead)
			{*/
			breakFlag = 1; //set the flag here, so that it enters the loop when it is supposed to.
			while (breakFlag)
			{
				//Issue the read operation
				dwRead = 0;//just for refreshing the read operation instead of over-writing the previous value.
				if (!ReadFile(hSerial, bufRead, 1, &dwRead, NULL))
				{
					
					//the readfile function has failed.
					if (GetLastError() != ERROR_IO_PENDING)
					{
						//Error in communication that you cannot handle.
						printf("\nFatal Error in GetLastError(). Report...");
					}
					else
					{
						//adding the GetOverlappedResult thingy
						dwRes = WaitForSingleObject(osReader.hEvent, 1000);
						switch (dwRes)
						{
							//indicates that read has completed.
						case WAIT_OBJECT_0:
							if (!GetOverlappedResult(hSerial, &osReader, &dwRead, TRUE))//making the last param true will make it wait until the read completes, simulating non-overlapped i/o
							{
								printf("\nFatal Error!\n");
								exit(1);
							}
							else
							{
								//read has completed successfully.
								//however, the number of times the control will ever come here is hardly anything. I think it is 0.
								//But safer to have this.
								bufRead[1] = '\0';
								if (1 == startFrame)
								{
									//copy into strCommand[] only if you are reading valid data that belongs to a frame.
									//but 0x9C will not be put into the array in that case, we handle this particular byte in switch below.
									strCommand[cntCommand++] = bufRead[0];
								}
								//writeDataToPort("dummy");
								//we write the Framing logic here...
								switch (bufRead[0])
								{
								case 0x9C:
									//this may or may not be the start frame, this can be data as well.
									if (0 == startFrame)
									{
										startFrame = 1;
										strCommand[cntCommand++] = bufRead[0]; //coyping the start byte manually.
									}
									else
									{
										//it is a part of data and not the startFrame
										//already been copied in the strCommand array, just before entering the switch.
									}
									break;
								case 0xC9:
									//again, this may or may not be an end frame.
									if ((cntCommand == lengthPacket) && (1 == startFrame))
									{
										//this means that we have encountered a byte that is the end of the current frame.
										startFrame = 0;
										endFrame = 1;
									}
									break;
								default:
									if (startFrame)
									{
										if (3 == cntCommand)
										{
											//this byte is the actual length of the arriving packet.
											lengthPacket = strCommand[cntCommand - 1]; //the value resides at position 2 in the array strCommand[]
										}
										else
										{
											if (4 == cntCommand)
											{
												actCommand = strCommand[cntCommand - 1];
											}
										}
									}
									else
									{
										//this means that rubbish data has been encountered at the port. so make the rubbishData flag as 1;
										rubbishData = 1;
									}
									break;
								}
								if (1 == endFrame)
								{
									//first assign the values to the needed variables and just before creating the thread, reset cntCommand.
									strCommand[cntCommand] = '\0';
									printf("\n\nDatagram read -> ");
									PrintCommand(strCommand, cntCommand);
									initParseStruct(pt, strCommand, cntCommand);
									cntTotalPackets++;
									cntCommand = 0;//to start reading the next frames into the same strCommand array.
									lengthPacket = 0;
									startFrame = 0;
									endFrame = 0;
									//now that all has been call the thread and pass pt to it.
									//_beginthread(*ParseFrame, 0, (void *)pt);
									if ((0x17 == actCommand) && (0xFF == strCommand[cntCommand - 2]))
									{
										liveStream = 1;
									}
									else
									{
										liveStream = 0;
									}
									if (0x17!=actCommand)
										ParseFrame((void*)pt);
									else
									{
										if (liveStream)
										{
											threadHandle=_beginthread(ParseFrame, 0, (void *)pt);
										}
										else
										{
											//means that command is 
											if (0x00==strCommand[cntCommand-2])
												TerminateThread((HANDLE)threadHandle, 0);
										}
									}
								}
							}
							break;
						case WAIT_TIMEOUT:
							//do not reset the fWaitingOnRead. 
							breakFlag = 0; //this is the point which will make the enclosing while loop terminate.
							break;
						default:
							printf("\nFatal Error in WaitForSingleObject! Probably some error in the overlapped structure! Aborting...");
							exit(1);
						}
					}
				}
				else
				{
					//this means that read has completed immediately.
					if (dwRead > 0)
					{
						bufRead[1] = '\0';
						if (1 == startFrame)
						{
							//copy into strCommand[] only if you are reading valid data that belongs to a frame.
							//but 0x9C will not be put into the array in that case, we handle this particular byte in switch below.
							strCommand[cntCommand++] = bufRead[0];
						}
						//writeDataToPort("dummy");

						//we write the Framing logic here...

						switch (bufRead[0])
						{
						case 0x9C:
							//this may or may not be the start frame, this can be data as well.
							if (0 == startFrame)
							{
								startFrame = 1;
								strCommand[cntCommand++] = bufRead[0]; //coyping the start byte manually.
							}
							else
							{
								//it is a part of data and not the startFrame
								//already been copied in the strCommand array, just before entering the switch.
							}
							break;
						case 0xC9:
							//again, this may or may not be an end frame.
							if ((cntCommand == lengthPacket) && (1 == startFrame))
							{
								//this means that we have encountered a byte that is the end of the current frame.
								startFrame = 0;
								endFrame = 1;
							}
							break;
						default:
							if (startFrame)
							{
								if (3 == cntCommand)
								{
									//this byte is the actual length of the arriving packet.
									lengthPacket = strCommand[cntCommand - 1]; //the value resides at position 2 in the array strCommand[]
								}
								else
								{
									if (4 == cntCommand)
									{
										actCommand = strCommand[cntCommand - 1];
									}
								}
							}
							else
							{
								//this means that rubbish data has been encountered at the port. so make the rubbishData flag as 1;
								rubbishData = 1;
							}
							break;
						}
						if (1 == endFrame)
						{
							//first assign the values to the needed variables and just before creating the thread, reset cntCommand.
							strCommand[cntCommand] = '\0';
							printf("\n\nDatagram read -> ");
							PrintCommand(strCommand, cntCommand);
							initParseStruct(pt, strCommand, cntCommand);
							cntTotalPackets++;
							cntCommand = 0;//to start reading the next frames into the same strCommand array.
							lengthPacket = 0;
							startFrame = 0;
							endFrame = 0;
							//now that all has been call the thread and pass pt to it.
							//_beginthread(*ParseFrame, 0, (void *)pt);
							if ((0x17 == actCommand) && (0xFF == strCommand[cntCommand - 2]))
							{
								liveStream = 1;
							}
							else
							{
								liveStream = 0;
							}
							if (0x17 != actCommand)
								ParseFrame((void*)pt);
							else
							{
								if (liveStream)
								{
									threadHandle = _beginthread(ParseFrame, 0, (void *)pt);
								}
								else
								{
									//means that command is 
									if (0x00 == strCommand[cntCommand - 2])
										_beginthread(ParseFrame, 0, (void *)pt);
								}
							}
						}
					}
					else
					{	
						//Now is the time to send NACK!!
						//dwBytes is 0.
						//So, if startFrame==1 OR rubbishData==1 then break, but first, re-initialize the required fields
						if ((1 == startFrame) || (1 == rubbishData))
						{
							cntCommand = 0;//to start reading the next frames into the same strCommand array.
							lengthPacket = 0;
							startFrame = 0;
							endFrame = 0;
							break;
						}
						else
							continue;
					}
					memset(bufRead, '\0', 2);
				} //end of else "read completes immediately case"

			}//end of while loop that fills up the buffer

			if ((startFrame) || (0 == cntTotalPackets) || ((0==startFrame) && (0==cntTotalPackets)) || (rubbishData))
			{
				rubbishData = 0;
				printf("\nErroneous bytes encountered or No data received!");
				printf("\nSending NACK!");
				writeDataToPort(NACK);
			}
		}//end of if waitcommevent
		else
		{
		}
	}
	CloseHandle(osReader.hEvent);// as we are closing the handle, we are not resetting the hEvent.
}

void initParseStruct(PARSE_THREAD *pt, unsigned char strCommand[300], int cntCommand)
{
	int i;
	for (i = 0; i < cntCommand; i++)
	{
		pt->Frame[i] = strCommand[i];
	}
	pt->count = i;
}
//void readDataFromPort(void *ptr)
//{
//	DWORD dwRead;
//	DWORD dMask;
//	DWORD dwRes;
//	int fWaitingOnRead = 0;//FALSE;
//	OVERLAPPED osReader = { 0 };
//	unsigned char buffer[500];
//	int breakFlag = 0; //used to break the below while loop
//	unsigned char szCommand[500];
//	int cntCommand=0;
//	int cntBytesRead = 0; //to track the number of bytes read by any particular instance of the thread.
//
//	//now we create an overlapped event, that must be closed before exiting to avoid handle leak.
//	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//	if (NULL == osReader.hEvent)
//	{
//		printf("\nFailed to created overlapped event!\nExiting...");
//		exit(1);
//	}
//	Sleep(5000);
//	while (1)
//	{
//		if (WaitCommEvent(hSerial, &dMask, &osReader))
//		{
//			//this is the first part of the operation where we detect the happening of a Read operation.
//			if (!fWaitingOnRead)
//			{
//				//init to FALSE, if we come here, we initiate the read operation, ie, if we are not waiting anymore for the read operation.
//				if (!ReadFile(hSerial, buffer, 1, &dwRead, &osReader))
//				{
//					//if the ReadFile returns FALSE, it better be for ERROR_IO_PENDING, otherwise report an error.
//					if (ERROR_IO_PENDING != GetLastError())
//					{
//						printf("\nInternal error occurred in ReadFile(). Exiting...");
//						exit(1);
//					}
//					else
//					{
//						//ERROR_IO_PENDING is true in this case, so make fWaitingOnRead TRUE
//						fWaitingOnRead = 1;// TRUE;
//					}
//				}
//				else
//				{
//					//If we are here, it means that ReadFile() has completed successfully and that now we have all the stuff in the buffer.
//					//dwRead now contains the count of the total number of bytes read.
//					if (dwRead > 0)
//						cntBytesRead++;
//					szCommand[cntCommand++] = buffer[0];
//				}
//			}
//			else
//			{
//				//this is the second part of the operation where fWaitingOnRead is TRUE. If we are waiting for a read, we do the following steps to detect its completion.
//				dwRes = WaitForSingleObject(osReader.hEvent, 500);//500 milliseconds is the read timeout.
//				switch (dwRes)
//				{
//					//if the read operation completes, WAIT_OBJECT_0 is the value in dRes
//				case WAIT_OBJECT_0:
//					if (!GetOverlappedResult(hSerial, &osReader, &dwRead, FALSE))
//					{
//						//if the function returns 0 and we are here, it implies that there's been an internal error in the communication
//						printf("\nInternal Error in Communication. Exiting...");
//						exit(1);
//					}
//					else
//					{
//						//the read has completed successfully, handle it.
//						if (dwRead > 0)
//							cntBytesRead++;
//						szCommand[cntCommand++] = buffer[0];
//					}
//					//reset the flag so that we can again reset this operation for the next occurrence
//					fWaitingOnRead = 0;
//					break;
//				case WAIT_TIMEOUT:
//					//let's visit the loop again by giving a continue. We will be acting a bit impatient, but never mind.
//					//otherwise we would have done some background task. But we don't have any background task.
//					breakFlag = 1;
//					break;
//				default:
//					printf("\nInternal Error encountered. \nExiting...");
//					break;
//				}
//			}
//		}
//	}
//	if (cntBytesRead)
//	{
//		PrintCommand(szCommand, cntCommand);
//		ConsolidateCommands(szCommand, cntCommand);
//	}
//	else
//	{
//		printf("\nNo data arrived yet!");
//	}
//	CloseHandle(osReader.hEvent);
//	newThreadSwitch = 1;//this particular thread is exiting, now we need a new thread.
//	_endthread();
//}



