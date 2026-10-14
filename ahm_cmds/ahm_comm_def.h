/*
 * radar_comm_def.h
 *
 *  Created on: Sept 02, 2020
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */

#ifndef RADAR_COMM_DEF_H
#define RADAR_COMM_DEF_H

#include "stdint.h"


//sequence command
#define mRDRCOM_decode_sequence_tx(x) ((x & 0xF0) >> 4)
#define mRDRCOM_decode_sequence_rx(x) ((x & 0x0F))
#define mRDRCOM_encode_sequence(t,r) (((t & 0xF)<<4) | (r & 0xF))
//antenna array macro command
#define mRDRCOM_antarrayID_isRx(id) (id < 4)
#define mRDRCOM_decode_antArrayID(id) ((id > 3) ? (id-4) : id)
#define mRDRCOM_encode_antArrayID(id, isRx) ((isRx) ? id : (id+4))


//bitmaps (option procedure)
#define RDCOM_OPTPROC_BYTE1_DET_ALGO_POS 		0
#define RDCOM_OPTPROC_BYTE1_CPLX_IMAGE_POS 		1
#define RDCOM_OPTPROC_BYTE1_EXE_RECONSTRUCTION 	2


#define RDCOM_OPTPROC_BYTE0_DCREM_POS 			0
#define RDCOM_OPTPROC_BYTE0_CORR_POS 			1
#define RDCOM_OPTPROC_BYTE0_MATCHFILT_POS 		2



/*
 * RADARCOMM_GET_DATA_MULTIPLE
 * Header
 * Bytes	: Description
 * 0-1		: u16, number of samples for each scan
 * 2-3		: u16, from bit 0-11 is elapesd time, bit 12-15 is the data format according to FMT encoding
 * 4-5		: u16, total number of samples expected, this number is equal to NumSamples*numScan
 * 5-end	: data, complex data are transferre as In,Qn, In+1, Qn+1 .... according to format specified into the FMT field
 * */

#define RADARCOMM_BASIC \
	RADARCOMM_UNKNOWN_COMMAND,\
	RADARCOMM_NOOP,							\
	RADARCOMM_START,							\
	RADARCOMM_STOP,							\
	RADARCOMM_GET_DATA,						\
	RADARCOMM_SET_FRAME_RATE,					\
	RADARCOMM_SET_TX_POWER,					\
	RADARCOMM_SET_MIN_XRANGE,					\
	RADARCOMM_SET_MAX_XRANGE,					\
	RADARCOMM_SET_OFFSET,					\
	RADARCOMM_SET_ITERATIONS,					\
	RADARCOMM_KALMAN,							\
	RADARCOMM_RESET,					\
	RADARCOMM_SET_DECLUT_LENGTH, 				\
	RADARCOMM_SET_ELAB,						\
	RADARCOMM_SET_OPTPROC,						\
	RADARCOMM_SG_BAUDRATE,\
	RADARCOMM_SET_DATA_CHANNEL,					\
	RADARCOMM_GET_VERSION,						\
	RADARCOMM_GET_DATA_MULTIPLE,				\
	RADARCOMM_GET_OUTPUT_SIZE,					\
	RADARCOMM_GET_2D,							\
	RADARCOMM_SET_SEQUENCE,						\
	RADARCOMM_SET_DATA_FMT,						\
	RADARCOMM_SET_FREQ_CARRIER,					\
	RADARCOMM_SET_BW,							\
	RADARCOMM_SG_BWMODE,\
	RADARCOMM_SET_CODE,							\
	RADARCOMM_SET_GAIN,							\
	RADARCOMM_GET_IMG,							\
	RADARCOMM_SET_ANT_ARRAY,						\
	RADARCOMM_GET_SAMPLING_RATE_MHZ,\
	RADARCOMM_SET_CANVAS,						\
	RADARCOMM_SG_MULTISTREAM,					\
	RADARCOMM_SET_PROFILE,\
	RADARCOMM_SG_STATICOBJREMOVAL,\
	RADARCOMM_SG_STATICOBJMAPUPDATETIME,\
	RADARCOMM_SG_RXGAIN,\
	RADARCOMM_SG_EMBEDDEDIMGALGO

namespace rdr_AHM2D {
		
	typedef enum {
		RADARCOMM_BASIC
	}radar_command;

	const uint8_t RDR_TX_POWER_OFF = 0;
	const uint8_t RDR_TX_POWER_HIGH = 7;

	const uint16_t HWID = 0xa2d0;
}

namespace rdr_AHM3D {
typedef enum {
		RADARCOMM_BASIC,
		RADARCOMM_GET_VOLUME
	}radar_command;

	const uint8_t RDR_TX_POWER_OFF = 0;
	const uint8_t RDR_TX_POWER_HIGH = 7;

	const uint16_t HWID = 0xa3d0;
}

namespace rdr_AHM2DL {

	typedef enum {
		RADARCOMM_BASIC
	}radar_command;

	const uint8_t RDR_TX_POWER_OFF = 0;
	const uint8_t RDR_TX_POWER_HIGH = 7;

	const uint16_t HWID = 0xa2d1;
}






#endif
