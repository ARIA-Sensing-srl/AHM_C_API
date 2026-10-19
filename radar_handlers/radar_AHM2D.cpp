/*
 * radar_api_base.cpp
 *
 *  Created on: 20 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
 */


#include <ahm_comm_def.h>
#include "radar_AHM2D.h"
#include "string.h"
#include "f16tosingle.h"
#include "stdlib.h"
#include <string>

using namespace rdr_AHM2D;

#define mSetU8(cmd, val) { \
		radar_command_payload payload; \
		payload.data_uint8 = val; \
		int ret = rdr_set_val(getCommandItem((uint32_t)cmd), &payload); \
		if (ret) \
			return RDR_ERROR; \
		return 0; \
	}

#define mSetU16(cmd, val) { \
		radar_command_payload payload; \
		payload.data_uint16 = val; \
		int ret = rdr_set_val(getCommandItem((uint32_t)cmd), &payload); \
		if (ret) \
			return RDR_ERROR; \
		return 0; \
	}

#define mSetU32(cmd, val) { \
		radar_command_payload payload; \
		payload.data_uint32 = val; \
		int ret = rdr_set_val(getCommandItem((uint32_t)cmd), &payload); \
		if (ret) \
			return RDR_ERROR; \
		return 0; \
	}

#define mSetFloat(cmd, val) { \
		radar_command_payload payload; \
		payload.data_float = val; \
		int ret = rdr_set_val(getCommandItem((uint32_t)cmd), &payload); \
		if (ret) \
			return RDR_ERROR; \
		return 0; \
	}

#define mGetU8(cmd, pval) { \
	radar_command_payload payload; \
	int ret = rdr_get_val(getCommandItem((uint32_t)cmd), &payload); \
	if (ret) \
		return RDR_ERROR; \
	*pval =  payload.data_uint8; \
	return 0; }

#define mGetU16(cmd, pval) { \
	radar_command_payload payload; \
	int ret = rdr_get_val(getCommandItem((uint32_t)cmd), &payload); \
	if (ret) \
		return RDR_ERROR; \
	*pval =  payload.data_uint16; \
	return 0; }

#define mGetU32(cmd, pval) { \
	radar_command_payload payload; \
	int ret = rdr_get_val(getCommandItem((uint32_t)cmd), &payload); \
	if (ret) \
		return RDR_ERROR; \
	*pval =  payload.data_uint32; \
	return 0; }

#define mGetFloat(cmd, pval) { \
	radar_command_payload payload; \
	int ret = rdr_get_val(getCommandItem((uint32_t)cmd), &payload); \
	if (ret) \
		return RDR_ERROR; \
	*pval =  payload.data_float; \
	return 0; }


const RadarCommandItem_t radar_AHM2D::RadarCommandItemTbl[] = {
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
		{0x0C,RADARCOMM_SET_BW						,PT_U16		,0xFFFF								},
		{0x03,RADARCOMM_SG_BWMODE					,PT_U8		,0xFF								},
		{0x0D,RADARCOMM_SET_CODE					,PT_STRUCT	,QUERY_PAYLOAD_STRUCT_VARIABLE		},
		{0x0E,RADARCOMM_SET_GAIN					,PT_STRUCT	,4									},
		{0x0F,RADARCOMM_GET_IMG						,PT_NO		,0									},
		{0x05,RADARCOMM_GET_SAMPLING_RATE_MHZ		,PT_U16		,0xFFFF								},
		{'v',RADARCOMM_SET_ANT_ARRAY				,PT_STRUCT	,QUERY_PAYLOAD_STRUCT_VARIABLE		},
		{'m',RADARCOMM_SET_CANVAS					,PT_STRUCT	,QUERY_PAYLOAD_STRUCT_VARIABLE		},
		{0x04,RADARCOMM_SG_BAUDRATE					,PT_U32		,0xFFFFFFFF							},
		{0x02,RADARCOMM_SG_MULTISTREAM				,PT_U16	    ,0xFFFF								},
		{0x84,RADARCOMM_SET_PROFILE					,PT_U8	    ,0xFF								},
		{0x80,RADARCOMM_SG_STATICOBJREMOVAL			,PT_U8	    ,0xFF								},
		{0x81,RADARCOMM_SG_STATICOBJMAPUPDATETIME	,PT_FLOAT   ,0xFFFFFFFF							},
		{0x82,RADARCOMM_SG_RXGAIN					,PT_U8	    ,0xFF								},
		{0x83,RADARCOMM_SG_EMBEDDEDIMGALGO			,PT_U8	    ,0xFF								}
};




