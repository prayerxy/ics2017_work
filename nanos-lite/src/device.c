#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};
//把事件写入到buf中, 最长写入len字节, 然后返回写入的实际长度。
size_t events_read(void *buf, size_t len) {
  char buffer[40];
  int key = _read_key();
  bool isDown = false;
  if(key & 0x8000 ) {
    key ^= 0x8000;
    isDown = true;
  }
  // //返回系统启动后的时间
  // if(key == _KEY_NONE) {
  //   unsigned long t = _uptime();
  //   sprintf(buf, "t %d\n", t);
  // } 
  // else {//按键
  //   sprintf(buf, "%s %s\n", isDown ? "kd" : "ku", keyname[key]);
  // }
  // return strlen(buf);
  //}
  if(key != _KEY_NONE) {
    sprintf(buffer, "%s %s\n", isDown ? "kd": "ku", keyname[key]);
  }
  else {
  sprintf(buffer,"t %d\n", _uptime());
  }
  if(strlen(buffer) <= len) {
    strncpy((char*)buf, buffer,strlen(buffer));
    return strlen(buffer);
  }
    Log("strlen(event)>len, return 0");
    return strlen(buf);
  
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  //用于把字符串dispinfo中offset开始的len字节写到buf中.
  memcpy(buf, dispinfo + offset, len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
  //用于把buf中的len字节写到屏幕上offset处
  //需要先从offset计算出屏幕上的坐标, 然后调用IOE的_draw_rect()接口.
  assert(len>=0);
  offset=offset/4;
  int x=offset%_screen.width;
  int y=offset/_screen.width;
  _draw_rect((uint32_t*)buf,x,y,len/4,1);
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d",_screen.width,_screen.height);
}
