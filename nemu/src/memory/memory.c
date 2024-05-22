#include "nemu.h"
#include "device/mmio.h"
#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int map_NO = is_mmio(addr);
  if(map_NO == -1)
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  else
    return mmio_read(addr, len, map_NO);
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int map_NO = is_mmio(addr);
  if(map_NO == -1)
    memcpy(guest_to_host(addr), &data, len);
  else
    mmio_write(addr, len, data, map_NO);
}

#define PDX(va) (((uint32_t)(va) >> 22) & 0x03ff)
#define PTX(va) (((uint32_t)(va) >> 12) & 0x03ff)
#define OFF(va) (((uint32_t)(va) >> 0) & 0x0fff)
#define REMOVE_OFF(x) ((uint32_t)(x) & 0xfffff000)
//分页地址转换   write是区分vaddr_read还是vaddr_write
paddr_t page_translate(vaddr_t vaddr,bool write){
  PDE*pgdir;
  PDE pde;
  PTE*ptable;
  PTE pte;
  paddr_t paddr;
  //只有进入保护模式并开启分页机制之后才会进行页级地址转换
  if(cpu.cr0.paging && cpu.cr0.protect_enable){
    //基地址存在cr3高20位中
    pgdir=(PDE*)REMOVE_OFF(cpu.cr3.val);
    pde.val=paddr_read((paddr_t)&pgdir[PDX(vaddr)],4);
    //检查表项的有效性
    assert(pde.present);
    pde.accessed=1;

    ptable=(PTE*)REMOVE_OFF(pde.val);
    pte=(PTE)paddr_read((paddr_t)&ptable[PTX(vaddr)],4);
    assert(pte.present);
    pte.accessed=1;
    if(write)pte.dirty=1;
    paddr=REMOVE_OFF(pte.val)|OFF(vaddr);
    return paddr;


  }
  else return vaddr;
}

bool ifcrossPage(vaddr_t vaddr,int len){
  return (vaddr&~0xfff)!=((vaddr+len-1)&~0xfff);
}

//访问内存的函数 addr内存首地址 len代表读取内存的字节数
uint32_t vaddr_read(vaddr_t addr, int len) {
  if(ifcrossPage(addr,len)){
    assert(0);
  }
  else{
    paddr_t paddr=page_translate(addr,false);
    return paddr_read(paddr,len);

  }
  
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if(ifcrossPage(addr,len)){
    assert(0);
  }
  else{
    paddr_t paddr=page_translate(addr,true);
    return paddr_write(paddr,len);
  }
}