radar_AHM2D::radar_AHM2D(rifll_h*val) : radar_api_base(val, 0xa2d0){
	RadarCommandItem = RadarCommandItemTbl;
	RadarCommandTblItems = sizeof(RadarCommandItemTbl)/sizeof(RadarCommandItemTbl[0]);
};

radar_AHM2D::radar_AHM2D(rifll_h*val, uint16_t hwval) : radar_api_base(val, hwval){

};

radar_AHM2D::~radar_AHM2D(){};

//public functions common between all radars
int radar_AHM2D::rdr_start(){

	//special case, need custom function
	const RadarCommandItem_t * pitem = getCommandItem(RADARCOMM_START);
	uint8_t buffer[4];
	buffer[0]=pitem->cmdCode;
	int ret = rh->txrxMessage(buffer,1,buffer,4);
	if (ret != 1)
		return RDR_ERROR;
	if (buffer[0] != 'r')
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_stop(){
	int ret = rdr_set_val(getCommandItem(RADARCOMM_STOP), NULL);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_set_fps(uint16_t val){
	radar_command_payload payload;
	payload.data_uint16 = val;
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_FRAME_RATE), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_get_fps(uint16_t * val){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_FRAME_RATE), &payload);
	if (ret)
		return RDR_ERROR;
	*val = payload.data_uint16;
	return 0;
}
int radar_AHM2D::rdr_set_tx_power(uint8_t val){
	if (val < RDR_TX_POWER_OFF || val > RDR_TX_POWER_HIGH)
		return RDR_ERR_INVALID_PARAM;
	radar_command_payload payload;
	payload.data_uint8 = val;
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_TX_POWER), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_get_tx_power(uint8_t * val){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_TX_POWER), &payload);
	if (ret)
		return RDR_ERROR;
	*val = payload.data_uint8;
	return 0;
}

int radar_AHM2D::rdr_set_min_Xrange (float val){
	radar_command_payload payload;
	payload.data_float = val;
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_MIN_XRANGE), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_get_min_Xrange (float *val){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_MIN_XRANGE), &payload);
	if (ret)
		return RDR_ERROR;
	*val =  payload.data_float;
	return 0;
}
int radar_AHM2D::rdr_set_max_Xrange (float val){
	radar_command_payload payload;
	payload.data_float = val;
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_MAX_XRANGE), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_get_max_Xrange (float * val){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_MAX_XRANGE), &payload);
	if (ret)
		return RDR_ERROR;
	*val =  payload.data_float;
	return 0;
}
int radar_AHM2D::rdr_set_range(float min, float max) {
	int ret;
	if (ret = rdr_set_min_Xrange(min))
		return ret;
	if (ret = rdr_set_max_Xrange(max))
		return ret;
	return 0;
};
int radar_AHM2D::rdr_get_range(float* pmin, float* pmax) {
	int ret;
	if (ret = rdr_get_min_Xrange(pmin))
		return ret;
	if (ret = rdr_get_max_Xrange(pmax))
		return ret;
	return 0;
};


