/*
 * radar_api_base.h
 *
 *  Created on: 21 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */

#ifndef RADAR_API_BASE_H_
#define RADAR_API_BASE_H_

#include "radarif_ll.h"
#include "rdr_common_types.h"
#include "ahm_common.h"
#include "radarData.h"

typedef enum _rdrElabTypes_t{ 
    RT_UNKNOWN = -1, 	
    RT_RAW = 0, 		
    RT_MTI = 1, 		
}rdrElabTypes_t;

typedef struct _RadarCommandItem_t{
	unsigned char cmdCode;	 //code of the command
	uint32_t cmd;
	payloadType_t payloadType;
	uint32_t queryPayload;//for struct query payload is structure size
}RadarCommandItem_t;


class radar_api_base {

protected:
	rifll_h * rh;
	int rdr_get_val(const RadarCommandItem_t * pitem, radar_command_payload * ppayload);
	int rdr_set_val(const RadarCommandItem_t * pitem, radar_command_payload * ppayload);
	const RadarCommandItem_t * getCommandItem(uint32_t srcCmd, const RadarCommandItem_t * tbl, int tblSize);

public:
	radar_api_base(rifll_h*, uint16_t hw);
	radar_api_base();
	virtual ~radar_api_base();
	rifll_h* get_comm_handler(){return rh;};
	static int getHW(rifll_h * h, uint16_t * pHWCode);
	int getHW(uint16_t * pHWCode);
	const uint16_t HWCODE;
	virtual int rdr_start(){return -1;};
	virtual int rdr_stop(){return -1;};
	virtual int rdr_set_frame_rate(uint16_t){return -1;};
	virtual int rdr_get_frame_rate(uint16_t *){return -1;};
	virtual int rdr_set_tx_power(uint8_t){return -1;};
	virtual int rdr_get_tx_power(uint8_t *){return -1;};
	virtual int rdr_set_min_Xrange (float){return -1;};
	virtual int rdr_get_min_Xrange (float *){return -1;};
	virtual int rdr_set_max_Xrange (float){return -1;};
	virtual int rdr_get_max_Xrange (float *){return -1;};
	virtual int rdr_set_offset (float){return -1;};
	virtual int rdr_get_offset (float *){return -1;};
	virtual int rdr_set_iterations (uint16_t){return -1;};
	virtual int rdr_get_iterations (uint16_t *){return -1;};
	virtual int rdr_set_declutter_length (uint16_t){return -1;};
	virtual int rdr_get_declutter_length (uint16_t *){return -1;};
	virtual int rdr_set_elabtype(rdrElabTypes_t){return -1;};
	virtual int rdr_get_elabtype(rdrElabTypes_t *){return -1;};
	virtual int rdr_set_optproc(uint8_t opt0, uint8_t opt1){return -1;};
	virtual int rdr_get_optproc(uint8_t *opt0, uint8_t *opt1){return -1;};
	virtual int rdr_set_sequence(uint8_t * txSeqMask, uint8_t* rxSeqMask, uint8_t numSeq){return -1;};
	virtual int rdr_get_sequence(uint8_t * txSeqMask, uint8_t* rxSeqMask, uint8_t * pnumSeq){return -1;};
	virtual int rdr_set_freq_carrier(uint16_t carrierMHz){return -1;};
	virtual int rdr_get_freq_carrier(uint16_t * carrierMHz){return -1;};
	virtual int rdr_set_bw(uint16_t bwMHz){return -1;};
	virtual int rdr_get_bw(uint16_t * bwMHz){return -1;};
	virtual int rdr_set_bwmode(uint8_t bwmode){return -1;};
	virtual int rdr_get_bwmode(uint8_t * bemode){return -1;};
	virtual int rdr_set_code(int8_t *code, int size){return -1;};
	virtual int rdr_get_code(int8_t *code, int *size){return -1;};
	virtual int rdr_set_gain(uint16_t gainI, uint16_t gainQ){return -1;};
	virtual int rdr_get_gain(uint16_t * gainI, uint16_t *gainQ){return -1;};
    virtual int rdr_get_fSampling(uint16_t * fMhz){return -1;};
	virtual int rdr_get_arrayDescr(txAntDesc_t *, rxAntDesc_t *){return -1;};
	virtual int rdr_set_canvas(canvas2d_param_t){return -1;};
	virtual int rdr_get_canvas(canvas2d_param_t *){return -1;};
	virtual int rdr_set_fmt(uint8_t fmt){return -1;};
	virtual int rdr_get_fmt(uint8_t * fmt){return -1;};
	virtual int rdr_rx_multichannel(radarData<cplx_float_t> * prd){return -1;};
	virtual int rdr_get_image(radarData<float> * prd){return -1;};
	virtual int rdr_set_baudrate(uint32_t br){return -1;};
	virtual int rdr_set_multistream(uint16_t nstream){return -1;};
    virtual int rdr_get_multistream(uint16_t * pnstream){return -1;};
	virtual int rdr_conv_mask2sequence(uint8_t * txMask, uint8_t * rxMask, scanSequence_t * pseq, int num){return -1;};
	virtual int rdr_set_canvas(canvas3d_param_t){return -1;};
	virtual int rdr_get_canvas(canvas3d_param_t *){return -1;};
	//new
	virtual int rdr_set_profile(profile_t) { return -1; };
	virtual int rdr_set_staticObjRemoval(uint8_t enable) { return -1; };
	virtual int rdr_get_staticObjRemoval(uint8_t * penable) { return -1; };
	virtual int rdr_set_staticObjMapUpdateTime(float timeConstant) { return -1; };
	virtual int rdr_get_staticObjMapUpdateTime(float * ptimeConstant) { return -1; };
	virtual int rdr_set_rxGain(uint8_t val) { return -1; };
	virtual int rdr_get_rxGain(uint8_t * pval) { return -1; };
	virtual int rdr_set_embeddedImgAlgo(uint8_t enable) { return -1; };
	virtual int rdr_get_embeddedImgAlgo(uint8_t* penable) { return -1; };

};

#define RDR_RET_DATA_COMPLEX 1
#define RDR_RET_DATA_REAL 0

#define RDR_SUCCESS 0
#define RDR_ERROR -1
#define RDR_ERR_COMM -2
#define RDR_ERR_VERS_UNSUPPORTED -3
#define RDR_ERR_INVALID_PARAM -4
#define RDR_BOOT_ERROR -5
#define RDR_BOOT_ERROR_NO_FW -6
#define RDR_ERR_INVALID_STATE -7
#define RDR_ERR_NO_MEMORY -8

#endif /* RADAR_API_BASE_H_ */
