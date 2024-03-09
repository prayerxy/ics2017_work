#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32
//使用链表组织监视点的信息
static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
//从free_链表中返回一个空闲的监视点结构
WP*new_wp(){
  
}
//将wp归还至free_链表中
void free_wp(WP*wp){

}

void print_WPinfo(){
  WP* tmp = head;
	if(!tmp){
		printf("no watchpoint is in use..!\n");
		return;
	}
	while(tmp){
		printf("%d   ,%s  ,%d  \n", tmp->NO, tmp->expr, tmp->value);
		tmp = tmp -> next;
	}
}
