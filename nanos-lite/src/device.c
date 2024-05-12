#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  //用于把字符串dispinfo中offset开始的len字节写到buf中.
  strncpy(buf, dispinfo + offset, len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
  //用于把buf中的len字节写到屏幕上offset处
  //需要先从offset计算出屏幕上的坐标, 然后调用IOE的_draw_rect()接口.
  if(len<=0){return;}
  offset=offset/4;
  int x=offset%_screen.width;
  int y=offset/_screen.width;
  _draw_rect((uint32_t*)buf,x,y,len/4,1);
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\n HEIGHT:%d\n",_screen.width,_screen.height);
}
