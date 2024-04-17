#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)
extern size_t get_ramdisk_size();
extern void ramdisk_read(void *, off_t, size_t);

uintptr_t loader(_Protect *as, const char *filename) {
  //将ramdisk中从0开始的所有内容放置在default_entry处的位置
  size_t size = get_ramdisk_size();
  ramdisk_read(DEFAULT_ENTRY, 0, size); 
  return (uintptr_t)DEFAULT_ENTRY;
}
