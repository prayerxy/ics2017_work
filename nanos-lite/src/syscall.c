#include "common.h"
#include "syscall.h"

static inline void do_sys_none(_RegSet*r){
  SYSCALL_ARG1(r)=1;//返回值置1
}
static inline void do_sys_exit(_RegSet*r){
  _halt(SYSCALL_ARG2(r));
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  //系统调用号在eax寄存器中

  switch (a[0]) {
    case SYS_none:
      do_sys_none(r);
      break;
    case SYS_exit:
      //sys_exit中把第一个参数(退出状态)写到ebx寄存器中。 
      do_sys_exit(r);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
