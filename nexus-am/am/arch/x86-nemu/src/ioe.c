#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  unsigned long t=inl(RTC_PORT)-boot_time;
  return t;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  //矩形绘制不会超出屏幕的右边缘
  int r_w = (w > _screen.width - x) ? _screen.width - x : w;
  //一行绘制的bytes
  int w_bytes = r_w * sizeof(uint32_t);
  for (int i = 0; i < h && y + i < _screen.height; i++) {
    memcpy(&fb[(y + i) * _screen.width + x], pixels+i*w, w_bytes);
  }
}

void _draw_sync() {
}

int _read_key() {
  if(inb(0x64)){
    return inl(0x60);
  }
  return _KEY_NONE;
}