int radar_AHM2D::rdr_set_offset (float val){
	radar_command_payload payload;
	payload.data_float = val;
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_OFFSET), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_get_offset (float * val){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_OFFSET), &payload);
	if (ret)
		return RDR_ERROR;
	*val =  payload.data_float;
	return 0;
}
int radar_AHM2D::rdr_set_iterations (uint16_t val){
	radar_command_payload payload;
	payload.data_uint16 = val;
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_ITERATIONS), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_get_iterations (uint16_t * val){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_ITERATIONS), &payload);
	if (ret)
		return RDR_ERROR;
	*val =  payload.data_uint16;
	return 0;
}
int radar_AHM2D::rdr_set_declutter_length (uint16_t val){
	radar_command_payload payload;
	payload.data_uint16 = val;
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_DECLUT_LENGTH), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_get_declutter_length (uint16_t * val){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_DECLUT_LENGTH), &payload);
	if (ret)
		return RDR_ERROR;
	*val = payload.data_uint16;
	return 0;
}

int radar_AHM2D::rdr_set_elab(rdrElabTypes_t val){
	radar_command_payload payload;
	payload.data_uint8 = val;
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_ELAB), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_get_elab(rdrElabTypes_t * val){
	radar_command_payload payload;

	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_ELAB), &payload);
	if (ret)
		return RDR_ERROR;
	*val = (rdrElabTypes_t)payload.data_uint8;
	if (*val > RT_MTI || *val < RT_RAW)
		*val = RT_UNKNOWN;
	return 0;
}


int radar_AHM2D::rdr_set_optproc(uint8_t opt0, uint8_t opt1){
	radar_command_payload payload;
	payload.data_struct.buffer[0] = opt0;
	payload.data_struct.buffer[1] = opt1;
	payload.data_struct.size = 2;
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_OPTPROC), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_get_optproc(uint8_t *opt0, uint8_t *opt1){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_OPTPROC), &payload);
	if (ret)
		return RDR_ERROR;
	*opt0 = payload.data_struct.buffer[0];
	*opt1 = payload.data_struct.buffer[1];
	return 0;
}
int radar_AHM2D::rdr_set_sequence(uint8_t * txSeqMask, uint8_t* rxSeqMask, uint8_t numSeq){
	radar_command_payload payload;
	if (numSeq == 0)
		return RDR_ERR_INVALID_PARAM;
	payload.data_struct.size = numSeq;
	for(int i = 0; i < numSeq; i++){
		uint8_t encoded;
		encoded = mRDRCOM_encode_sequence(txSeqMask[i], rxSeqMask[i]);
		payload.data_struct.buffer[i] = encoded;
	}
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_SEQUENCE), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}

//txSeq must be 16 item large
int radar_AHM2D::rdr_get_sequence(uint8_t * txSeqMask, uint8_t* rxSeqMask, uint8_t * pnumSeq){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_SEQUENCE), &payload);
	if (ret)
		return RDR_ERROR;
	int retNumSeq;
	retNumSeq = payload.data_struct.size;
	if (retNumSeq > 16)
		return RDR_ERR_COMM; //invalid size
	*pnumSeq =retNumSeq;
	for(int i = 0 ; i< retNumSeq; i++){
		txSeqMask[i] = mRDRCOM_decode_sequence_tx(payload.data_struct.buffer[i]);
		rxSeqMask[i] = mRDRCOM_decode_sequence_rx(payload.data_struct.buffer[i]);
	}
	return 0;
}


int radar_AHM2D::rdr_get_version_fw(RdrFWVer_t* p) { 
	uint8_t version[(RDR_VERSION_SIZE)+1]; //version + response command
	uint8_t reqPayload = RDR_VERSION_CMD;
	int ret = rh->txrxMessage(&reqPayload, 1, version, sizeof(version));
	if (ret != (RDR_VERSION_SIZE)+1)
		return RDR_ERR_COMM;
	//check
	if (version[0] != RDR_VERSION_CMD)
		return RDR_ERR_COMM;

	p->customerID = (uint16_t)version[1] | (((uint16_t)version[2]) << 8);
	p->hwcode = (uint16_t)version[3] | (((uint16_t)version[4]) << 8);
	p->IDver = version[5];
	p->featureSet = version[6];
	p->IDsubVer = (uint16_t)version[7] | (((uint16_t)version[8]) << 8);
	return 0;
};


