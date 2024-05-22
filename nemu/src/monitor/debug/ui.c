#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void print_WPinfo();
WP* new_wp();
void free_wp(int n);
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);//传入-1,是uint64_t中的最大值
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

//add some functions in pa1
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_p(char *args);
static int cmd_x(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "args:[N]; Execute N instructions step by step ",cmd_si},
  { "info","args:r/w; Use info r to print status about registers,use info w to print status about watchpoints",cmd_info},
  { "p", "args:expr; Compute the value of expressions",cmd_p},
  { "x", "args: N expr; scan the N 4bytes memory from address EXPR",cmd_x},
  { "w", "args:expr; set the watchpoint,pause the program when value of expr changes",cmd_w},
  { "d", "args: N; delete watchpoint that index is N",cmd_d}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args){
  //获取第一个参数
  char *arg = strtok(NULL, " ");
  if(arg==NULL){
    cpu_exec(1);
  }
  else{
    int N=atoi(arg);//atoi可以转换负数
    if(N<=0){
      printf("Error in cmd_si!\n");
      return 0;
    }
    cpu_exec(N);
  }
  return 0;
}
static int cmd_info(char *args){
  char *arg = strtok(NULL, " ");
  if(arg==NULL){
    printf("please check your param in cmd_info()!\n");
    return 0;
  }
  else if(strcmp(arg,"r")==0){
    //这里只打印32位的寄存器 
    printf("reg\t16进制\t\t10进制\n");
    for(int i=0;i<8;i++)printf("%s\t0x%08x\t%d\n",regsl[i],reg_l(i),reg_l(i));
    printf("eip\t0x%08x\t%d\n",cpu.eip,cpu.eip);
    printf("CR0=0x%08x，CR3=0x%08x\n",cpu.cr0,cpu.cr3);
  }
  else if(strcmp(arg,"w")==0){
    //打印监视点的信息
    print_WPinfo();
  }

  return 0;
}
static int cmd_p(char *args){
  //表达式求值
  if(args==NULL){
    printf("please enter the expr you want to compute!\n");
    return 0;
  }
  bool success;
  uint32_t res=expr(args,&success);
  if(!success){
    printf("please check your expr!\n");
    return 0;
  }
  printf("the value of expr is: %d(0x%x)\n",res,res);
  return 0;
}
static int cmd_x(char *args){
  //扫描内存
  //x N EXPR
  char *arg1 = strtok(NULL, " ");
  if(arg1==NULL){
    printf("the first param must be N to specify the N 4bytes!\n");
    return 0;
  }
  int N=atoi(arg1);
  char *arg2=strtok(NULL," ");
  if(arg2==NULL){
    printf("the second param must be an EXPR!\n");
    return 0;
  }
  //使用expr函数
  bool success;
  uint32_t addr=expr(arg2,&success);
  uint32_t addr2=addr;
  if(success==false){
    printf("something errors in expr()!\n");
    return 0;
  }
  printf("addr\t\tmemory\n");
  for(int i=0;i<N;i++){
    //打印地址
    printf("0x%x:\t",addr);
    //打印地址对应的值 一次性打印1bytes
    for(int j=0;j<4;j++){
      //2位宽度
      printf("0x%02x ", vaddr_read(addr,1));
      addr++;
    }
    printf("\t");
    //一次性打印4bytes  小段字节序 低位在低地址
    printf("0x%08x",vaddr_read(addr2,4));
    addr2+=4;
    printf("\n");
  }
  return 0;
}
static int cmd_w(char *args){
  char *arg = strtok(NULL, " ");
  if(arg==NULL){printf("please check you expr of watchpoint!\n");assert(0);}
  bool success;
  uint32_t value=expr(arg,&success);
  if(success!=true){
    printf("something error in cmd_w()!\n");
    assert(0);
  }
  WP*wp_cur=new_wp();
  wp_cur->Value=value;//值
  strcpy(wp_cur->expr,arg);//表达式
  printf("set one watchpoints,NO.:%d,expr:%s,value:%d\n",wp_cur->NO,wp_cur->expr,wp_cur->Value);
  return 0;
}
static int cmd_d(char *args){
  char *arg = strtok(NULL, " ");
  if(arg==NULL){printf("please check your args in cmd_d()!\n");return 0;}
  int index=atoi(arg);
  free_wp(index);
  return 0;
}


void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
