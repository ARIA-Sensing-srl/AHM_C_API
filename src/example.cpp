/*
 * main.c
 *
 *  Created on: 23 feb 2021
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */


#include "stdio.h"

#ifdef CONFIG_USE_SPI
#include "radarif_ll_spi.h"
#endif
#ifndef WIN
#include "unistd.h"
#include <sys/time.h>
#else
#include <windows.h>
#include "synchapi.h"
#endif
#include "wiringSerial.h"
#include "radarif_ll.h"
#include "radarif_ll_uart.h"
#include "radar_AHM2D.h"
#include "ahm_comm_def.h"
#include "radar_AHM3D.h"
#include "string.h"
#include <stddef.h>
#include "radar_AHM2DL.h"

#ifdef WIN
#define usleep(x) Sleep(x/1000)
#define sleep(x) Sleep(x)
#endif


//static rifll_h * comm_h; 	//communication handler
static radar_api_base* rdr_h;


#ifdef WIN
int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}
#endif


/**
 * @brief Configuration save routine
 * Store information into json formatted file
 * */
static int save_rdr_configuration(radar_api_base * h, const char * filename){

	FILE * cfgFile = fopen(filename, "w");
	if (cfgFile == NULL){
		printf("Failed to open file\n");
		return -1;
	}
	//JSON
	fprintf(cfgFile, "{");

	fprintf(cfgFile, "\"hwcode\":");
	//HWCODE
	switch (h->HWCODE){
		case rdr_AHM2D::HWID:
			fprintf(cfgFile, "\"AHM2D\",");
			break;
		case rdr_AHM2DL::HWID:
			fprintf(cfgFile, "\"AHM2DL\",");
			break;
		case rdr_AHM3D::HWID:
			fprintf(cfgFile, "\"AHM3DL\",");
			break;
		default:
			fprintf(cfgFile, "\"unknown\",");
			break;
	}

	//bandwidth
	uint16_t u16var;
	if (!h->rdr_get_bw(&u16var)){
		fprintf(cfgFile, "\"bandwidth\":%d,", u16var);
	}
	//sampling rate
	if (!h->rdr_get_fSampling(&u16var)){
		fprintf(cfgFile, "\"samplingrate\":%d,", u16var);
	}
	//carrier
	if (!h->rdr_get_freq_carrier(&u16var)){
		fprintf(cfgFile, "\"carrier\":%d,", u16var);
	}

	//range min
	float floatvar;
	if (!h->rdr_get_min_Xrange(&floatvar)){
		fprintf(cfgFile, "\"rangemin\":%g,", floatvar);
	}

	//range max
	if (!h->rdr_get_max_Xrange(&floatvar)){
		fprintf(cfgFile, "\"rangemax\":%g,", floatvar);
	}

	//offset
	if (!h->rdr_get_offset(&floatvar)){
		fprintf(cfgFile, "\"offset\":%g,", floatvar);
	}

	//gain
	uint16_t u16var2;
	if (!h->rdr_get_gain(&u16var, &u16var2)){
		fprintf(cfgFile, "\"rxgain\":[%d, %d],", u16var, u16var2);
	}

	//declutter
	if (!h->rdr_get_declutter_length(&u16var)){
		fprintf(cfgFile, "\"declutter\":%d,", u16var);
	}

	//iterations
	if (!h->rdr_get_iterations(&u16var)){
		fprintf(cfgFile, "\"iterations\":%d,", u16var);
	}

	//elaboration
	rdrElabTypes_t elabType;
	if (!h->rdr_get_elabtype(&elabType)){
		fprintf(cfgFile, "\"elab\":");
		switch(elabType){
			case RT_RAW:
				fprintf(cfgFile, "\"raw\",");
				break;
			case RT_MTI:
				fprintf(cfgFile, "\"MTI\",");
				break;
			default:
				fprintf(cfgFile, "\"unknown\",");
				break;
		}
	}

	//frame rate
	if (!h->rdr_get_frame_rate(&u16var)){
		fprintf(cfgFile, "\"framerate\":%d,", u16var);
	}
	//code
	int8_t code[32];
	int size = 32;
	if (!h->rdr_get_code(code, &size)){
		fprintf(cfgFile, "\"code\": [");
		for(int i = 0 ; i< size; i++){
			fprintf(cfgFile, "%d", code[i]);
			if (i < (size-1)){
				fprintf(cfgFile, ",");
			}
		}
		fprintf(cfgFile, "],");
	}

	//scan sequence
	uint8_t txMask[16];
	uint8_t rxMask[16];
	uint8_t numSeq = 16;
	if (!h->rdr_get_sequence(txMask, rxMask, &numSeq )){
		fprintf(cfgFile, "\"sequence\": {\"txmask\":[");
		for(int i = 0 ; i < numSeq; i++){
			fprintf(cfgFile, "%d", txMask[i]);
			if (i < (numSeq-1)){
				fprintf(cfgFile,",");
			}
		}
		fprintf(cfgFile, "],\"rxmask\":[");
		for(int i = 0 ; i < numSeq; i++){
			fprintf(cfgFile, "%d", rxMask[i]);
			if (i < (numSeq-1)){
				fprintf(cfgFile,",");
			}
		}

		fprintf(cfgFile, "]}");
	}
	fprintf(cfgFile, "}");


	fclose(cfgFile);
	return 0;
}



