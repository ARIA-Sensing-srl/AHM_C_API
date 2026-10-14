/*
 * radar_AHM2DL.h
 *
 *  Created on: Jan 7, 2026
 *     Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 *
 *  Handler for AHM2DL module, communication is the same of the AHM2D.
 */

#ifndef RADAR_AHM2DL_H_
#define RADAR_AHM2DL_H_

#include "radar_AHM2D.h"
#include "radar_api_base.h"
#include "radarDataVolume.h"



class radar_AHM2DL : public radar_AHM2D {

public:
	radar_AHM2DL(rifll_h*);
	~radar_AHM2DL();

private:
	static const RadarCommandItem_t RadarCommandItemTbl[];

};

#endif /* RADAR_AHM2DL_H_ */