int radar_AHM2D::rdr_set_carrier_frequency(uint16_t carrierMHz){
	mSetU16(RADARCOMM_SET_FREQ_CARRIER, carrierMHz);
}
int radar_AHM2D::rdr_get_carrier_frequency(uint16_t * carrierMHz){
	mGetU16(RADARCOMM_SET_FREQ_CARRIER, carrierMHz);
}
int radar_AHM2D::rdr_set_pulse_bandwidth(uint16_t bwMHz){
	mSetU16(RADARCOMM_SET_BW, bwMHz);
}
int radar_AHM2D::rdr_get_pulse_bandwidth(uint16_t * bwMHz){
	mGetU16(RADARCOMM_SET_BW, bwMHz);
}
int radar_AHM2D::rdr_set_bwmode(uint8_t bwmode){

	//this command require little time between command and response retrieve,
	this->rh->set_tx2rx_wait(250000);
	radar_command_payload payload;
	payload.data_uint8 = bwmode;
	int ret = rdr_set_val(getCommandItem((uint32_t)RADARCOMM_SG_BWMODE), &payload);
	this->rh->set_tx2rx_wait(0); //restore regulat value 0.
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_get_bwmode(uint8_t * bwmode){
	mGetU8(RADARCOMM_SG_BWMODE, bwmode);
}

int radar_AHM2D::rdr_set_baudrate(uint32_t br){
	mSetU32(RADARCOMM_SG_BAUDRATE, br);
}

int radar_AHM2D::rdr_set_profile(profile_t val) {
	mSetU8(RADARCOMM_SET_PROFILE, (uint8_t)val);
}
int radar_AHM2D::rdr_set_staticObjRemoval(uint8_t enable) {
	mSetU8(RADARCOMM_SG_STATICOBJREMOVAL, enable);
}
int radar_AHM2D::rdr_get_staticObjRemoval(uint8_t* penable) {
	mGetU8(RADARCOMM_SG_STATICOBJREMOVAL, penable);
}
int radar_AHM2D::rdr_set_staticObjMapUpdateTime(float timeConstant) {
	mSetFloat(RADARCOMM_SG_STATICOBJMAPUPDATETIME, timeConstant);
}
int radar_AHM2D::rdr_get_staticObjMapUpdateTime(float* ptimeConstant) {
	mGetFloat(RADARCOMM_SG_STATICOBJMAPUPDATETIME, ptimeConstant);
}
int radar_AHM2D::rdr_set_rxGain(uint8_t val) {
	mSetU8(RADARCOMM_SG_RXGAIN, val);
}
int radar_AHM2D::rdr_get_rxGain(uint8_t* pval) {
	mGetU8(RADARCOMM_SG_RXGAIN, pval);
}
int radar_AHM2D::rdr_set_embeddedImgAlgo(uint8_t enable) {
	mSetU8(RADARCOMM_SG_EMBEDDEDIMGALGO, enable);
}
int radar_AHM2D::rdr_get_embeddedImgAlgo(uint8_t* penable) {
	mGetU8(RADARCOMM_SG_EMBEDDEDIMGALGO, penable);
}


int radar_AHM2D::rdr_set_multistreammode(uint16_t nstream){
	mSetU16(RADARCOMM_SG_MULTISTREAM, nstream);
}
int radar_AHM2D::rdr_get_multistreammode(uint16_t *pnstream){
	mGetU16(RADARCOMM_SG_MULTISTREAM, pnstream);
}

//convert mask into sequence, used for reconstruction
int radar_AHM2D::rdr_conv_mask2sequence(uint8_t * txMask, uint8_t * rxMask, scanSequence_t * pseq, int num){
	//check number, not valid if zero
	if (!num)
		return RDR_ERROR;

	//check range (txMask must be 1 2 4 8)
	pseq->numAnt = num;

	for(int i = 0 ; i < num; i++){
		bool txSet, rxSet;
		txSet = false;
		rxSet = false;
		for(int k = 0 ; k < 3; k++){
			if (mAHMANT_Index2Mask(k) == txMask[i]){
				txSet = true;
				pseq->txI[i] = k;
			}
			if (mAHMANT_Index2Mask(k) == rxMask[i]){
				rxSet = true;
				pseq->rxI[i] = k;
			}
		}
		if (!rxSet || !txSet){
			return RDR_ERROR; //invalid encoding
		}
	}
	return 0;

}


int radar_AHM2D::rdr_set_code(int8_t *code, int size){
	if (size == 0 || size >AHM_CODE_MAX_LENGTH)
		return RDR_ERR_INVALID_PARAM;
	//verify code
	for(int i = 0 ; i < size ; i++) {
		if (abs((int)code[i]) > 1){
			return RDR_ERR_INVALID_PARAM;
		}
	}
	radar_command_payload payload;
	payload.data_struct.size = size;
	memcpy((void*)payload.data_struct.buffer, (const void*)code, size);
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_CODE), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;

}
int radar_AHM2D::rdr_get_code(int8_t *code, int *size){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_CODE), &payload);
	if (ret)
		return RDR_ERROR;
	if (*size > payload.data_struct.size)
		*size = payload.data_struct.size;
	memcpy((void*)code, (const void*)payload.data_struct.buffer, *size);
	return 0;
}
int radar_AHM2D::rdr_set_gain(uint16_t gainI, uint16_t gainQ){
	radar_command_payload payload;
	payload.data_struct.size = 4;
	memcpy(payload.data_struct.buffer, &gainI, 2);
	memcpy(payload.data_struct.buffer+2, &gainQ, 2);
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_GAIN), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_get_gain(uint16_t * gainI, uint16_t *gainQ){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_GAIN), &payload);
	if (ret)
		return RDR_ERROR;
	memcpy(gainI, payload.data_struct.buffer, 2);
	memcpy(gainQ, payload.data_struct.buffer+2,  2);
	return 0;
}
int radar_AHM2D::rdr_get_adcfreq(uint16_t *fMhz){
	mGetU16(RADARCOMM_GET_SAMPLING_RATE_MHZ, fMhz);
}
int radar_AHM2D::rdr_get_antConfig(txAntDesc_t * ptx, rxAntDesc_t * prx){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_ANT_ARRAY), &payload);
	if (ret)
		return RDR_ERROR;

	if (payload.data_struct.size != 17*8){
		return RDR_ERROR;
	}
	int numSettings = 8;
	float pdataf[4];

	for(int i = 0; (i< numSettings); i++){
		//pdataf = (float*)&(payload.data_struct.buffer[i*17+1]);
		memcpy((void*)pdataf, (const void*)&(payload.data_struct.buffer[i*17+1]), sizeof(pdataf));
		uint8_t antID = payload.data_struct.buffer[17*i];
		float curX, curY, del, ampl;
		curX = pdataf[0];
		curY = pdataf[1];
		del = pdataf[2];
		ampl = pdataf[3];
		if (mRDRCOM_antarrayID_isRx(antID)){
			//set Rx
			antID = mRDRCOM_decode_antArrayID(antID);
			prx->ant[antID].delay_s = del;
			prx->ant[antID].x = curX;
			prx->ant[antID].y = curY;
			prx->ant[antID].ampl = ampl;
		}else{
			antID = mRDRCOM_decode_antArrayID(antID);
			ptx->ant[antID].delay_s = del;
			ptx->ant[antID].x = curX;
			ptx->ant[antID].y = curY;
			ptx->ant[antID].ampl = ampl;
		}
	}
	return 0;
}
int radar_AHM2D::rdr_set_canvas(canvas2d_param_t canvas){
	radar_command_payload payload;
	payload.data_struct.size = 6*4+1;
	payload.data_struct.buffer[0] = canvas.algo;
	float *pval = (float*)(payload.data_struct.buffer+1);
	pval[0] = canvas.rhoMin;
	pval[1] = canvas.rhoStep;
	pval[2] = canvas.rhoMax;
	pval[3] = canvas.phiMin;
	pval[4] = canvas.phiStep;
	pval[5] = canvas.phiMax;
	int ret = rdr_set_val(getCommandItem(RADARCOMM_SET_CANVAS), &payload);
	if (ret)
		return RDR_ERROR;
	return 0;
}
int radar_AHM2D::rdr_get_canvas(canvas2d_param_t * p){
	radar_command_payload payload;
	int ret = rdr_get_val(getCommandItem(RADARCOMM_SET_CANVAS), &payload);
	if (ret)
		return RDR_ERROR;
	if(payload.data_struct.size != 6*4+1){
		return RDR_ERROR;
	}
	float * pval;
	pval = (float*)(payload.data_struct.buffer+1);
	p->algo = (recAlgo_t)payload.data_struct.buffer[0];
	p->rhoMin = pval[0];
	p->rhoStep = pval[1];
	p->rhoMax = pval[2];
	p->phiMin = pval[3];
	p->phiStep = pval[4];
	p->phiMax = pval[5];
	return 0;
}
int radar_AHM2D::rdr_set_data_fmt(uint8_t fmt){
	mSetU8(RADARCOMM_SET_DATA_FMT, fmt);
}
int radar_AHM2D::rdr_get_data_fmt(uint8_t * fmt){
	mGetU8(RADARCOMM_SET_DATA_FMT, fmt);
}
int radar_AHM2D::rdr_rx_multichannel(radarData<cplx_float_t> * prd){


	uint8_t cmd = getCommandItem(RADARCOMM_GET_DATA)->cmdCode;
	int inSize = rh->txrxMessage(&cmd, 1, NULL, 0); //plan to direct access to buffer data

	if (inSize < 0)
		return RDR_ERR_COMM;
	uint8_t * pbuffer = rh->rxbuf.pdata+1;
	//check command
	if (pbuffer[0] != cmd){
		return RDR_ERR_INVALID_STATE; //radar is in a not valid state for this request
	}

	pbuffer++;
	inSize--;
	//readout canonical data
	uint16_t headerSamples;
	uint16_t etime_fmt;
	uint16_t totalSamples;
	memcpy((void*)&headerSamples, (const void*)pbuffer, 2);
	memcpy((void*)&etime_fmt, (const void*)(pbuffer+2), 2);
	memcpy((void*)&totalSamples, (const void*)(pbuffer+4), 2);
	inSize -= 6;
	pbuffer += 6;

	uint16_t fmt = etime_fmt >> 12;
	//compute expected samples
	uint32_t expectedSize = (uint32_t)totalSamples *2;
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
			return RDR_ERR_INVALID_PARAM;
			break;
	}


	if (expectedSize == inSize){
		//verify the number of channels
		int numChannels = totalSamples/headerSamples;
		prd->resize(numChannels, headerSamples);
		//decode and save
		for(int ch = 0; ch < numChannels; ch++){
			for(int s = 0; s < headerSamples; s++){
				cplx_float_t curSample;
				float vals_f[2];

				switch(fmt) {
					case FMT_Q7:
						curSample.re = ((int8_t*)pbuffer)[0];
						curSample.im = ((int8_t*)pbuffer)[1];
						pbuffer += 2;
						prd->set(ch, s, curSample);
						break;
					case FMT_Q15:
						curSample.re = ((int16_t*)pbuffer)[0];
						curSample.im = ((int16_t*)pbuffer)[1];
						pbuffer += 4;
						prd->set(ch, s, curSample);
						break;
					case FMT_F16:
						f16tosingle((float16_t*)pbuffer, vals_f, 2);
						pbuffer += 4;
						curSample.re = vals_f[0];
						curSample.im = vals_f[1];
						prd->set(ch, s, curSample);
						break;
					case FMT_Q31:
						curSample.re = ((int32_t*)pbuffer)[0];
						curSample.im = ((int32_t*)pbuffer)[1];
						pbuffer += 8;
						prd->set(ch, s, curSample);
						break;
					case FMT_F32:
						curSample.re = ((float*)pbuffer)[0];
						curSample.im = ((float*)pbuffer)[1];
						pbuffer += 8;
						prd->set(ch, s, curSample);
						break;
				}
			}
		}
	}else
		return RDR_ERROR;
	return 0; //no error
}

