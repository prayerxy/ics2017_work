#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

FLOAT F_mul_F(FLOAT a, FLOAT b) {
  return ((int64_t)a*(int64_t)b)>>16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  //a/b*2^16
  FLOAT res=Fabs(a)/Fabs(b);
  FLOAT m=Fabs(a);
  FLOAT n=Fabs(b);
  m=m%n;
  for(int i=0;i<16;i++){
    res<<=1;
    m<=1;
    if(m>=n){m-=n;res|=1;}
  }
  if (((a ^ b) & 0x80000000) == 0x80000000) {
    res=-res;
  }
  return 0;
}

FLOAT f2F(float a) {
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */
  union float_ {
    struct {
      uint32_t m : 23;//尾数部分
      uint32_t e : 8;//指数部分
      uint32_t signal : 1;//符号位部分
    };
    uint32_t value;
  };
  union float_ f;
  f.value = *((uint32_t*)(void*)&a);
  int e=f.e-127;//真实的指数位
  uint32_t res;
  uint32_t temp;
  //-127与128
  //已经左移23位，现在右移动7位修正，然后指数e左移动
  if (e==128)
    assert(0);//无穷大或者NaN
  else if(e==-127){
    temp=f.m;
    e+=1;//非规格化
  }
  else{
    temp=(f.m |(1<<23));
  }
  if(e<=7){
    res=temp>>(7-e);
  }
  else{
    res=temp<<(e-7);
  }
  return 0;
  // return f.signal==0?res:-res;
}

FLOAT Fabs(FLOAT a) {
  return (a>0)?a:-a;
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x) {
  FLOAT dt, t = int2F(2);

  do {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y) {
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}