/**
 * @brief Configuration routine
 * This routine configures the module and start radar operation
 * User could edit this routine and modify acquisition parameters
 * */
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

	//set to 1 for 1.8GHz pulse's bandwidth (increase ADC speed and power consumption)
	if (h->rdr_set_bwmode(0)){
		printf("Bwmode failed\n");
		return -1;
	}
	sleep(1); //bwmode command alter internal clock, this requires extra time before send new commands

	//Carrier frequency, do not change
	if (h->rdr_set_freq_carrier(8064)){
		printf("Set frequency failed\n");
		return -1;
	}

	//set up to 1800 MHz (mind to switch bwmode to 1 if bandwidth greater than 1.3G is required)
	if (h->rdr_set_bw(1300)){
		printf("Bandwidth setting failed\n");
		return -1;
	}

	//application / co-processor elaboration options.
	uint8_t opt1, opt0;
	opt1 = 0;
	opt0 = (1<<RDCOM_OPTPROC_BYTE0_DCREM_POS) | (1<<RDCOM_OPTPROC_BYTE0_MATCHFILT_POS);
	if (h->rdr_set_optproc(opt0, opt1)){
		printf("Opt mode failed\n");
		return -1;
	}

	//acquisition offset (meters), the default -1 m value has been found by in-house calibration
	if(h->rdr_set_offset(-1))
		return -1;

	//acquisition range (meters)
	if(h->rdr_set_min_Xrange(0))
		return -1;
	if (h->rdr_set_max_Xrange(5))
		return -1;

	//receiver chain gain (values from 0-33), suggested value from 15 to 22
	if (h->rdr_set_gain(18, 18))
		return -1;

	//de-clatter length (equivalent to number of frame used for build the static object matrix)
	if (h->rdr_set_declutter_length(200))
		return -1;

	//transmitter power (0==OFF to 7==MAX)
	if (h->rdr_set_tx_power(7))
		return -1;

	//number of integrations (aka number of pulses for build a stream)
	int iterationsSetting = 10000;
	if (h->HWCODE == rdr_AHM2DL::HWID)
		iterationsSetting = 4000;

	if (h->rdr_set_iterations(iterationsSetting))
		return -1;

	//pulse sequence transmitted (default 1 pulse per burst)
	int8_t code[1] = {1};
	if(h->rdr_set_code(code, sizeof(code)))
		return -1;

	//returned data type (RAW/MTI)
	if(h->rdr_set_elabtype(RT_RAW))
		return -1;

	if (h->rdr_set_multistream(1)){
		return -1;
	}

	//data format (data transfer only, returned data are converted locally to float)
	if(h->rdr_set_fmt(FMT_F16))
		return -1;

	//required frame rate (Hz)
	if (h->rdr_set_frame_rate(20))
		return -1;


	uint16_t fs;
	//get current ADC's sampling rate (MHz)
	if (h->rdr_get_fSampling(&fs))
		return -1;
	printf("Sampling rate: %d\r\n", fs);

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
		return new radar_AHM3D(comm_h);
	}
	if (hwCode == rdr_AHM2DL::HWID){
		printf("Detected AHM2DL\n");
		return new radar_AHM2DL(comm_h);
	}
	printf("Unknown radar ID %x\n", hwCode);
	return NULL;
}

