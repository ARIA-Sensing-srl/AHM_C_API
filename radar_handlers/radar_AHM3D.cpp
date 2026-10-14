/*
 * radar_AHM3D.cpp
 *
 *  Created on: 21 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */

#include "radar_AHM3D.h"
#include "ahm_comm_def.h"
#include "ahm_common.h"
#include "stddef.h"
#include "string.h"
#include "f16tosingle.h"

using namespace rdr_AHM3D;


const RadarCommandItem_t radar_AHM3D::RadarCommandItemTbl[] = {
	{'s',RADARCOMM_START						,PT_NO		,0									},
		{'o',RADARCOMM_STOP							,PT_NO		,0									},
		{'d',RADARCOMM_GET_DATA						,PT_NO		,0									},
		{'f',RADARCOMM_SET_FRAME_RATE				,PT_U16		,0xFFFF								},
		{'p',RADARCOMM_SET_TX_POWER					,PT_U8		,0xFF								},
		{'x',RADARCOMM_SET_MIN_XRANGE				,PT_FLOAT	,0xFFFFFFFF							},
		{'X',RADARCOMM_SET_MAX_XRANGE				,PT_FLOAT	,0xFFFFFFFF							},
		{'0',RADARCOMM_SET_OFFSET					,PT_FLOAT	,0xFFFFFFFF							},
		{'l',RADARCOMM_SET_DECLUT_LENGTH			,PT_U16		,0xFFFF								},
		{'i',RADARCOMM_SET_ITERATIONS				,PT_U16		,0xFFFF								},
		{'e',RADARCOMM_SET_ELAB						,PT_U8		,0xFF								},
		{0x01,RADARCOMM_GET_VERSION					,PT_NO		,0									},
		{'?',RADARCOMM_UNKNOWN_COMMAND				,PT_NO		,0									},
		{'$',RADARCOMM_SET_DATA_CHANNEL				,PT_U8		,0xFF								},
		{'b',RADARCOMM_SET_OPTPROC					,PT_STRUCT	,2									},
		{0x07,RADARCOMM_GET_DATA_MULTIPLE			,PT_NO		,0									},
		{0x08,RADARCOMM_SET_SEQUENCE				,PT_STRUCT	,QUERY_PAYLOAD_STRUCT_VARIABLE		},
		{0x0A,RADARCOMM_SET_DATA_FMT				,PT_U8		,0xFF								},/**/
		{0x0B,RADARCOMM_SET_FREQ_CARRIER			,PT_U16		,0xFFFF								},
		{0x03,RADARCOMM_SG_BWMODE					,PT_U8		,0xFF								},
		{0x0C,RADARCOMM_SET_BW						,PT_U16		,0xFFFF								},
		{0x0D,RADARCOMM_SET_CODE					,PT_STRUCT	,QUERY_PAYLOAD_STRUCT_VARIABLE		},
		{0x0E,RADARCOMM_SET_GAIN					,PT_STRUCT	,4									},
		{0x0F,RADARCOMM_GET_VOLUME					,PT_NO		,0									},
		{0x05,RADARCOMM_GET_SAMPLING_RATE_MHZ		,PT_U16		,0xFFFF								},
		{'v',RADARCOMM_SET_ANT_ARRAY				,PT_STRUCT	,QUERY_PAYLOAD_STRUCT_VARIABLE		},
		{'m',RADARCOMM_SET_CANVAS					,PT_STRUCT	,QUERY_PAYLOAD_STRUCT_VARIABLE		},
		{0x02,RADARCOMM_SG_MULTISTREAM				,PT_U16	    ,0xFFFF								},
		{0x04,RADARCOMM_SG_BAUDRATE					,PT_U32		,0xFFFFFFFF							}};

radar_AHM3D::radar_AHM3D(rifll_h* val) : radar_AHM2D(val, 0xa3d0){
	RadarCommandItem = RadarCommandItemTbl;
	RadarCommandTblItems = sizeof(RadarCommandItemTbl)/sizeof(RadarCommandItemTbl[0]);
}
radar_AHM3D::~radar_AHM3D(){};

