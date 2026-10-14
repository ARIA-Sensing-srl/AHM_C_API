/*
 * spi_interface.c
 *
 *  Created on: Jun 3, 2024
 *      Author: andream
 */


#include "spi_interface.h"
#include "ftd2xx.h"
#include "libmpsse_spi.h"
#include "string.h"
#include <sys/time.h>
#include "stdbool.h"
#include "stdio.h"

#define RD_MEM_CMD 0x0B
#define WR_MEM_CMD 0x02
#define DUMMY_BITS 32
#define SEM_H2D_FLAGS (0x01000001UL)
#define SEM_D2H_FLAGS (0x00010100UL)
#define DATA_SIZE_CONT_FLAG 0x8000


static DWORD address_semaphore = 0x1C072100UL;
static DWORD address_buffer = 0x1C072104UL;
static const unsigned char rdRegTable[4] __attribute__((unused)) = {0x05, 0x07, 0x21, 0x31} ;
static const unsigned char wrRegTable[4] = {0x01, 0x11, 0x20, 0x30};
static FT_HANDLE handler;


static int priv_writeRegister(FT_HANDLE, UCHAR reg, UCHAR value);
static int priv_readRegister(FT_HANDLE, UCHAR reg, UCHAR * value);
static int priv_writeMem(FT_HANDLE, DWORD address, UCHAR * pbuffer, int size);
static int priv_readMem(FT_HANDLE, DWORD address, UCHAR * pbuffer, int size);
static int priv_readStatus(FT_HANDLE, DWORD * pstatus);
static int priv_writeStatus(FT_HANDLE, DWORD status);


int SPIInt_init(char * dev, int speed_Hz, int channel){
	FT_STATUS ftStatus;
	Init_libMPSSE();
	DWORD numChannels;
	SPI_GetNumChannels(&numChannels);
	//printf("%d", numChannels);
	if (numChannels == 0){
		printf("No channels available");
		return -1;
	}
	ChannelConfig chConfig;
	chConfig.ClockRate = speed_Hz;
	chConfig.LatencyTimer = 1;
	chConfig.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 |
			SPI_CONFIG_OPTION_CS_ACTIVELOW;
	chConfig.Pin = 0;
	chConfig.currentPinState = 0;
	ftStatus = SPI_OpenChannel(channel,&handler);
	if (ftStatus != FT_OK){
		printf("Error while opening channel\n");
		return -1;
	}
	ftStatus = SPI_InitChannel(handler, &chConfig);
	if (ftStatus != FT_OK){
		printf("Error\n");
		return -1;
	}

	priv_writeRegister(handler, 1, DUMMY_BITS-1);
	volatile UCHAR config;
	priv_readRegister(handler, 1, (UCHAR *) &config);
	if (config != DUMMY_BITS-1){
		SPIInt_close();
		return -1;
	}

	return 0;

}
int SPIInt_close(){
	SPI_CloseChannel(handler);
	Cleanup_libMPSSE();
	return 0;
}
int SPIInt_sendMessage(unsigned char * pbuffer, int size){
	DWORD status;
	priv_readStatus(handler, &status);
	if (status & SEM_H2D_FLAGS){
		return -1; //busy
	}
	//buffer is free
	UCHAR buf[size +2];
	buf[0] = size & 0xFF;
	buf[1] = size>>8;
	memcpy((void*)&buf[2], (const void*)pbuffer, size);
	int ret = priv_writeMem(handler, address_buffer, buf, sizeof(buf));
	if (ret)
		return -1;
	return priv_writeStatus(handler, SEM_H2D_FLAGS);
}
int SPIInt_getResponse(unsigned char * pbuffer, int size, int timeout_ms){
	struct timeval stop, start;
	int totTransferData = 0;
	for (;;){
		gettimeofday(&start, NULL);
		DWORD status;
		do{
			priv_readStatus(handler, & status);
			gettimeofday(&stop, NULL);
			long int elapsedTime = (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec)/1000;
			if (elapsedTime > timeout_ms){
				break;
			}
		}while(!(status & SEM_D2H_FLAGS));
		if (!(status & SEM_D2H_FLAGS))
			return -1; //exit for timeout
		if (status == 0xFFFFFFFF)
			return 0; //abnormal

		//data available
		DWORD dataAvailable;
		if (priv_readMem(handler, address_buffer, (UCHAR*)&dataAvailable, 4))
			return -1;

		dataAvailable &= 0xFFFF;
		bool partial = (dataAvailable & DATA_SIZE_CONT_FLAG);
		dataAvailable &= ~DATA_SIZE_CONT_FLAG;
		UCHAR tdata[dataAvailable+2];
		if (priv_readMem(handler, address_buffer, tdata, dataAvailable+2)){
			return -1;
		}
		//check if I can copy all data
		if (size >= dataAvailable){
			memcpy((void*)pbuffer, (const void*) &tdata[2], dataAvailable);
			pbuffer += dataAvailable;
			size -= dataAvailable;
			totTransferData += dataAvailable;
		}else{
			if (size >0 ){
				memcpy((void*)pbuffer, (const void*) &tdata[2], size);
				pbuffer += size;
				size =0;
				totTransferData += size;
			}
		}
		priv_writeStatus(handler, 0); //free buffer
		if (!partial)
			break;
	}

	return totTransferData;
}



