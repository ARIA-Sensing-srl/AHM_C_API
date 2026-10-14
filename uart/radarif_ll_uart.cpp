/*
 * radarif_ll_spi.cpp
 *
 *  Created on: 27 gen 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
 */




#include "radarif_ll_uart.h"
#include "stdlib.h"
#include "string.h"
#include "msgEncoderDecored.h"
#include <stdio.h>

#ifdef WIN
#define usleep(x) Sleep(x/1000)
#endif

#define RIFLL_START 0xFF
#define RIFLL_STOP 0x00


radarif_ll_uart::radarif_ll_uart(std::string comboard, int baudrate){
	serial_fd = serialOpen(comboard.c_str(), baudrate);
	configured = 1;
#ifdef WIN
	if (serial_fd == INVALID_HANDLE_VALUE){
		configured = 0;
	}
#else
	if (serial_fd == -1){
		configured = 0;
	}
#endif
	
	
	int	rxBufferSize = RIFLL_INIT_DEF_BUFFER;
	uint8_t * pdata = (uint8_t*)malloc(rxBufferSize);
	//allocation done
	this->rxbuf.pdata=pdata;
	this->rxbuf.bufsize = rxBufferSize;
}



int radarif_ll_uart::txdata(uint8_t * buffer, int size){
    for(int i=0; i < size; i++){
        serialPutchar(serial_fd, *buffer++);
    }
    return 0;
}


int radarif_ll_uart::rxdata(uint8_t * buffer, int size){
    int ret;
    int retry = 0;
    do{
    ret = serialDataAvail(serial_fd);
	
        if (ret > 0){
            *buffer = serialGetchar(serial_fd);
             buffer += 1;
               size -= 1;
        }
        if (ret <= 0){
            usleep(1000);
            retry++;
        }
    }while(size && (retry < 1000));

    if (ret)
        return ret;
    return -1; //if ret == 0 error
}
int radarif_ll_uart::txrxMessage(uint8_t * ptxdata, int txsize, uint8_t * rxdataBuf, int rxsize){
	if (!this->configured){
		return RIFLL_NOT_INIT;
	}
	int ret;
	if (ptxdata){
		int encLgth = MSGComputeEncodedLength(ptxdata,txsize) +2; //add start and stop in size computation
		if (encLgth > this->rxbuf.bufsize)
			return RIFLL_ERROR;
		//encode message
		MSGEncode(ptxdata,  this->rxbuf.pdata+1, txsize);
		this->rxbuf.pdata[0]=RIFLL_START;
		this->rxbuf.pdata[encLgth-1] = RIFLL_STOP;
		ret = this->txdata(this->rxbuf.pdata, encLgth);
		if (ret < 0){
			return RIFLL_ERROR;
		}
	}
	//ok, receive message
	int rxData = 0;
	uint8_t * ptemp;
	ptemp = this->rxbuf.pdata;
	uint8_t curChar;
	while(this->rxbuf.bufsize - rxData){
		ret = this->rxdata(&curChar, 1);
		if (ret <= 0){
			return RIFLL_ERROR; //comm error or no data returned
		}

		if (!rxData){
			//check start
			if (curChar != RIFLL_START){
				continue;
			}
		}
		//store data
		*ptemp++ = curChar;
		rxData++;

		//check stop condition
		if (curChar == RIFLL_STOP){
			break;
		}
	}
	//data received correctly
	ret = MSGDecode(this->rxbuf.pdata+1, this->rxbuf.pdata+1, rxData-2);
	if (ret < 0){
		return RIFLL_ERROR;
	}
	if (rxdataBuf == NULL){
		//use direct access to data
		return ret;
	}
	if (ret > rxsize){
		//copy all into destination buffer
		memcpy((void*)rxdataBuf, this->rxbuf.pdata+1, rxsize);
		return RIFLL_TRUNCATE;
	}
	//copy all
	memcpy((void*)rxdataBuf, this->rxbuf.pdata+1, ret);
	return ret; //OK return the number of data received
}
radarif_ll_uart::~radarif_ll_uart(){
	if (configured)
		serialClose(serial_fd);
}
