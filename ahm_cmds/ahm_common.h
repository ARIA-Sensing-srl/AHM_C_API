/*
 * ahm_common.h
 *
 *  Created on: 24 gen 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */

#ifndef AHM_CMDS_AHM_COMMON_H_
#define AHM_CMDS_AHM_COMMON_H_

#include "stdint.h"

#define mAHMANT_Index2Mask(x) (1<<x)

#define AHM_ANT_TX0	mAHMANT_Index2Mask(0)
#define AHM_ANT_TX1	mAHMANT_Index2Mask(1)
#define AHM_ANT_TX2	mAHMANT_Index2Mask(2)
#define AHM_ANT_TX3	mAHMANT_Index2Mask(3)

#define AHM_ANT_RX0	mAHMANT_Index2Mask(0)
#define AHM_ANT_RX1	mAHMANT_Index2Mask(1)
#define AHM_ANT_RX2	mAHMANT_Index2Mask(2)
#define AHM_ANT_RX3	mAHMANT_Index2Mask(3)





#define FMT_Q7 			0
#define FMT_Q15 		1
#define FMT_Q31 		2
#define FMT_F32			3
#define FMT_F16 		4
#define FMT_F16CPLX 	5

#define AHM_CODE_MAX_LENGTH 64

typedef enum _recAlgo_t {RA_DAS=0, RA_DMAS=1}recAlgo_t;

typedef struct ant_t{
	float x;
	float y;
	float delay_s;
	float ampl;
}ant_t;

typedef struct canvas2d_param_t{
    float rhoMin;
    float rhoMax;
    float rhoStep;
    float phiMin;
    float phiMax;
    float phiStep;
    recAlgo_t algo;
}canvas2d_param_t;


typedef struct canvas3d_param_t{
    float rhoMin;
    float rhoMax;
    float rhoStep;
    float elMin;
    float elMax;
    float elStep;
    float azimMin;
	float azimMax;
	float azimStep;
	recAlgo_t algo	;
}canvas3d_param_t;

typedef struct _txAntDesc_t{
	ant_t ant[4];
}txAntDesc_t;

typedef struct _rxAntDesc_t{
	ant_t ant[4];
}rxAntDesc_t;

typedef struct _scanSequence_t{
	uint8_t txI[16];
	uint8_t rxI[16];
	uint8_t numAnt;
}scanSequence_t;

typedef enum _profile_t {
    PROF_SR_MID_RES = 0,
    PROF_SR_HIGH_RES = 1,
    PROF_LR_MID_RES = 2,
    PROF_LR_HIGH_RE = 3
}profile_t;

#endif /* AHM_CMDS_AHM_COMMON_H_ */
