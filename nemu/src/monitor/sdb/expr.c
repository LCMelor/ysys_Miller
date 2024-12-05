/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <memory/vaddr.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_INEQ, TK_AND, TK_DENUM, TK_HXNUM, TK_REG, DEREF

  /* TODO: Add more token types */

};

static int op_priority_table[300];

static void init_op_prio_table()
{
  /* give each operator a priority */
  /* priority
    *(dereference) 5
    * /            4
    + -            3
    == !=          2
    &&             1
  */
  int i;
  for(i = 0; i < 300;i ++) {
    op_priority_table[i] = 0;
  }
  op_priority_table[DEREF] = 5;
  op_priority_table['*'] = 4;
  op_priority_table['/'] = 4;
  op_priority_table['+'] = 3;
  op_priority_table['-'] = 3;
  op_priority_table[TK_EQ] = 2;
  op_priority_table[TK_INEQ] = 2;
  op_priority_table[TK_AND] = 1;
}

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-", '-'},           // subtraction
  {"\\*", '*'},         // multiplication
  {"/", '/'},           // division
  {"\\(", '('},         // left bracket
  {"\\)", ')'},         // right bracket
  {"0x[0-9,A-F]+", TK_HXNUM}, // hexadecimal-number
  {"[0-9]+", TK_DENUM}, // decimal-number
  {"==", TK_EQ},        // equal
  {"\\!=", TK_INEQ},    // ineuqal
  {"&&", TK_AND},       // and
  {"\\$(\\$0|[a-z,0-9]+)", TK_REG}, // reg
  {"\\*", DEREF},         // dereference
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

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

static Token tokens[3200] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static int eval(int l, int r);

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);


        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        position += substr_len;

        // skip the notype
        if (rules[i].token_type == TK_NOTYPE)
        {
          break;
        }
        
        tokens[nr_token].type = rules[i].token_type;
        Assert(substr_len <= 32, "The token length is too long");
        strncpy(tokens[nr_token].str, substr_start, substr_len);
        tokens[nr_token].str[substr_len] = '\0';
        nr_token ++;


        // switch (rules[i].token_type) {
        //   default: TODO();
        // }

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

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

    int i;
    for (i = 0; i < nr_token; i ++) {
    if (tokens[i].type == '*' && (i == 0 || 
      tokens[i - 1].type == TK_AND 
    || tokens[i - 1].type == TK_EQ 
    || tokens[i - 1].type == TK_INEQ 
    || tokens[i - 1].type == '+' 
    || tokens[i - 1].type == '-' 
    || tokens[i - 1].type == '*' 
    || tokens[i - 1].type == '/' 
    || tokens[i - 1].type == '(') ) {
      tokens[i].type = DEREF;
    }
  }

  /* TODO: Insert codes to evaluate the expression. */
  *success = true;
  init_op_prio_table();
  return eval(0, nr_token - 1);
}

static bool check_parentheses(int l, int r)
{
  // character stack
   char stack[30];
   int top = 0;

    // if exp is surrounded by a match pair of parenthese
   if(tokens[l].type == '(' && tokens[r].type == ')') {
    // if encounter '(' then push it
    // else if encouonter ')', checking the top whehter is '('
    // if not, get an error
    // if yes, pop it and continue
      for(int i = l + 1; i < r; i++) {
        if(tokens[i].type == '(') {
          top++;
          stack[top] = '(';
        }
        else if(tokens[i].type == ')') {
          // detect ')' before '('
          if(top == 0)
            return false;
          if(stack[top] == '(') {
            top--;
          }
        }
      }
      // if stack is empey, then check successfully
      if(top == 0)
      return true;
      else
      return false;
   }
   else
    return false; 
}

static int find_main_op(int l, int r)
{
  int l_parenthese = 0;
  int main_op = l;
  // the far left and right side must not be operator
  for(int i = l; i < r; i ++) {

    if(tokens[i].type == '(') {
      l_parenthese++;
    }
    else if(tokens[i].type == ')') {
      l_parenthese--;
    }
    
    // out of parenthese and detect operator
    if(l_parenthese == 0 && op_priority_table[tokens[i].type] != 0) {
      /* detect a lower priority operator or first find operator */
      if((op_priority_table[tokens[i].type] > op_priority_table[tokens[main_op].type]) &&
          op_priority_table[tokens[main_op].type] != 0) {
        continue;
      }
      main_op = i;
    }
  }

  return main_op;
}

static int eval(int l, int r)
{
  if(l > r) {
    Log("l:%d, r:%d", l ,r);
    assert(0);
  }
  else if (l == r) {
    if(tokens[l].type == TK_REG) {
      bool success;
      word_t reg_content = isa_reg_str2val(tokens[l].str + 1, &success);
      if(!success) {
        assert(0);
      }
      return reg_content;
    }
    /* convert the str by prefix into integer */
    return strtol(tokens[l].str, NULL, 0);
  }
  else if(check_parentheses(l, r) == true) {
    return eval(l + 1, r - 1);
  }
  else {
    int op = find_main_op(l, r);

    /* *(dereference) is the highest operator. When only it exist, we should use it to get value */
    if(tokens[op].type == DEREF) {
      vaddr_t dere_addr = strtol(tokens[op + 1].str, NULL, 0);
      word_t mem_content = vaddr_read(dere_addr, 4);
      return mem_content;
    }
    else {
      int val1 = eval(l, op - 1);
      int val2 = eval(op + 1, r);

      // var op represent index of main operator in tokens array
      switch(tokens[op].type){
        case '+': return (unsigned)val1 + val2; break;
        case '-': return (unsigned)val1 - val2; break;
        case '*': return (unsigned)val1 * val2; break;
        case '/': return (unsigned)val1 / val2; break;
        case TK_EQ: return val1 == val2; break;
        case TK_INEQ: return val1 != val2; break;
        case TK_AND: return val1 && val2; break;
        default: assert(0);
      }
    }
  }
}