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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/wait.h>

#define MAX_BUF 65535
#define MAX_EXP_NUM 200

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
static int buf_ptr;
static int exp_num;

static inline uint32_t choose(uint32_t n)
{
  return rand() % n;
}

static void gen_num()
{
  int str_num[10];
  int num_len = 0;

  // get a rand number
  int num = choose(100);

  // convert number to string
  if(num != 0) {
    for(int tmp = num; tmp > 0; tmp /= 10) {
      int msb = tmp % 10;
      str_num[num_len] = msb + 0x30;
      num_len++;
    }
  // write string number to buf
    for(int i = 0; i < num_len; i++) {
      buf[buf_ptr] = str_num[num_len - 1 - i];
      buf_ptr++;
      assert(buf_ptr < MAX_BUF);
    }
  }
  else {
    buf[buf_ptr] = '0';
    buf_ptr++;
  }
}

static void gen(char c)
{
  if (c == '(')
  {
    buf[buf_ptr] = '(';
    buf_ptr++;
  }
  else if(c == ')') {
    buf[buf_ptr] = ')';
    buf_ptr++;
  }
  assert(buf_ptr < MAX_BUF);
}

static void gen_rand_op()
{
  int op = choose(4);
  if(op == 0) {
    buf[buf_ptr] = '+';
  }
  else if(op == 1) {
    buf[buf_ptr] = '-';
  }
  else if(op == 2) {
    buf[buf_ptr] = '*';
  }
  else if(op == 3) {
    buf[buf_ptr] = '/';
  }
  buf_ptr ++;
  assert(buf_ptr < MAX_BUF);
}

static void gen_void()
{
  int num = choose(2);
  for(int i = 0; i < num; i++)
  {
    buf[buf_ptr] = ' ';
    buf_ptr++;
  }
}

static void gen_rand_expr(int flag) {
  int mod = choose(2);
  while (flag == 4 && mod == 1)
  {
    mod = choose(3);
  }
  if (exp_num > MAX_EXP_NUM)
  {
    mod = 0;
  }
  switch (mod) {
    case 0: gen_num(0); break;
    case 1: gen('('); exp_num++; gen_void(); gen_rand_expr(4); gen_void(); gen(')'); break;
    default: exp_num++; gen_void(); gen_rand_expr(0); gen_rand_op(); exp_num++; gen_void(); gen_rand_expr(0); break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf_ptr = 0;
    exp_num = 0;
    gen_rand_expr(0);
    buf[buf_ptr] = '\0';

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc -Werror /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) {
      i--;
      continue;
    }

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}