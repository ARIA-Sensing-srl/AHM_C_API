/*
 * radarData.h
 *
 *  Created on: 24 gen 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
 */

#ifndef INC_RADARDATA_H_
#define INC_RADARDATA_H_


typedef struct _cplx_float_t{
	float re;
	float im;
}cplx_float_t;


template <class T> class radarData {

private:
	T * pdata;
	int c;
	int r;

public:
	void getClmn(int ic, T *);
	void getRaw(int ir, T *);
	radarData(int rows, int clms);
	radarData();
	void resize(int rows, int clmns);
	~radarData();
	T val(int ir, int ic);
	void set(int ir, int ic, T value);
	int numClmns();
	int numRows();
	T operator[](int index);
	int numItems();
};


#endif /* INC_RADARDATA_H_ */