int radar_AHM2D::rdr_get_versExtended(std::string* pstr, uint8_t type) {
	uint8_t verStr[256]; //preallocate array
	uint8_t reqPayload[2];
	reqPayload[0] = RDR_VERSION_EXT_CMD;
	reqPayload[1] = type;
	int ret = rh->txrxMessage(reqPayload, 2, verStr, sizeof(verStr));
	if (ret < 1)
		return RDR_ERR_COMM;
	//check
	if (verStr[0] != RDR_VERSION_EXT_CMD)
		return RDR_ERR_COMM;
	//
	verStr[ret] = 0; //place null
	pstr->assign((const char*)(verStr + 1));
	return 0;

}

int radar_AHM2D::rdr_get_image(radarData<float> * prd){


	uint8_t cmd = getCommandItem(RADARCOMM_GET_IMG)->cmdCode;
	int inSize = rh->txrxMessage(&cmd, 1, NULL, 0); //plan to direct access to buffer data

	if (inSize < 0)
		return RDR_ERR_COMM;
	uint8_t * pbuffer = rh->rxbuf.pdata+1;
	//check command
	if (pbuffer[0] != cmd){
		return RDR_ERR_INVALID_STATE; //radar is in a not valid state for this request
	}

	pbuffer++;
	inSize--;
	//readout canonical data
	uint16_t clmns, rows;
	uint16_t etime_fmt;
	uint16_t totalSamples;
	memcpy((void*)&clmns, (const void*)pbuffer, 2);
	memcpy((void*)&etime_fmt, (const void*)(pbuffer+2), 2);
	memcpy((void*)&rows, (const void*)(pbuffer+4), 2);
	inSize -= 6;
	pbuffer += 6;
	totalSamples = clmns*rows;

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
			return RDR_ERR_INVALID_PARAM;
			break;
	}


	if (expectedSize == inSize){
		//verify the number of channels
		prd->resize(rows, clmns);
		//decode and save
		for(int r = 0; r < rows; r++){
			for(int c = 0; c < clmns; c++){
				float curSample;

				switch(fmt) {
					case FMT_Q7:
						curSample = ((int8_t*)pbuffer)[0];
						pbuffer += 1;
						prd->set(r, c, curSample);
						break;
					case FMT_Q15:
						curSample = ((int16_t*)pbuffer)[0];
						pbuffer += 2;
						prd->set(r, c, curSample);
						break;
					case FMT_F16:
						f16tosingle((float16_t*)pbuffer, &curSample, 1);
						pbuffer += 2;
						prd->set(r, c, curSample);
						break;
					case FMT_Q31:
						curSample = ((int32_t*)pbuffer)[0];
						pbuffer += 4;
						prd->set(r, c, curSample);
						break;
					case FMT_F32:
						curSample = ((float*)pbuffer)[0];
						pbuffer += 4;
						prd->set(r, c, curSample);
						break;
				}
			}
		}
	}else
		return RDR_ERROR;

	return 0; //no error
}


const RadarCommandItem_t * radar_AHM2D::getCommandItem(uint32_t srcCmd){
	return radar_api_base::getCommandItem(srcCmd, RadarCommandItem, RadarCommandTblItems);
}





