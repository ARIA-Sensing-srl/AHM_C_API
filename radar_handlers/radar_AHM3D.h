/*
 * radar_AHM3D.h
 *
 *  Created on: 21 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
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

    int rdr_set_SNRs(float SNRs[4]) override { return RDR_ERR_VERS_UNSUPPORTED; };
    int rdr_set_SNRranges(float SNRranges[3]) override { return RDR_ERR_VERS_UNSUPPORTED; }
    int rdr_set_SNRpwrs(float SNRpwrs[3]) override { return RDR_ERR_VERS_UNSUPPORTED; }
    int rdr_get_SNRs(float SNRs[4]) override { return RDR_ERR_VERS_UNSUPPORTED; }
    int rdr_get_SNRranges(float SNRranges[3]) override { return RDR_ERR_VERS_UNSUPPORTED; }
    int rdr_get_SNRpwrs(float SNRpwrs[3]) override { return RDR_ERR_VERS_UNSUPPORTED; }
    int rdr_set_blobFilter(blobFilter_param_t) override { return RDR_ERR_VERS_UNSUPPORTED; }
    int rdr_get_blobFilter(blobFilter_param_t*) override { return RDR_ERR_VERS_UNSUPPORTED; }
    int rdr_set_kalman2D(kalmand_2D_param_t) override { return RDR_ERR_VERS_UNSUPPORTED; }
    int rdr_get_kalman2D(kalmand_2D_param_t*) override { return RDR_ERR_VERS_UNSUPPORTED; }
    int rdr_get_detection2D(std::vector<det_blob_t>* p) override { return RDR_ERR_VERS_UNSUPPORTED; }
    int rdr_get_detection2D(std::vector<det_pos_t>* p, bool filtered) override { return RDR_ERR_VERS_UNSUPPORTED; }
    int rdr_get_detection2D(std::vector<det_pos_full_t>* p, bool filtered) override { return RDR_ERR_VERS_UNSUPPORTED; }

private:
	static const RadarCommandItem_t RadarCommandItemTbl[];

};



#endif /* RADAR_AHM3D_H_ */
