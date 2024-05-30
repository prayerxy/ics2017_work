#include "cpu/exec.h"
#include "memory/mmu.h"
//中断
void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  //首先将eflags、cs、eip值压入堆栈
  rtl_push(&cpu.eflags);
  cpu.IF=0; //避免中断嵌套
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);

  //从idtr读取idt首地址 cpu.idtr.base
  //idt中索引,找到门
  //门描述符8字节的结构体
  uint32_t h1=vaddr_read(cpu.idtr.base+NO*8,4)&0x0000ffff;
  uint32_t h2=vaddr_read(cpu.idtr.base+NO*8+4,4)&0xffff0000;
  //合成目标地址
  decoding.jmp_eip=h1|h2;
  decoding.is_jmp=1;
  
}

void dev_raise_intr() {
  //将INTR引脚设置为高电平
  cpu.INTR=true;
}
