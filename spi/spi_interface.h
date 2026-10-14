/*
 * spi_interface.h
 *
 *  Created on: Jun 3, 2024
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
 */

#ifndef INCLUDE_SPI_INTERFACE_H_
#define INCLUDE_SPI_INTERFACE_H_

#include "stdint.h"

int SPIInt_init(char * pif, int speed_Hz, int channel);
int SPIInt_close();
int SPIInt_sendMessage(unsigned char * pbuffer, int size);
int SPIInt_getResponse(unsigned char * pbuffer, int size, int timeout_ms);


#endif /* INCLUDE_SPI_INTERFACE_H_ */
