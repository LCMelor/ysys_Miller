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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "monitor/sdb.h"
#include <utils.h>
#include <memory/vaddr.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
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
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);
static int cmd_s(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);
static int cmd_p(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "s", "Step the program", cmd_s},
  { "info", "Print program status infomation", cmd_info},
  { "x", "scan the memory", cmd_x},
  { "w", "set watch point", cmd_w},
  { "d", "delete watch point", cmd_d},
  { "p", "evaluate expression", cmd_p}

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

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

static int cmd_s(char *args)
{
  char *arg = strtok(NULL, " ");
  int step_count = 1;

  if(arg != NULL) {
    step_count = strtol(arg, NULL, 10);
  }

  cpu_exec(step_count);

  return 0;
}

static int cmd_info(char *args)
{
  char *arg = strtok(NULL, " ");
  if(strcmp(arg, "r") == 0) {
    isa_reg_display();
  }
  if(strcmp(arg, "w") == 0) {
    print_WP();
  }
  return 0;
}

static int cmd_x(char *args)
{
  /* extract the argument count and expression */
  char *arg_count = strtok(NULL, " ");
  char *arg_exp = strtok(NULL, " ");

  /* convert the string into numerical value */
  int count = strtol(arg_count, NULL, 10);
  vaddr_t addr = strtol(arg_exp, NULL, 0);

  /* if(addr % 4 != 0) {
    printf("The target address is not legal\n");
    return 0;
  } */

  /* print 4 machine word in one line */
  int i = 0;
  word_t content;
  for(i = 0; i < count; i++, addr+=4) {
    if(i % 4 == 0 && i != 0) {
      printf("\n");
    }
    if (i % 4 == 0) {
      printf("0x%x:", addr);
    }

    content = vaddr_read(addr, 4);
    printf("%08x  ", content);
  }
  if(i % 4 != 1 || i == 1) {
    printf("\n");
  }
  return 0;
}

static int cmd_w(char *args)
{
  char *exp = strtok(NULL, "\n");
  int exp_len = strlen(exp);

  WP *wp_ptr = new_wp();
  strncpy(wp_ptr->exp, exp, exp_len);
  wp_ptr->exp[exp_len] = '\0';

  bool success;
  wp_ptr->value = expr(wp_ptr->exp, &success);
  if(success == false) {
    assert(0);
  }

  return 0;
}

static int cmd_d(char *args)
{
  char *id_exp = strtok(NULL, " ");
  int id = strtol(id_exp, NULL, 10);

  free_wp(id);

  return 0;
}

static int cmd_p(char *args)
{
  char *exp = strtok(NULL, "\n");
  bool success;
  unsigned value = expr(exp, &success);
    if(!success) {
      assert(0);
    }
  printf("value is:%u\n", value);

  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
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

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
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

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
