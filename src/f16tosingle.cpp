/*
 * f16tosingle.c
 *
 *  Created on: 03 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
 */

#include "f16tosingle.h"

static inline uint32_t as_uint(const float x){
  return *((uint32_t*)&x);
}


void f16tosingle(float16_t * pin, float *pdst, int elem)
{
  for(int i = 0 ; i< elem; i++){
    union {
      float f;
      uint32_t u32;
    }converted;
    uint32_t e,m,v;
    uint32_t curVal = (uint32_t)pin[i];
    e = (curVal & 0x7C00)>>10;
    m = (curVal & 0x03FF)<<13;
    v = as_uint((float)m)>>23;
    if (e == 0x1F){
      if (m == 0){
        converted.u32 = (0xff)<<23 | ((curVal & 0x8000) << 16);
      }else{
        converted.u32 = -1;
      }
      pdst[i] = converted.f;
    }else{
      converted.u32 = (curVal & 0x8000)<<16 | (e != 0)*((e+112)<<23|m) | ((e==0)&(m!=0))*((v-37)<<23|((m<<(150-v))&0x007FE000));
      pdst[i] = converted.f;
    }
  }
}






