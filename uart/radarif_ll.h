/*
 * radarif_ll.h
 *
 *  Created on: 03 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
 */

#ifndef RADARIF_LL_H
#define RADARIF_LL_H

#include <stdint.h>



class rifll_h{

private:
	

protected:
	int tx2rx_wait_us; //used by SPI to set small interval between Tx and Rx, igonred by UART
	uint8_t configured;

public:
	int (*tx_cb)(uint8_t*, int);
	int (*rx_cb)(uint8_t*, int);
	struct {
		uint8_t * pdata;
		int bufsize;
	}rxbuf;
	virtual int txrxMessage(uint8_t * txdata, int txsize, uint8_t * rxdata, int rxsize);
	rifll_h(int (*tx_bc)(uint8_t *, int), int (*rx_cb)(uint8_t *, int), int rxBufferSize);
	rifll_h();
	int set_tx2rx_wait(unsigned int waitTimeus);
	uint8_t isValid() {return configured;};
	unsigned int get_tx2rx_wait();
	virtual ~rifll_h();
};




#define RIFLL_INIT_DEF_BUFFER (100*1024)
#define RIFLL_DATA_BUFFER_INDEX_START 1 //position where data are valid into buffer handler


#define RIFLL_SUCCESS 0
#define RIFLL_ERROR -1
#define RIFLL_TRUNCATE -2
#define RIFLL_NOT_INIT -3
#define RIFLL_INVALID_PARAMS -4

#endif
