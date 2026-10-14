/*
 * radarData.cpp
 *
 *  Created on: 24 gen 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
 */



#include "radarData.h"
#include "stdlib.h"

template class radarData<float>;
template class radarData<cplx_float_t>;

template <typename T> radarData<T>::radarData(int rows, int clmns){
	//
	int totSize = rows * clmns * sizeof(T);
	pdata = (T*)malloc(totSize);
	r = rows;
	c = clmns;

}

template <typename T> radarData<T>::radarData(){
	r = 0;
	c= 0;
	pdata = NULL;
}


template <typename T> void radarData<T>::resize(int rows, int clmns){
	if ((rows == this->r) && (clmns == this->c)){
		return;
	}
	free((void*)pdata);
	int totSize = rows * clmns * sizeof(T);
	pdata = (T*)malloc(totSize);
	r = rows;
	c = clmns;
}


template <typename T> radarData<T>::~radarData(){
	free((void*)pdata);
}

template <typename T> T radarData<T>::val(int ir, int ic){
	int i = ir * this->c + ic;
	return pdata[i];
}

template <typename T> void radarData<T>::set(int ir, int ic, T value){
	int i = ir * this->c + ic;
	pdata[i] = value;
}

template <typename T> int radarData<T>::numClmns(){return c;};
template <typename T> int radarData<T>::numRows(){return r;};
template <typename T> int radarData<T>::numItems(){return r*c;};


template <typename T> void radarData<T>::getClmn(int ic, T * pout){
	for(int i = 0 ; i < this->r; i++){
		pout[i] = this->val(i, ic);
	}
}
template <typename T> void radarData<T>::getRaw(int ir, T * pout){
	for(int i = 0 ; i < this->c; i++){
		pout[i] = this->val(ir, i);
	}
}

template <typename T> T radarData<T>::operator[](int index){
	return pdata[index];
}
