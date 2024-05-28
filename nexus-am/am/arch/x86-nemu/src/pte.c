#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  //切换不同进程的地址空间
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) {
  //功能:将虚拟地址空间p中的虚拟地址va映射到物理地址pa
  //p->ptr为页目录基地址
  PDE*pde=&(((PDE*)(p->ptr))[PDX(va)]);
  PTE*pgtable=NULL;
  if(*pde&PTE_P){
    pgtable=(PTE*)PTE_ADDR(*pde);
  }
  else{
    pgtable=(PTE*)(palloc_f());
    *pde=PTE_ADDR(pgtable)|PTE_P;
  }
  //二级页表 pte  页表项储存物理索引 以及12位的符号位
  pgtable[PTX(va)]=PTE_ADDR(pa)|PTE_P;
}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {

    //注意入栈的顺序是异常处理的相关顺序
    //8 gprs,eflags, cs, eip, error code, irq，8 gprs
    //一开始的8 gprs是参数

    uint32_t *tmp = ustack.end;
    uint32_t rem[30] = {
      0, 0, 0, 0, 0, 0, 0, 0,            // 前8个GPRs (edi, esi, ebp, esp, ebx, edx, ecx, eax)
      0x202, 0x8, (uint32_t)entry, 0x0,  // eflags, cs, eip, error code
      0x81,                              // irq
      0, 0, 0, 0, 0, 0, 0, 0             // 后8个GPRs (edi, esi, ebp, esp, ebx, edx, ecx, eax)
    };
    for (int i = 0; i < (8 + 5 + 8); i++) {
      *tmp = rem[i];
      tmp--;
    }
    tmp++; // 调整指针，指向陷阱帧起始位置
    return (_RegSet*) tmp;
}
