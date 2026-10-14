/*
 * radar_api_base.h
 *
 *  Created on: 20 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */

#ifndef INC_RADAR_AHM2D_H_
#define INC_RADAR_AHM2D_H_

#include "radarif_ll.h"
#include "rdr_common_types.h"
#include "radar_api_base.h"
#include "ahm_common.h"
#include "radarData.h"


class radar_AHM2D : public radar_api_base {



public:
	//public functions common between all radars
	radar_AHM2D(rifll_h*);
	radar_AHM2D(rifll_h*, uint16_t); //this contructor is used for derived class
	radar_AHM2D(){};
	virtual ~radar_AHM2D();
    int rdr_start() override;
    int rdr_stop() override;
    int rdr_set_frame_rate(uint16_t) override;
    int rdr_get_frame_rate(uint16_t *) override;
    int rdr_set_tx_power(uint8_t) override;
    int rdr_get_tx_power(uint8_t *) override;
    int rdr_set_min_Xrange (float) override;
    int rdr_get_min_Xrange (float *) override;
    int rdr_set_max_Xrange (float) override;
    int rdr_get_max_Xrange (float *) override;
    int rdr_set_offset (float) override;
    int rdr_get_offset (float *) override;
    int rdr_set_iterations (uint16_t) override;
    int rdr_get_iterations (uint16_t *) override;
    int rdr_set_declutter_length (uint16_t) override;
    int rdr_get_declutter_length (uint16_t *) override;
    int rdr_set_elabtype(rdrElabTypes_t) override;
    int rdr_get_elabtype(rdrElabTypes_t *) override;
    int rdr_set_optproc(uint8_t opt0, uint8_t opt1) override;
    int rdr_get_optproc(uint8_t *opt0, uint8_t *opt1) override;
    int rdr_set_sequence(uint8_t * txSeqMask, uint8_t* rxSeqMask, uint8_t numSeq) override;
    int rdr_get_sequence(uint8_t * txSeqMask, uint8_t* rxSeqMask, uint8_t * pnumSeq) override;
    int rdr_set_freq_carrier(uint16_t carrierMHz) override;
    int rdr_get_freq_carrier(uint16_t * carrierMHz) override;
    int rdr_set_bw(uint16_t bwMHz) override;
    int rdr_get_bw(uint16_t * bwMHz) override;
    int rdr_set_bwmode(uint8_t bwmode) override;
    int rdr_get_bwmode(uint8_t * bemode) override;
    int rdr_set_code(int8_t *code, int size) override;
    int rdr_get_code(int8_t *code, int *size) override;
    int rdr_set_gain(uint16_t gainI, uint16_t gainQ) override;
    int rdr_get_gain(uint16_t * gainI, uint16_t *gainQ) override;
    int rdr_get_fSampling(uint16_t *fMhz) override;
    int rdr_get_arrayDescr(txAntDesc_t *, rxAntDesc_t *) override;
    int rdr_set_canvas(canvas2d_param_t) override;
    int rdr_get_canvas(canvas2d_param_t *) override;
    int rdr_set_fmt(uint8_t fmt) override;
    int rdr_get_fmt(uint8_t * fmt) override;
    int rdr_rx_multichannel(radarData<cplx_float_t> * prd) override;
    int rdr_get_image(radarData<float> * prd) override;
    int rdr_set_baudrate(uint32_t br) override;
    int rdr_set_multistream(uint16_t nstream) override;
    int rdr_get_multistream(uint16_t *pnstream) override;
    int rdr_conv_mask2sequence(uint8_t * txMask, uint8_t * rxMask, scanSequence_t * pseq, int num) override;

protected:
	const RadarCommandItem_t * RadarCommandItem;
	int RadarCommandTblItems;
	const RadarCommandItem_t * getCommandItem(uint32_t srcCmd);

private:
	static const RadarCommandItem_t RadarCommandItemTbl[];

};



#define METERS_PER_BIN  0.0070861678004535151f



#endif
