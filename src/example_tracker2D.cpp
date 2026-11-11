/*
 * main.c
 *
 *  Created on: 23 feb 2021
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
 */


#include "stdio.h"
#ifndef WIN
#include "unistd.h"
#include <sys/time.h>
#include <signal.h>
#else
#include <windows.h>
#include "synchapi.h"
#include <atomic>
#endif
#include <iostream>
#include "wiringSerial.h"
#include "radarif_ll.h"
#include "radarif_ll_uart.h"
#include "radar_AHM2D.h"
#include "radar_AHM2DL.h"
#include "ahm_comm_def.h"
#ifdef CONFIG_USE_SPI
#include "radarif_ll_spi.h"
#endif
#include "string.h"

#ifdef WIN
#define usleep(x) Sleep(x/1000)
#define sleep(x) Sleep(x*1000)
static std::atomic<bool> stopthread;

#else

static volatile bool stopthread;
#endif

static radar_api_base * rdr_h;




static int configure_rdr(radar_api_base * h){

	uint16_t hwCode;
	if (h->getHW(&hwCode)){
		printf("Error on get HW model\n");
		return -1;
	}
	if (hwCode != h->HWCODE){
		printf("Invalid HW code, verify connected HW type\n");
		return -1;
	}

	if (h->rdr_stop()){
		printf("Stop failed\n");
		return -1;
	}

	if (h->rdr_set_profile(PROF_SR_MID_RES)) {
		printf("Set profile failed\n");
		return -1;
	}
	sleep(1); //required for correct setup



	if (h->rdr_set_staticObjRemoval(1)) {
		printf("Set static object removal failed\n");
		return -1;
	}

	if (h->rdr_set_staticObjMapUpdateTime(40)) {
		printf("Set static object map update time failed\n");
		return -1;
	}

	if (h->rdr_set_embeddedImgAlgo(1)) {
		printf("Embedded image algorithm set fails\n");
		return -1;
	}

	volatile uint8_t val = 0;
	h->rdr_get_embeddedImgAlgo((uint8_t*)&val);

	float maxDistance = 5;

	//acquisition range (meters)
	if (h->rdr_set_min_Xrange(-0.5))
		return -1;
	if (h->rdr_set_max_Xrange(maxDistance))
		return -1;

	//transmitter power (0==OFF to 7==MAX)
	if (h->rdr_set_tx_power(7))
		return -1;

	//number of integrations (aka number of pulses for build a stream)
	

	if (h->rdr_set_iterations(8000))
		return -1;

	//returned data type (RAW/MTI)
	if(h->rdr_set_elab(RT_MTI))
		return -1;

	//data format (data transfer only, returned data are converted locally to float)
	if(h->rdr_set_data_fmt(FMT_F16))
		return -1;

	//required frame rate (Hz)
	if (h->rdr_set_fps(20))
		return -1;


	uint16_t fs;
	//get current ADC's sampling rate (MHz)
	if (h->rdr_get_adcfreq(&fs))
		return -1;
	printf("Sampling rate: %d\r\n", fs);


	//enable coprocessor DC removal, image calculation and detection algorithm
	if (h->rdr_set_optproc((1 << RDCOM_OPTPROC_BYTE0_DCREM_POS), (1 << RDCOM_OPTPROC_BYTE1_DET_ALGO_POS) | (1 << RDCOM_OPTPROC_BYTE1_EXE_RECONSTRUCTION)))
		return -1;

	canvas2d_param_t canvas;
	canvas.algo = RA_DMAS;
	canvas.phiMax = 60;
	canvas.phiMin = -60;
	canvas.phiStep = 5;
	canvas.rhoMax = maxDistance;
	canvas.rhoMin = 0.5;
	canvas.rhoStep = 0.15;
	if (h->rdr_set_canvas(canvas))
		return -1;

	if (h->rdr_set_rxGain(5))
		return -1;
	
	float SNRs[4] = { 60.0, 40.0, 3.0, 3.0 };
	float SNRRanges[3] = {0.8, 1.2, 3.5};
	if (h->rdr_set_SNRs(SNRs))
		return -1;

	if (h->rdr_set_SNRranges(SNRRanges))
		return -1;


	if (h->rdr_start())
		return -1;

	return 0;
}

/**
 * This routine inquiry the device ID and instance the handler
 * */
radar_api_base * openDevice(rifll_h * comm_h){
	uint16_t hwCode;
	if (radar_api_base::getHW(comm_h, &hwCode)){
		printf("Error on retrieve HWID\n");
		return NULL;
	}
	if (hwCode == rdr_AHM2D::HWID){
		printf("Detected AHM2D\n");
		return new radar_AHM2D(comm_h);
	}
	if (hwCode == rdr_AHM3D::HWID){
		printf("Detected AHM3D\n");
		printf("This device doesn't support 2D image generation\n");
		return NULL;
	}
	if (hwCode == rdr_AHM2DL::HWID){
		printf("Detected AHM2DL\n");
		return new radar_AHM2DL(comm_h);
	}
	printf("Unknown radar ID %x\n", hwCode);
	return NULL;
}

void preExit(){
	delete rdr_h;
	printf("Press any key to exit...");
	getchar();
	getchar();
}

void printUsage(){
	printf("Usage <spi/uart> <device path>");
}

#if WIN
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT:
		stopthread = true;
		return TRUE;

	default:
		return FALSE;
	}
}
#else

void intHandler(int dummy) {
	stopthread = true;
}
#endif

//
int main(int argc, char * argv[]){

	int retcode = 0;
	if (argc != 3){
		printUsage();
		return 1;
	}
	enum {UART, SPI, UNKNOWN} sel_interface;
	sel_interface = UNKNOWN;
	if (strcmp(argv[1], "spi") == 0){
		sel_interface = SPI;
	}
	if (strcmp(argv[1], "uart") == 0){
		sel_interface = UART;
	}
	if (sel_interface == UNKNOWN){
		printUsage();
		return 1;
	}
	rifll_h * comm_h; //communication handler
	if (sel_interface == UART){
		comm_h = new radarif_ll_uart(std::string(argv[2]), 921600);
		if (!comm_h->isValid()) {
			printf("unable to open serial port\n");
			retcode = -1;
			preExit();
			return retcode;
		}
	}else{
#ifdef CONFIG_USE_SPI
		//instance SPI communication handler (it takes care of open the SPI and initialize the communication channel)
		comm_h = new radarif_ll_spi(argv[2], 0, 10000000UL, 0);
#else
		printf("Unsupported interface");
		return -1;
#endif
	}
	
	rdr_h = openDevice(comm_h);
	if (rdr_h == NULL) {
		retcode = -1;
		preExit();
		return retcode;
	}
	//wait for settling

	if (configure_rdr(rdr_h)){
		retcode = -1;
		preExit();
		return retcode;
	}

	printf("Ctrl-C to exit\n");
	stopthread = false;
#if WIN
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
#else
	signal(SIGINT, intHandler);
#endif
	int errCnt = 0;
	while (!stopthread && (errCnt < 3)) {
		std::vector<det_pos_t> detOut;
		if (rdr_h->rdr_get_detection2D(&detOut, true)){
			errCnt++;
			continue;
		}
		if (detOut.size() == 0)
			std::cout << "No detection                                      " << '\r';
		else {
			std::cout << "Detected " << detOut[0].x << ", " << detOut[0].y << '\r';
		}

	}
	rdr_h->rdr_stop();
	preExit();

	return retcode;
}
