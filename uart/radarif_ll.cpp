/*
 * radarif_ll.c
 *
 *  Created on: 03 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */


#include <stdlib.h>
#include "radarif_ll.h"
#include <string.h>
#include "msgEncoderDecored.h"

#define RIFLL_START 0xFF
#define RIFLL_STOP 0x00

rifll_h::rifll_h(int (*tx_cb)(uint8_t *, int), int (*rx_cb)(uint8_t *, int), int rxBufferSize){
	//setup handler
	this->rx_cb = rx_cb;
	this->tx_cb = tx_cb;
	this->tx2rx_wait_us = 0;
	//alloc buffer
	if (!rxBufferSize)
		rxBufferSize = RIFLL_INIT_DEF_BUFFER;
	uint8_t * pdata = (uint8_t*)malloc(rxBufferSize);
	//allocation done
	this->rxbuf.pdata=pdata;
	this->rxbuf.bufsize = rxBufferSize;
	configured = 1;

}

rifll_h::rifll_h(){
}


int rifll_h::set_tx2rx_wait(unsigned int waitTimeus){
	tx2rx_wait_us = waitTimeus;
	return 0;
}
unsigned int rifll_h::get_tx2rx_wait(){
	return tx2rx_wait_us;
}

//use txdata == NULL to rx only
int rifll_h::txrxMessage(uint8_t * txdata, int txsize, uint8_t * rxdataBuf, int rxsize){

	if (!this->configured){
		return RIFLL_NOT_INIT;
	}
	int ret;
	if (txdata){
		int encLgth = MSGComputeEncodedLength(txdata,txsize) +2; //add start and stop in size computation
		if (encLgth > this->rxbuf.bufsize)
			return RIFLL_ERROR;
		//encode message
		MSGEncode(txdata,  this->rxbuf.pdata+1, txsize);
		this->rxbuf.pdata[0]=RIFLL_START;
		this->rxbuf.pdata[encLgth-1] = RIFLL_STOP;
		ret = this ->tx_cb(this->rxbuf.pdata, encLgth);
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
		ret = this->rx_cb(&curChar, 1);
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


rifll_h::~rifll_h(){
	//dealloc
	if (this->rxbuf.pdata){
		free((void*)this->rxbuf.pdata);
	}
	this->configured = 0;
}
