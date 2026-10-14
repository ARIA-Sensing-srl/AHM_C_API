/*
 * radar_AHM3D.h
 *
 *  Created on: 21 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */

#ifndef RADAR_AHM3D_H_
#define RADAR_AHM3D_H_

#include "radar_AHM2D.h"
#include "radar_api_base.h"
#include "radarDataVolume.h"



class radar_AHM3D : public radar_AHM2D {

public:
	radar_AHM3D(rifll_h*);
	~radar_AHM3D();
    int rdr_set_canvas(canvas3d_param_t) override;
    int rdr_get_canvas(canvas3d_param_t *) override;
	radarDataVolume<float> * rdr_get_volume();

private:
	static const RadarCommandItem_t RadarCommandItemTbl[];

};



#endif /* RADAR_AHM3D_H_ */
