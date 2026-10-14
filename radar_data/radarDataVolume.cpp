/*
 * radarDataVolume.cpp
 *
 *  Created on: 21 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
 */

#include "radarDataVolume.h"
#include "stdlib.h"

template class radarDataVolume<float>;

template <typename T> radarDataVolume<T>::radarDataVolume(int rhoPts, int elPts, int azimPts){
	int numPts = rhoPts*elPts*azimPts;
	pdata = (T*)malloc(numPts * sizeof(T));
	nAzim = azimPts;
	nEl = elPts;
	nRho = rhoPts;
}
template <typename T> radarDataVolume<T>::~radarDataVolume(){
	free((void*)pdata);
}
template <typename T> T radarDataVolume<T>::val(int irho, int iel, int iazim){
	return pdata[toIndex(irho, iel, iazim)];
}
template <typename T> void radarDataVolume<T>::set(int irho, int iel, int iazim, T value){
	pdata[toIndex(irho, iel, iazim)] = value;
}
template <typename T> int radarDataVolume<T>::numRho(){
	return nRho;
}
template <typename T> int radarDataVolume<T>::numEl(){
	return nEl;
}
template <typename T> int radarDataVolume<T>::numAzim(){
	return nAzim;
}
template <typename T> int radarDataVolume<T>::numItems(){
	return nRho*nEl*nAzim;
}
template <typename T> int radarDataVolume<T>::toIndex(int irho, int iel, int iazim){
	return irho+nRho*(iel + nEl*iazim);
}
template <typename T> T * radarDataVolume<T>::getBuffer(){
	return pdata;
}

