#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)
extern size_t get_ramdisk_size();
extern void ramdisk_read(void *, off_t, size_t);
extern int fs_open(const char* path, int flags, int mode);
extern size_t fs_filesz(int fd);
extern ssize_t fs_read(int fd, void* buf, size_t len);
extern int fs_close(int fd);
uintptr_t loader(_Protect *as, const char *filename) {
  //将ramdisk中从0开始的所有内容放置在default_entry处的位置
  // size_t size = get_ramdisk_size();
  // ramdisk_read(DEFAULT_ENTRY, 0, size); 
  
  int fd=fs_open(filename,0,0);
  ssize_t fs_size=fs_filesz(fd);
  fs_read(fd,DEFAULT_ENTRY,fs_size);
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