/**
 * De-initialization routine, execute before exit
 * */
void preExit(){
	delete rdr_h;
	//delete comm_h;

	printf("Press any key to exit...");
	getchar();
	getchar();
}

void printUsage(){
	printf("Usage <spi/uart> <device path>\n");
}

/**
 * @brief Sequential read example routing
 * Intended for speed verification, this routine acquires a pre-defined
 * number of acquisition, store them into a file, and calculate the frame rate achieved
 * */
static int packedRead(){
	FILE * fp = fopen("chainAcq.txt", "w");
	if (!fp){
		printf("Error on file opening\r\n");
		return -1;
	}
	bool errorReadout = false;
	radarData<cplx_float_t> rd;
	struct timeval stop, start;
	gettimeofday(&start, NULL);
	int numAcquisitions = 20; //number of rx command issued, actual number of acquisition is numAcquisitions*numStreams
	uint16_t numStreams;
	if(rdr_h->rdr_get_multistream(&numStreams))
		return -1;

	for (int i = 0; i<numAcquisitions; i++){
		if (rdr_h->rdr_rx_multichannel(&rd) != 0){
			errorReadout = true;
		}
		int channels, samples;
		channels = rd.numRows();
		samples = rd.numClmns();
		//printf("Readout done, received %d samples\r\n", samples);
		for(int c=0; c < channels; c++){
			for (int s = 0; s < samples; s++){
				fprintf(fp, "%f %f ", rd.val(c, s).re, rd.val(c, s).im);
			}
			fprintf(fp, "\n");
		}
	}
	gettimeofday(&stop, NULL);
	fclose(fp);
	if (errorReadout){
		printf("Multiple readout error\r\n");
		return -1;
	}
	long int elapsedTime = (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec)/1000;
	printf("Elapsed time %lu ms \r\n", elapsedTime);
	printf("Equivalent frame rate %g\r\n", ((float)numStreams*numAcquisitions/((float)elapsedTime))*1000.0);
	return 0;
}

int main(int argc, char * argv[]){

	int retcode = 0;

	//argv[1] interface, SPI or UART
	//argv[2] interface file

	if (argc != 3){
		printUsage();
		return 1;
	}
	enum {UART, SPI, UNKNOWN} sel_interface;
	sel_interface = UNKNOWN;

	//interface parsing
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
	rifll_h* comm_h;

	if (sel_interface == UART){
		//initialize serial port and instance the UART communication handler

		comm_h = new radarif_ll_uart(std::string(argv[2]), 921600);
		if (!comm_h->isValid()) {
			printf("unable to open serial port\n");
			retcode = -1;
			preExit();
			return retcode;
		}


		//change baudrate

	}else{
#ifdef CONFIG_USE_SPI
		//instance SPI communication handler (it takes care of open the SPI and initialize the communication channel)
		comm_h = new radarif_ll_spi(argv[2], 0, 10000000UL, 0);
#else
		printf("Unsupported interface");
		return -1;
#endif
	}

	//Open module
	rdr_h = openDevice(comm_h);

	if (rdr_h == NULL){
		retcode = -1;
		preExit();
		return retcode;
	}


	//setup module
	if (configure_rdr(rdr_h)){
		retcode = -1;
		printf("Configuration error\r\n");
		preExit();
		return retcode;
	}

	save_rdr_configuration(rdr_h, "config.json");

	//instance frame data
	radarData<cplx_float_t> rd;
	sleep(1);


	printf("Test readout of all channels\r\n");
	{

		packedRead(); //continuous read example

		//single read, data stored into file
		if (rdr_h->rdr_rx_multichannel(&rd) == 0){

			FILE * fp = fopen("multiple.txt", "w");
			if (!fp){
				printf("Error on file opening\r\n");
			}else{
				int channels, samples;
				channels = rd.numRows();
				samples = rd.numClmns();
				printf("Readout done, received %d samples\r\n", samples);
				for(int c=0; c < channels; c++){
					for (int s = 0; s < samples; s++){
						fprintf(fp, "%f %f ", rd.val(c, s).re, rd.val(c, s).im);
					}
					fprintf(fp, "\n");
				}
				fclose(fp);
			}
		}else{
			printf("Readout error\r\n");
		}
	}

	//send stop command
	rdr_h->rdr_stop();


	//de-initialize
	preExit();


	return retcode;
}
