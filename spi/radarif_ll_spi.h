/*
 * radarif_ll_spi.h
 *
 *  Created on: 27 gen 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */

#ifndef RADARIF_LL_SPI_H_
#define RADARIF_LL_SPI_H_

#include "radarif_ll.h"



class radarif_ll_spi : public rifll_h {

private:
	int channel;

public:
	radarif_ll_spi(char * pif, int channel, int speed_Hz, int rxBuffer);
	int txrxMessage(uint8_t * txdata, int txsize, uint8_t * rxdata, int rxsize);
	~radarif_ll_spi();
};


#endif /* RADARIF_LL_SPI_H_ */
