#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32
//使用链表组织监视点的信息
static WP wp_pool[NR_WP];
static WP *head, *free_;//head组织使用中的链表结构，free_组织空闲的监视点结构
static int used_node=0;
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
//从free_链表中返回一个空闲的监视点结构 这里没有完成赋值
WP*new_wp(){
  if(free_==NULL)assert(0);
  WP*cur=free_;
  cur->NO=used_node;
  used_node++;
  if(used_node>NR_WP){
    assert(0);
  }
  free_=free_->next;
  WP*tmp=head;
  cur->next=NULL;
  if(head==NULL){
    head=cur;
  }
  else{
    //head指向使用的链表的最头部
    while(tmp->next!=NULL)
      tmp=tmp->next;
    tmp->next=cur;
  }
  return cur;
}
//将wp归还至free_链表中
void free_wp(int n){
  WP*cur=head;
  WP*res=NULL;
  WP*cur_prev=NULL;
  if(cur==NULL){printf("NO watchpoints now!\n");return;}
  else{
    while(cur!=NULL){
      if(cur->NO==n){
        res=cur;
        break;
      }
      cur_prev=cur;
      cur=cur->next;
    }
    if(res){
      if(cur_prev){
        cur_prev->next=cur->next;
      }else {assert(head==res);head=cur->next;printf("head\n");}//删掉的是头节点

      res->next=free_;
      free_=res;//free_指向空闲链表的最末端
    }
    else{
      printf("there is no such watchpoint!\n");
    }
  }
}

void print_WPinfo(){
  WP* tmp = head;
	if(!tmp){
		printf("no watchpoint is in use..!\n");
		return;
	}
	while(tmp){
    printf("No.\tExpr\t\tValue\n");
		printf("%d\t%s\t\t%d\n", tmp->NO, tmp->expr, tmp->Value);
		tmp = tmp -> next;
	}
}

//判断监视点是否触发
bool check_wpvalue(){
  WP*cur=head;
  bool success;
  if(cur==NULL)return false;
  while(cur!=NULL){
    uint32_t res;
    res=expr(cur->expr,&success);
    if(!success){
      printf("computing value of expr errors!\n");
      continue;
    }
    if(res!=cur->Value){
      printf("watchpoint NO.:%d,expr:%s,old_value:%d,now_value:%d\n",cur->NO,cur->expr,cur->Value,res);
      cur->Value=res;
      return true;
    }
    cur=cur->next;
  }
  return false;
}
