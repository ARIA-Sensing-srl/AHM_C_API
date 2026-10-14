/*
 * main.c
 *
 *  Created on: 23 feb 2021
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */


#include "stdio.h"
#include "unistd.h"
#include "wiringSerial.h"
#include "radarif_ll.h"
#include "radar_AHM2D.h"
#include "ahm_comm_def.h"
#include "radarif_ll_spi.h"
#include "string.h"

static int serial_fd=0;
static rifll_h * comm_h; //communication handler
static radar_AHM2D * rdr_h;


int txdata(uint8_t * buffer, int size){
	for(int i=0; i < size; i++){
		serialPutchar(serial_fd, *buffer++);
	}
	return 0;
}

int rxdata(uint8_t * buffer, int size){
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

static int configure_rdr(radar_AHM2D * h){

	uint16_t hwCode;
	if (h->getHW(&hwCode)){
		printf("Error on get HW model\n");
		return -1;
	}
	if (hwCode != h->HWCODE){
		printf("Invalid HW code, verify connected HW type\n");
		return -1;
	}
	if (h->rdr_stop())
		return -1;
	if (h->rdr_set_freq_carrier(8064))
		return -1;
	if (h->rdr_set_bw( 1000))
		return -1;
	uint8_t opt1, opt0;
	opt1 = (1<<RDCOM_OPTPROC_BYTE1_EXE_RECONSTRUCTION);
	opt0 = (1<<RDCOM_OPTPROC_BYTE0_DCREM_POS) | (1<<RDCOM_OPTPROC_BYTE0_MATCHFILT_POS);
	if (h->rdr_set_optproc( opt0, opt1))
		return -1;

	if(h->rdr_set_offset(-4))
		return -1;

	if(h->rdr_set_min_Xrange(1))
		return -1;
	if (h->rdr_set_max_Xrange( 5))
		return -1;
	if (h->rdr_set_gain(20, 20))
		return -1;
	if (h->rdr_set_declutter_length(100))
		return -1;

	int8_t code[1] = {1};
	if(h->rdr_set_code( code, sizeof(code)))
		return -1;
	if (h->rdr_set_tx_power( 7))
		return -1;
	if (h->rdr_set_iterations( 10000))
		return -1;

	uint8_t txScan[4];
	uint8_t rxScan[4];
	txScan[0] = AHM_ANT_TX1;
	rxScan[0] = AHM_ANT_RX1;
	txScan[1] = AHM_ANT_TX1;
	rxScan[1] = AHM_ANT_RX2;
	txScan[2] = AHM_ANT_TX2;
	rxScan[2] = AHM_ANT_RX2;
	txScan[3] = AHM_ANT_TX2;
	rxScan[3] = AHM_ANT_RX1;
	if(h->rdr_set_sequence( txScan, rxScan, 4))
		return -1;

	if(h->rdr_set_elabtype(RT_RAW))
		return -1;

	if(h->rdr_set_fmt(FMT_F16))
		return -1;

	if (h->rdr_start())
		return -1;

	return 0;
}

void preExit(){
	delete rdr_h;
	delete comm_h;
	if (serial_fd > 0){
		serialClose(serial_fd);
	}
	printf("Press any key to exit...");
	getchar();
	getchar();
}

void printUsage(){
	printf("Usage <spi/uart> <device path>");
}

//
int main(int argc, char * argv[]){

	int retcode = 0;
	if (argc != 3){
		printUsage();
		return 1;
	}
	enum {UART, SPI, UNKNOWN} interface;
	interface = UNKNOWN;
	if (strcmp(argv[1], "spi") == 0){
		interface = SPI;
	}
	if (strcmp(argv[1], "uart") == 0){
		interface = UART;
	}
	if (interface == UNKNOWN){
		printUsage();
		return 1;
	}
	if (interface == UART){
		serial_fd = serialOpen(argv[2], 921600);
		if (serial_fd < 0){
			printf("unable to open serial port\n");
			retcode = -1;
			preExit();
			return retcode;
		}
		printf("Serial port open\n");
		comm_h = new rifll_h(txdata, rxdata, 0);
	}else{
		comm_h = new radarif_ll_spi(argv[2], 0, 30000000UL, 0);
	}
	rdr_h = new radar_AHM2D(comm_h);




	//wait for settling

	if (configure_rdr(rdr_h)){
		retcode = -1;
		preExit();
		return retcode;
	}
	radarData<float> rd;

	sleep(1);

	printf("Test readout of all channels\r\n");
	{

		if (!rdr_h->rdr_get_image(&rd)){

			FILE * fp = fopen("image.txt", "w");
			if (!fp){
				printf("Error on file opening\r\n");
			}else{
				int channels, samples;
				channels = rd.numRows();
				samples = rd.numClmns();
				printf("Readout done, received %d samples\r\n", samples);
				for(int c=0; c < channels; c++){
					for (int s = 0; s < samples; s++){
						fprintf(fp, "%f ", rd.val(c, s));
					}
					fprintf(fp, "\n");
				}
				fclose(fp);
			}
		}else{
			printf("Readout error\r\n");
		}
	}

	rdr_h->rdr_stop();
	preExit();

	return retcode;
}
