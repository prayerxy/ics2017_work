#include "nemu.h"
#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  /* TODO: Add more token types */
  TK_DEX,TK_HEX,TK_REG,
  TK_EQ,TK_NEQ,TK_AND,TK_OR,TK_NOT,TK_GETVAL,TK_NEGATIVE
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\*", '*'},          // multi/getval
  {"\\/", '/'},          // div
  {"\\+", '+'},          // plus
  {"\\-", '-'},          // minus
  {"!",TK_NOT},
  {"==", TK_EQ},         // equal
  {"\\|\\|", TK_OR},        // calc-or
  {"&&", TK_AND},         // calc-and
  {"==", TK_EQ},         // equal
  {"!=", TK_NEQ},        // not-equal
  {"0[xX][1-9A-Fa-f][0-9A-Fa-f]*",TK_HEX},
   {"0|[1-9][0-9]*",TK_DEX},
  {"\\$(eax|ebx|ecx|edx|esp|ebp|esi|edi|eip|ax|bx|cx|dx|sp|bp|si|di|al|bl|cl|dl|ah|bh|ch|dh)",TK_REG},
  {"\\(", '('},       
  {"\\)", ')'},         

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];//按顺序存放已经被识别出的token信息
int nr_token;//指示已经被识别出的token数目

//识别一个表达式是否被一对匹配的括号包围着
bool check_parentheses(int p,int q){
  if(p>=q){
    printf("p>=q is impossible!\n");
    return false;
  }
  if(tokens[p].type!='('||tokens[q].type!=')'){
    return false;
  }
  int count=0;
  for(int i=p;i<=q;i++){
    if(tokens[i].type=='(')count++;
    else if(tokens[i].type==')'){
      if(count>0)count--;
      else return false;//此时count<=0且多一个)
    }
  }
  return count==0;
}

//dominant Operator todo
static int Oprt_priority(int i){
  //优先级由高到低 排除括号
  if(tokens[i].type=='!'||tokens[i].type==TK_GETVAL||tokens[i].type==TK_NEGATIVE){return 1;}
  else if(tokens[i].type=='*'||tokens[i].type=='/'){return 2;}
  else if(tokens[i].type=='+'||tokens[i].type=='-'){return 3;}
  else if(tokens[i].type==TK_EQ||tokens[i].type==TK_NEQ){return 4;}
  else if(tokens[i].type==TK_AND){return 5;}
  else if(tokens[i].type==TK_OR){return 6;}

  //数字或寄存器之类
  return 14;
}
//找到dominant op
static int dominant_OP(int p,int q){
  int i=0;
  int dom_op=0;
  int opp,dom_op_index=0;
  for(i=p;i<=q;i++){
    //非运算符
    if(tokens[i].type==TK_DEX||tokens[i].type==TK_HEX||tokens[i].type==TK_REG)continue;
    //括号里面的不可能是dom_op
    else if(tokens[i].type=='('){
      while(tokens[i].type!=')'&&i<=q){
        i++;
      }
      assert(i<q||tokens[p].type!='(');//不可能有括号包围整个表达式的情况
    }
    else{
      opp=Oprt_priority(i);
      //找到优先级最低的op
      if(opp>=dom_op){//单目表达式  最低的在前面 --3
        if(opp==dom_op&&opp==1){}
        else{
          dom_op=opp;
          dom_op_index=i;
        }
        
      }
    }
  }
  assert(dom_op!=0);
  return dom_op_index;
}
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    //遍历所有规则
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        //log输出匹配成功的信息
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if(substr_len>32){
          //处理溢出
          assert(0);
        }
        switch (rules[i].token_type) {
          case TK_NOTYPE:break;
          case TK_DEX:
            memcpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len]='\0';
            tokens[nr_token].type = rules[i].token_type;
            //printf("expr_cur:%s\n",tokens[nr_token].str);
            nr_token++;
            break;
          case TK_HEX:
            memcpy(tokens[nr_token].str,substr_start+2,substr_len-2);
            tokens[nr_token].str[substr_len-2]='\0';
            tokens[nr_token].type = rules[i].token_type;
            //printf("expr_cur:%s\n",tokens[nr_token].str);
            nr_token++;
            break;
          case TK_REG:
            memcpy(tokens[nr_token].str,substr_start+1,substr_len-1);
            tokens[nr_token].str[substr_len-1]='\0';
            tokens[nr_token].type = rules[i].token_type;
            //printf("expr_cur:%s\n",tokens[nr_token].str);
            nr_token++;
            break;
          default: 
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
            break;
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
    
  }
  if(tokens[0].type=='-')
    tokens[0].type=TK_NEGATIVE;
  if(tokens[0].type=='*')
    tokens[0].type=TK_GETVAL;
  for(int i=1;i<nr_token;i++){
    if(tokens[i].type=='*'){
      if(tokens[i-1].type!=')'&&Oprt_priority(i-1)<14)
        tokens[i].type=TK_GETVAL;//解引用
    }
    if(tokens[i].type=='-'){
      if(tokens[i-1].type!=')'&&Oprt_priority(i-1)<14)
        tokens[i].type=TK_NEGATIVE;
    }
  }
  return true;
}

//eval函数
static uint32_t eval(int p,int q){
  if (p > q) {
    printf("p>q in eval()!\n");
    assert(0);
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    int number=0;
    switch (tokens[p].type)
    {
      case TK_DEX:
        sscanf(tokens[p].str,"%d",&number);
        return number;
        break;
      case TK_HEX:
        sscanf(tokens[p].str,"%x",&number);
        return number;
        break;
      case TK_REG:
        for(int i=0;i<8;i++){
          if(strcmp(tokens[p].str,regsl[i])==0)
            return reg_l(i);
          else if(strcmp(tokens[p].str,regsw[i])==0)
            return reg_w(i);
          else if(strcmp(tokens[p].str,regsb[i])==0)
            return reg_b(i);
        }
        if(strcmp(tokens[p].str,"eip")==0)
          return cpu.eip;
        else
        {
          printf("check register's name in eval!\n");
          assert(0);
        }
      default:
        printf("error in p=q of eval()!\n");
        assert(0);
        break;
    }

  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else {
    //op = the position of dominant operator in the token expression;
    int op=dominant_OP(p,q);
    //printf("op:%d\n",op);
    uint32_t val2 = eval(op + 1, q);
    //单目表达式
    if(tokens[op].type==TK_NEGATIVE){
      //printf("%d\n",val2);
      return -val2;
    }
    else if(tokens[op].type==TK_GETVAL){
      uint32_t res=vaddr_read(val2,4);
      printf(" get the value=%d(0x%08x) of address=0x%x\n",res,res,val2);
      return vaddr_read(val2,4);
    }
    else if(tokens[op].type=='!'){
      if(val2!=0)return 0;
      else return 1;
    }
    //双目表达式
    uint32_t val1=eval(p,op-1);
    switch (tokens[op].type) {
      case '+': return val1+val2;
      case '-': 
        if(val1>=val2)
          return val1-val2;
      case '*': return val1*val2;
      case '/':
      if(val2==0){
          printf("divide 0 error in eval()!\n");
          assert(0);
        } 
      return val1/val2;
      case TK_EQ:{
        return val1==val2;
      }
      case TK_NEQ:{
        return val1!=val2;
      }
      case TK_AND:{
        return val1&&val2;
      }
      case TK_OR:{
        return val1||val2;
      }
      default: assert(0);
    }
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  uint32_t val=eval(0,nr_token-1);
  //执行完毕，说明正确
  *success=true;
  return val;
}
