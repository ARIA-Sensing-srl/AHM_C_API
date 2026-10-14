/*
 * wiringSerial.c:
 *	Handle a serial port
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/WiringPi/WiringPi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <Windows.h>



#include "wiringSerial.h"

/*
 * serialOpen:
 *	Open and initialise the serial port, setting all the right
 *	port parameters - or as many as are required - hopefully!
 *********************************************************************************
 */

HANDLE serialOpen (const char *device, const int baud)
{
	HANDLE hComm;
	hComm = CreateFile(device,        // port name
		GENERIC_READ | GENERIC_WRITE, // Read/Write
		0,                            // No Sharing
		NULL,                         // No Security
		OPEN_EXISTING,                // Open existing port only
		0,                            // Non Overlapped I/O
		NULL);                        // Null for Comm Devices

	if (hComm == INVALID_HANDLE_VALUE)
		return hComm;


	//Configuring the DCB structure for Serial Comm

	DCB DCB_Struct_Parameter = { 0 }; //Zero out all the DCB struct members before calling GetCommState()

	DCB_Struct_Parameter.DCBlength = sizeof(DCB_Struct_Parameter); // Get the size of the DCB structure using sizeof()
	// update .DCBlength member with that size
	// required by GetCommState()


	BOOL status = GetCommState(hComm, &DCB_Struct_Parameter); // Get the current settings for the port hComm
	// and fill it in the DCB_Struct_Parameter struct

	if (status == FALSE){
		serialClose(hComm);
		return INVALID_HANDLE_VALUE;
	}

	// Fill in the DCB with required settings 
	// Set Parameters in 8N1 format,8 data bits ,No Parity, 1 stop bit

	DCB_Struct_Parameter.BaudRate = baud;       // other rates 4800,19200,38400etc 
	DCB_Struct_Parameter.ByteSize = 8;          // data word length = 8 bits per byte.Common values are 5, 6, 7, or 8
	DCB_Struct_Parameter.Parity = NOPARITY;   // NOPARITY, ODDPARITY, EVENPARITY, MARKPARITY, or SPACEPARITY
	DCB_Struct_Parameter.StopBits = ONESTOPBIT; // ONESTOPBIT, ONE5STOPBITS, or TWOSTOPBITS

	status = SetCommState(hComm, &DCB_Struct_Parameter); //update the changed parameters to dcb 

	if (status == FALSE){
		serialClose(hComm);
		return INVALID_HANDLE_VALUE;
	}

	// Setting Read Time outs for Serial Read operation.

	COMMTIMEOUTS timeouts = { 0 }; //Zero out the COMMTIMEOUTS structure

	timeouts.ReadIntervalTimeout = 20; // Max time between bytes (ms)
	timeouts.ReadTotalTimeoutMultiplier = 1;  // Per-byte timeout
	timeouts.ReadTotalTimeoutConstant = 50; // Constant timeout (ms)

	timeouts.WriteTotalTimeoutMultiplier = 1; // Per-byte timeout
	timeouts.WriteTotalTimeoutConstant = 50;// Constant timeout (ms)

	if (SetCommTimeouts(hComm, &timeouts) == FALSE){
		serialClose(hComm);
		return INVALID_HANDLE_VALUE;
	}


	//Clear both the TX and RX buffer
	PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR); //purge both tx and rx buffer
	
	Sleep(0.1);

  return hComm ;
}


/*
 * serialFlush:
 *	Flush the serial buffers (both tx & rx)
 *********************************************************************************
 */

void serialFlush (HANDLE fd)
{
  //Clear both the TX and RX buffer
  PurgeComm(fd, PURGE_RXCLEAR | PURGE_TXCLEAR); //purge both tx and rx buffer
}


/*
 * serialClose:
 *	Release the serial port
 *********************************************************************************
 */

void serialClose (HANDLE fd)
{
   CloseHandle(fd);
}


/*
 * serialPutchar:
 *	Send a single character to the serial port
 *********************************************************************************
 */

void serialPutchar(HANDLE fd, const unsigned char c)
{
	DWORD numBytesWritten;
	
	if (!WriteFile(fd, &c, 1, &numBytesWritten, NULL)){
		printf("Error on writing serial port\n");
	}
}


/*
 * serialPuts:
 *	Send a string to the serial port
 *********************************************************************************
 */

void serialPuts(HANDLE fd, const char *s)
{
    size_t len = strlen(s);
    
	DWORD numBytesWritten;
	
	if (!WriteFile(fd, s, len, &numBytesWritten, NULL)){
		printf("Error on writing serial port\n");
	}
}

/*
 * serialPrintf:
 *	Printf over Serial
 *********************************************************************************
 */

void serialPrintf (HANDLE fd, const char *message, ...)
{
  va_list argp ;
  char buffer [1024] ;

  va_start (argp, message) ;
    vsnprintf (buffer, 1023, message, argp) ;
  va_end (argp) ;

  serialPuts (fd, buffer) ;
}


/*
 * serialDataAvail:
 *	Return the number of bytes of data avalable to be read in the serial port
 *********************************************************************************
 */

int serialDataAvail (HANDLE fd)
{	
	DWORD   Errors;
	COMSTAT Stat;
	ClearCommError(fd, &Errors, &Stat);
	return Stat.cbInQue;
}


/*
 * serialGetchar:
 *	Get a single character from the serial device.
 *	Note: Zero is a valid character and this function will time-out after
 *	10 seconds.
 *********************************************************************************
 */

int serialGetchar (HANDLE fd)
{
  uint8_t x ;
  DWORD dwRead;
	
  if (!ReadFile(fd, &x, 1, &dwRead, NULL)){
	  return -1;
  }

  return ((int)x) & 0xFF ;
}
