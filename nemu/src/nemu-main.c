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

#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();
// expression test
word_t expr(char *e, bool *success);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

/* -----------------expression test start ---------------------*/
/* bool success;
  word_t exp_calcu_res;
  char exp_line[640];
  char *exp;
  word_t exp_right_res;

  FILE *input = fopen("tools/gen-expr/input", "r");

  int i = 1;
  while((fgets(exp_line, 640, input)) != NULL)
  {
    exp_right_res= strtol(strtok(exp_line, " "), NULL, 0);
    // fgets() read the '\n' at the end of line
    exp = strtok(NULL, "\n");
    exp_calcu_res = expr(exp, &success);

    if(!success) {
      Log("Expression %d error", i);
      assert(0);
    }
    else if (exp_calcu_res == exp_right_res) {
      Log("Expression %d test PASS!", i);
    }
    else {
      Log("Test %d fail. Exp:%s", i, exp);
      assert(0);
    }

    i++;
  }

  fclose(input);
*/
/* -----------------expression test end ---------------------*/
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
