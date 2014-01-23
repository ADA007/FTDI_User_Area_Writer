// I2CTEST.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>


//============================================================================
//  Use of FTDI D2XX library:
//----------------------------------------------------------------------------
//  Include the following 2 lines in your header-file
#pragma comment(lib, "FTD2XX.lib")
#include "FTD2XX.h"
//============================================================================


#include <stdlib.h>

	FT_STATUS ftStatus;			//Status defined in D2XX to indicate operation result
	FT_HANDLE ftHandle;			//Handle of FT2232H device port 
	//DWORD dwClockDivisor = 0x004A;  	//Value of clock divisor, SCL Frequency = 60/((1+0x004A)*2) (MHz) = 400khz
	DWORD dwClockDivisor = 0x0095;   //200khz

	BYTE ByteDataRead;//ByteAddress
	BYTE ByteAddresshigh = 0x00, ByteAddresslow = 0x80;		//EEPROM address is '0x0080'
	BYTE ByteDataToBeSend = 0x5A;							//data programmed and read

	DWORD dwNumDevs;				// The number of devices
	unsigned int uiDevIndex = 0xF;	// The device in the list that we'll use

	BYTE byOutputBuffer[1024];		// Buffer to hold MPSSE commands and data to be sent to the FT2232H
	BYTE byInputBuffer[1024];		// Buffer to hold data read from the FT2232H

	DWORD dwCount = 0;				// General loop index
	DWORD dwNumBytesToSend = 0;		// Index to the output buffer
	DWORD dwNumBytesSent = 0;		// Count of actual bytes sent - used with FT_Write
	DWORD dwNumBytesToRead = 0;		// Number of bytes available to read in the driver's input buffer
	DWORD dwNumBytesRead = 0;		// Count of actual bytes read - used with FT_Read



int _tmain(int argc, _TCHAR* argv[])
{

	printf("Checking for FTDI devices...\n");

	int	i, n, z, j, counter = 0;
	const int MAX_DEVICES = 1;
	int	iNumDevs = 0;
	const int BUF_SIZE = 16;
	char 	cBufWrite[BUF_SIZE];
	char * 	pcBufRead = NULL;
	char * 	pcBufLD[MAX_DEVICES + 1];
	char 	cBufLD[MAX_DEVICES][64];
	//ftStatus = FT_Open(cBufLD, &ftHandle);

	for(i = 0; i < MAX_DEVICES; i++) {
		pcBufLD[i] = cBufLD[i];
		}
	pcBufLD[MAX_DEVICES] = NULL;

	ftStatus = FT_ListDevices(pcBufLD, &iNumDevs, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);

	if(ftStatus != FT_OK) {
		printf("Error: FT_ListDevices(%d)\n", ftStatus);
		return 1;
	}
	for(j = 0; j < BUF_SIZE; j++) {
		cBufWrite[j] = j;
	}

	for(i = 0; ( (i <MAX_DEVICES) && (i < iNumDevs) ); i++) {
		printf("Device %d Serial Number - %s\n", i, cBufLD[i]);
	}
	
	//ftStatus = FT_OpenEx(cBufLD, FT_OPEN_BY_SERIAL_NUMBER, &ftHandle);
	ftStatus = FT_OpenEx(cBufLD, FT_OPEN_BY_SERIAL_NUMBER, &ftHandle);
	if (ftStatus != FT_OK)
	{
		printf("Open Failed with error %d\n", ftStatus);
		return 1;					// Exit with error
	}
	printf("Opened device %s\n", cBufLD);
	// Configure port parameters

	ftStatus |= FT_ResetDevice(ftHandle);
									//Reset USB device

	printf("Reading...\n");
	//ftStatus = FT_Read(ftHandle, &byInputBuffer, 52, &dwNumBytesRead);
				
	DWORD EEUA_Size = 0;
	ftStatus = FT_EE_UASize(ftHandle, &EEUA_Size);
	//Read UA EEPROM
	if(ftStatus == FT_OK)
	{
		// Read EEPROM DaTA
		unsigned char* Buffer= new unsigned char [EEUA_Size];
		DWORD BytesRead;
		ftStatus = FT_EE_UARead(ftHandle, Buffer, EEUA_Size, &BytesRead);
		if (ftStatus == FT_OK)
		{
			// FT_EE_UARead OK
			// User Area data stored in Buffer
			// Number of bytes read stored in BytesRead
			for (DWORD i=0;i<BytesRead;i++)
			{
				printf("0x%02x ",Buffer[i]);
			}
			printf("\n Read bytes %i \n", BytesRead);
		}
		else
		{
			printf("FT_EE_UARead FAILED!\n"); // FT_EE_UARead FAILED!
		}
	}
	// Write UA EEPROM
	if(ftStatus == FT_OK)
	{
		//unsigned char Buffer[EEUA_Size];
		//unsigned char* WRBuffer= new unsigned char [EEUA_Size+1];
		DWORD BytesWrite;
		/*for(int i=0;i<EEUA_Size;i++){
			WRBuffer[i] = i;
		}*/
		unsigned char WRBuffer[]= {0x01 ,0x00 ,0xc7 ,0x92 ,0x6a ,0x35 ,0x52 ,0x01 ,0x90 ,0x30 ,0x4a ,0x74 ,0x61 ,0x67 ,0x48 ,0x73 ,
					0x32 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x44 ,0x69 ,0x67 ,0x69 ,0x6c ,
					0x65 ,0x6e ,0x74 ,0x20 ,0x4a ,0x54 ,0x41 ,0x47 ,0x2d ,0x48 ,0x53 ,0x32 ,0x00 ,0x00 ,0x00 ,0x00 ,
					0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x11 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
					0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
					0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
					0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x48 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
					0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
					0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
					0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
		ftStatus = FT_EE_UAWrite(ftHandle, WRBuffer, EEUA_Size);
		if (ftStatus == FT_OK)
		{
			printf("FT_EE_UAWrite Writed sucessfully!\n");// FT_EE_UAWrite OK
		}
		else
		{
			printf("FT_EE_UAWrite FAILED!\n");// FT_EE_UAWrite FAILED!
		}	

		//Read writed DATA
		unsigned char* Buffer= new unsigned char [EEUA_Size];
		DWORD BytesRead;
		ftStatus = FT_EE_UARead(ftHandle, Buffer, EEUA_Size, &BytesRead);
		if (ftStatus == FT_OK)
		{
			// FT_EE_UARead OK
			// User Area data stored in Buffer
			// Number of bytes read stored in BytesRead
			for (DWORD i=0;i<BytesRead;i++)
			{
				printf("0x%02x ",Buffer[i]);
			}
			printf("\n Read bytes %i \n", BytesRead);
		}
		else
		{
			printf("FT_EE_UARead FAILED!\n"); // FT_EE_UARead FAILED!
		}

	}



	printf("\nJTAG program executed successfully.\n");
	printf("Press <Enter> to continue\n");
	getchar();

	FT_Close(ftHandle);

	//printf("failed to read data byte [Read Section]!\n");
	//while(1);
	return 0;
}

