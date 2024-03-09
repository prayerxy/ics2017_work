#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;//表示监视点的序号
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  int value;
  char expr[100];

} WP;

#endif