int radar_AHM3D::rdr_set_canvas(canvas3d_param_t canvas){
	radar_command_payload payload;
	payload.data_struct.size = 9*4+1;
	payload.data_struct.buffer[0] = canvas.algo;
	float *pval = (float*)(payload.data_struct.buffer+1);
	pval[0] = canvas.rhoMin;
	pval[1] = canvas.rhoStep;
	pval[2] = canvas.rhoMax;
	pval[3] = canvas.elMin;
	pval[4] = canvas.elStep;
	pval[5] = canvas.elMax;
	pval[6] = canvas.azimMin;
	pval[7] = canvas.azimStep;
	pval[8] = canvas.azimMax;
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_CANVAS), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM3D::rdr_get_canvas(canvas3d_param_t * p){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_CANVAS), &payload);
	if (ret)
		return RDR_ERROR;
	if(payload.data_struct.size != 9*4+1){
		return RDR_ERROR;
	}
	float * pval;
	pval = (float*)(payload.data_struct.buffer+1);
	p->algo = (recAlgo_t)payload.data_struct.buffer[0];
	p->rhoMin = pval[0];
	p->rhoStep = pval[1];
	p->rhoMax = pval[2];
	p->elMin = pval[3];
	p->elStep = pval[4];
	p->elMax = pval[5];
	p->azimMin = pval[6];
	p->azimStep = pval[7];
	p->azimMax = pval[8];
	return 0;
}


radarDataVolume<float> * radar_AHM3D::rdr_get_volume(){
	uint8_t cmd = getCommandItem(RADARCOMM_GET_VOLUME)->cmdCode;
	int inSize = rh->txrxMessage(&cmd, 1, NULL, 0); //plan to direct access to buffer data

	if (inSize < 0)
		return NULL;
	uint8_t * pbuffer = rh->rxbuf.pdata+1;
	//check command
	if (pbuffer[0] != cmd){
		return NULL; //radar is in a not valid state for this request
	}

	pbuffer++;
	inSize--;
	//readout canonical data
	uint16_t clmns;
	uint8_t nEl, nAzim;
	uint16_t etime_fmt;
	uint16_t totalSamples;
	memcpy((void*)&clmns, (const void*)pbuffer, 2);
	memcpy((void*)&etime_fmt, (const void*)(pbuffer+2), 2);
	nEl = pbuffer[4];
	nAzim = pbuffer[5];
	inSize -= 6;
	pbuffer += 6;
	totalSamples = clmns*nEl*nAzim;

	uint16_t fmt = etime_fmt >> 12;
	//compute expected samples
	uint32_t expectedSize = (uint32_t)totalSamples;
	switch(fmt){
		case FMT_Q7:
			expectedSize *= 1;
			break;
		case FMT_Q15:
			expectedSize *= 2;
			break;
		case FMT_F16:
			expectedSize *= 2;
			break;
		case FMT_Q31:
			expectedSize *= 4;
			break;
		case FMT_F32:
			expectedSize *= 4;
			break;
		default:
			return NULL;
			break;
	}

	radarDataVolume<float> * prd;
	prd = NULL;
	if (expectedSize == inSize){
		//verify the number of channels

		prd = new radarDataVolume<float>(clmns, nEl, nAzim);
		float * pdata = prd->getBuffer();
		//decode and save

		for(int i = 0; i < totalSamples; i++){
			float curSample;
			switch(fmt) {
				case FMT_Q7:
					curSample = ((int8_t*)pbuffer)[0];
					pbuffer += 1;
					pdata[i] = curSample;
					break;
				case FMT_Q15:
					curSample = ((int16_t*)pbuffer)[0];
					pbuffer += 2;
					pdata[i] = curSample;
					break;
				case FMT_F16:
					f16tosingle((float16_t*)pbuffer, &curSample, 1);
					pbuffer += 2;
					pdata[i] = curSample;
					break;
				case FMT_Q31:
					curSample = ((int32_t*)pbuffer)[0];
					pbuffer += 4;
					pdata[i] = curSample;
					break;
				case FMT_F32:
					curSample = ((float*)pbuffer)[0];
					pbuffer += 4;
					pdata[i] = curSample;
					break;
			}
		}
	}else
		return NULL;

	return prd; //no error
}
