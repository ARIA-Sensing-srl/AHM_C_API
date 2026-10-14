/*
 * radarDataVolume.h
 *
 *  Created on: 21 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */

#ifndef RADARDATAVOLUME_H_
#define RADARDATAVOLUME_H_



//this class mimic the storage template inside the module, data from serial could be direcly transferred to the buffer

template <class T> class radarDataVolume{

private:
	T * pdata;
	int nRho;
	int nEl;
	int nAzim;
	int toIndex(int irho, int iel, int iazim);

public:
	radarDataVolume(int rhoPts, int elPts, int azimPts);
	~radarDataVolume();
	T val(int irho, int iel, int iazim);
	void set(int irho, int iel, int iazim, T value);
	int numRho();
	int numEl();
	int numAzim();
	int numItems();
	T * getBuffer();
};


#endif /* RADARDATAVOLUME_H_ */
