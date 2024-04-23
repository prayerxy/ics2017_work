#include "common.h"
#include "syscall.h"
#include "arch.h"
static inline void do_sys_none(_RegSet*r){
  SYSCALL_ARG1(r)=1;//返回值置1
}
static inline void do_sys_exit(_RegSet*r){
  _halt(SYSCALL_ARG2(r));
}

static inline void do_sys_write(_RegSet*r){
  int fd = SYSCALL_ARG2(r);
  char* buf = (char*) SYSCALL_ARG3(r);
  int len = SYSCALL_ARG4(r);
  if(fd == 1 || fd == 2){
    Log("fd:%d,len:%d",fd,len);
    for(int i = 0; i < len; i++) {
      _putc(buf[i]);
    }
    //设置返回值
    SYSCALL_ARG1(r)=len;
    return ;
	}
  Log("fd is not 1 or 2");
}

static inline void do_sys_brk(_RegSet*r){
  SYSCALL_ARG1(r) = 0;//表示堆区成功调整
}
_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);
  //系统调用号在eax寄存器中
  switch (a[0]) {
    case SYS_none:
      do_sys_none(r);
      break;
    case SYS_exit:
      //sys_exit中把第一个参数(退出状态)写到ebx寄存器中。 
      do_sys_exit(r);
      break;
    case SYS_write:
      do_sys_write(r);
      break;
    case SYS_brk:
      do_sys_brk(r);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
