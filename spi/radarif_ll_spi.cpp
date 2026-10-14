/*
 * radarif_ll_spi.cpp
 *
 *  Created on: 27 gen 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */




#include "radarif_ll_spi.h"
#include "spi_interface.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"


radarif_ll_spi::radarif_ll_spi(char * pif, int ch, int speed_Hz, int rxBufferSize){
	this->channel = ch;
	SPIInt_init(pif, speed_Hz, channel);
	if (!rxBufferSize)
		rxBufferSize = RIFLL_INIT_DEF_BUFFER;
	uint8_t * pdata = (uint8_t*)malloc(rxBufferSize);
	this->rxbuf.pdata=pdata;
	this->rxbuf.bufsize = rxBufferSize;
}
int radarif_ll_spi::txrxMessage(uint8_t * txdata, int txsize, uint8_t * rxdata, int rxsize){
	SPIInt_sendMessage(txdata, txsize);

	if (this->tx2rx_wait_us > 0)
		usleep(tx2rx_wait_us); //add time

	//NOTE: for compatibility with UART, data payload is expected to byte 1
	//this tweack is required for functions that use direct access to rxbuf
	int ret = SPIInt_getResponse(rxbuf.pdata+1, rxbuf.bufsize-1, 1000);
	if (ret > 0){
		//if rxdata is NULL, calleer want to use the internal buffer
		if (rxdata != NULL){
			if (ret > rxsize){
				ret = rxsize;
			}
			memcpy((void*)rxdata, (const void*)rxbuf.pdata+1, ret);
		}
		return ret;
	}
	return -1;
}
radarif_ll_spi::~radarif_ll_spi(){
	SPIInt_close();
}