static int priv_writeRegister(FT_HANDLE h, UCHAR reg, UCHAR value){
	UCHAR iobuffer[2];
	memset((void*)iobuffer, 0 , sizeof(iobuffer));
	DWORD trx;
	iobuffer[0] = wrRegTable[reg];
	iobuffer[1] = value;
	FT_STATUS status = SPI_ReadWrite(h, iobuffer, iobuffer,sizeof(iobuffer), &trx, SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE |
				SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
				SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (status != FT_OK)
		return -1;
	return 0;
}

static int priv_readRegister(FT_HANDLE h, UCHAR reg, UCHAR * value){
	UCHAR iobuffer[5];
	DWORD trx;
	memset((void*)iobuffer, 0, sizeof(iobuffer));
	iobuffer[0] = rdRegTable[reg];
	FT_STATUS status = SPI_ReadWrite(h, iobuffer, iobuffer, 5, &trx, SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE |
				SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
				SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (status != FT_OK)
		return -1;
	*value = iobuffer[4];

	return 0;
}

static int priv_writeMem(FT_HANDLE h, DWORD address, UCHAR * pbuffer, int size){
	DWORD txfrSize = size;
	if (txfrSize  & 0x3){
		txfrSize = (txfrSize & ~0x3)+4;
	}
	//NB write MEM hasn't dummy
	UCHAR pIObuffer[txfrSize + 5];
	memset((void*)pIObuffer, 0, sizeof(pIObuffer));
	DWORD txfr;
	pIObuffer[0] = WR_MEM_CMD;
	pIObuffer[1] = (address >> 24) & 0xFF;
	pIObuffer[2] = (address >> 16) & 0xFF;
	pIObuffer[3] = (address >> 8) & 0xFF;
	pIObuffer[4] = (address >> 0) & 0xFF;
	memcpy((void*)&pIObuffer[5], (const void*)pbuffer, size);
	FT_STATUS status = SPI_ReadWrite(h, pIObuffer, pIObuffer, sizeof(pIObuffer), &txfr, SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE |
				SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (status != FT_OK)
		return -1;
	return 0;
}
static int priv_readMem(FT_HANDLE h, DWORD address, UCHAR * pbuffer, int size){
	DWORD txfrSize = size;
	if (txfrSize  & 0x3){
		txfrSize = (txfrSize & ~0x3)+4;
	}

	UCHAR pIObuffer[txfrSize + 5 + DUMMY_BITS/8];
	memset((void*)pIObuffer, 0, sizeof(pIObuffer));
	DWORD txfr;
	pIObuffer[0] = RD_MEM_CMD;
	pIObuffer[1] = (address >> 24) & 0xFF;
	pIObuffer[2] = (address >> 16) & 0xFF;
	pIObuffer[3] = (address >> 8) & 0xFF;
	pIObuffer[4] = (address >> 0) & 0xFF;
	for (int i = 0; i < DUMMY_BITS/8; i++){
		pIObuffer[5+i] = 0;
	}
	FT_STATUS status = SPI_ReadWrite(h, pIObuffer, pIObuffer, sizeof(pIObuffer), &txfr, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE | SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	if (status != FT_OK)
		return -1;

	memcpy((void *)pbuffer, (const void*)&pIObuffer[5+(DUMMY_BITS/8)], size);
	return 0;
}

static int priv_readStatus(FT_HANDLE h, DWORD * pstatus){
	UCHAR buf[4];
	int ret = priv_readMem(h, address_semaphore, buf, 4);
	if (ret)
		return ret;
	*pstatus = buf[0];
	*pstatus|= buf[1] << 8;
	*pstatus |= buf[2] << 16;
	*pstatus |= buf[3] << 24;
	return 0;
}

static int priv_writeStatus(FT_HANDLE h, DWORD status){
	UCHAR buf[4];
	buf[0] = status & 0xFF;
	buf[1] = (status >>8) & 0xFF;
	buf[2] = (status >> 16) & 0xFF;
	buf[3] = (status >> 24) & 0xFF;
	return priv_writeMem(h, address_semaphore, buf, 4);
}

