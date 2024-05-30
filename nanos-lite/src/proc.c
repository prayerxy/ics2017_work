#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}
int cur_flag=0;
int cur_game=0;
void changeGame(){
  cur_game=2-cur_game;
}
_RegSet* schedule(_RegSet *prev) {
  if(current!=NULL){
    current->tf=prev;//储存旧进程的Tf
  }
  //切换
  // current=&pcb[0];
  //分时复用
  current=&pcb[cur_game];
  cur_flag++;
  if(cur_flag==1000){cur_flag=0;current=&pcb[1];}
  // current=(current==&pcb[0]?&pcb[1]:&pcb[0]);
  // Log("we are now switching to pcb[0]\n");
  _switch(&current->as);
  return current->tf;
}
