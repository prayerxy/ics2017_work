#include "common.h"

/* Uncomment these macros to enable corresponding functionality. */
//#define HAS_ASYE
//#define HAS_PTE

void init_mm(void);
void init_ramdisk(void);
void init_device(void);
void init_irq(void);
void init_fs(void);
uint32_t loader(_Protect *, const char *);

int main() {
#ifdef HAS_PTE
  init_mm();
#endif
  //注意这里的log()宏不是NEMU中的，nemu与nanos-lite是独立的
  Log("'Hello World!' from Nanos-lite");
  Log("Build time: %s, %s", __TIME__, __DATE__);
  //初始化ramdisk 将一段内存作为磁盘来使用
  init_ramdisk();
  //初始化设备
  init_device();
#ifdef HAS_ASYE
  Log("Initializing interrupt/exception handler...");
  init_irq();
#endif

  init_fs();
  //加载用户程序  返回程序的入口地址
  uint32_t entry = loader(NULL, NULL);
  //跳转到用户程序入口执行
  ((void (*)(void))entry)();
  Log("'Hello World!' from Nanos-lite");

  panic("Should not reach here");
}
