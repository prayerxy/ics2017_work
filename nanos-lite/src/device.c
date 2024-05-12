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
  // if(len<=0){return;}
  // offset=offset/4;
  // int x=offset%_screen.width;
  // int y=offset/_screen.width;
  // _draw_rect((uint32_t*)buf,x,y,len/4,1);
  assert(offset % 4 == 0 && len % 4 == 0);
int index, screen_x1, screen_y1, screen_y2;
  int width=_screen.width;
  index=offset/4;
  screen_y1=index/width;
  screen_x1=index%width;
  index=(offset+len)/4;
  screen_y2=index/width;
  assert(screen_y2>=screen_y1);
  if(screen_y2==screen_y1)
  {
  _draw_rect(buf,screen_x1,screen_y1,len/4,1);
  return ;
  }
  int tempw=width-screen_x1;
  if(screen_y2-screen_y1==1)
  {
  _draw_rect(buf,screen_x1,screen_y1,tempw,1);
  _draw_rect(buf+tempw * 4 ,0,screen_y2,len/4-tempw,1);
  return ;
  }
  _draw_rect(buf, screen_x1, screen_y1, tempw, 1);
  int tempy = screen_y2 - screen_y1 - 1;
  _draw_rect(buf + tempw * 4, 0, screen_y1 + 1, width, tempy);
  _draw_rect(buf+tempw*4+tempy*width*4,0,screen_y2, len / 4 - tempw - tempy * width,
  1);
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\n HEIGHT:%d\n",_screen.width,_screen.height);
}
