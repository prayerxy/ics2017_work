#ifndef __FLOAT_H__
#define __FLOAT_H__

#include "assert.h"

typedef int FLOAT;
FLOAT f2F(float);
FLOAT F_mul_F(FLOAT, FLOAT);
FLOAT F_div_F(FLOAT, FLOAT);
FLOAT Fabs(FLOAT);
FLOAT Fsqrt(FLOAT);
FLOAT Fpow(FLOAT, FLOAT);
static inline int F2int(FLOAT a) {
  if((a&0x80000000)==0){
    return a>>16;
  }
  else{
    return -((-a)>>16);
  }
}

static inline FLOAT int2F(int a) {
  if((a&0x80000000)==0){
    return a<<16;
  }
  else{
    //整体取反
    return -((-a)<<16);
  }
}

static inline FLOAT F_mul_int(FLOAT a, int b) {
  return F_mul_F(a,int2F(b));
}

static inline FLOAT F_div_int(FLOAT a, int b) {
  return F_div_F(a,int2F(b));
}



#endif
