#include "common.h"
extern _RegSet* schedule(_RegSet *prev);
extern _RegSet* do_syscall(_RegSet *r);
static _RegSet* do_event(_Event e, _RegSet* r) {
  switch (e.event) {
    //8号事件
    case _EVENT_SYSCALL:
      return do_syscall(r);
    case _EVENT_TRAP:
      printf("事件:内核自陷!\n");
      return schedule(r);
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  //事件处理函数  隐含一个参数-陷阱栈指针
  _asye_init(do_event);
}
