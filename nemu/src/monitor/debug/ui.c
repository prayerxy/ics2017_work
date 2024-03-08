#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void print_WPinfo();
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
    printf("Error in cmd_info!\n");
    return 0;
  }
  else if(strcmp(arg,"r")){
    //这里只打印eax等32位的寄存器
    for(int i=0;i<8;i++)printf("%s  0x%x\n",regsl[i],reg_l(i));
    printf("eip  0x%x\n",cpu.eip);
  }
  else if(strcmp(arg,"w")){
    //打印监视点的信息 todo
    print_WPinfo();
  }

  return 0;
}
static int cmd_p(char *args){
  //todo 表达式求值
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
  //bool success;
  uint32_t addr=strtoul(arg2,NULL,16);;
  // if(success==false){
  //   printf("something errors in expr()!\n");
  //   return 0;
  // }
  for(int i=0;i<N;i++){
    //打印地址
    printf("0x%x:\t",addr);
    //打印地址对应的值 一次性打印4bytes
    for(int j=0;j<4;j++){
      printf("0x%x ", vaddr_read(addr,1));
      addr++;
    }
    printf("\n");
  }


  return 0;
}
static int cmd_w(char *args){
  return 0;
}
static int cmd_d(char *args){
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
