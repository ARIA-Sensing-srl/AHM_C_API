/*
 * f16tosingle.h
 *
 *  Created on: 03 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */


#ifndef F16TOSINGLE_H
#define F16TOSINGLE_H

#include "stdint.h"

typedef uint16_t float16_t;
void f16tosingle(float16_t * pin, float *pdst, int elem);


#endif
