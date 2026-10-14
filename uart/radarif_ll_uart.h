/*
 * radarif_ll_uart.h
 *
 *  Created on: 27 gen 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
 */

#ifndef RADARIF_LL_UART_H_
#define RADARIF_LL_UART_H_

#include "radarif_ll.h"
#include <cstdint>
#include <string>
#include "wiringSerial.h"

#ifndef WIN
#include "unistd.h"
#else
#include <windows.h>
#include "synchapi.h"
#endif

#ifdef WIN
#include <Windows.h>
typedef HANDLE hserial;
#else
typedef int hserial;
#endif





class radarif_ll_uart : public rifll_h {

private:
	hserial serial_fd;
    int txdata(uint8_t * buffer, int size);
    int rxdata(uint8_t * buffer, int size);

public:
	radarif_ll_uart(std::string comboard, int baudrate);
	int txrxMessage(uint8_t * txdata, int txsize, uint8_t * rxdata, int rxsize);
	~radarif_ll_uart();
};


#endif /* RADARIF_LL_UART_H_ */
