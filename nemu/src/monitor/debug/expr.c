#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  /* TODO: Add more token types */
  TK_DEX,TK_HEX,TK_REG,
  TK_EQ,TK_NEQ,TK_AND,TK_OR,TK_NOT
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
  {"0|[1-9][0-9]*",TK_DEX},
  {"0[xX][1-9A-Fa-f][0-9A-Fa-f]*",TK_HEX},
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
            nr_token++;
            break;
          case TK_HEX:
            memcpy(tokens[nr_token].str,substr_start+2,substr_len-2);
            tokens[nr_token].str[substr_len-2]='\0';
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
            break;
          case TK_REG:
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

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
