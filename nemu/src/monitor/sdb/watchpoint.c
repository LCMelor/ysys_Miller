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

#include "monitor/sdb.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}


WP *new_wp()
{
  /* get a new WP from head of free_ and add it to the head of head*/
  if(free_ == NULL) {
    Log("Watch points are exhausted");
    assert(0);
  }
  else if(head == NULL) {
      head = free_;
      free_ = free_->next;
      head->next = NULL;
  }
  else {
    WP *tmp = free_;
    free_ = free_->next;
    tmp->next = head;
    head = tmp;
  }
  return head;
}

/* free a WP and add it to the head of free_ */
void free_wp(int id)
{
  /* if head is empty */
  if(head == NULL) {
    Log("There have no Watch point is uesd");
    return;
  }
  else {
    WP *ptr = head;
    WP *pre_ptr = NULL;
    while(ptr->NO != id && ptr != NULL) {
      pre_ptr = ptr;
      ptr = ptr->next;
    }
    if(ptr == NULL) {
      Log("No watch point with id %d found", id);
      return;
    }
    else {
      /* delete head */
      if(ptr == head) {
        WP *tmp = head;
        head = head->next;
        tmp->next = free_;
        free_ = tmp;
      }
      else {
        pre_ptr->next = ptr->next;
        ptr->next = free_;
        free_ = ptr;
      }
    }
  }
}

WP *get_head()
{
  return head;
}
/* print the under using watch points */
void print_WP()
{
  WP *wp_ptr = head;
  if(head == NULL) {
    printf("There is no used watch point\n");
  }
  else {
    printf("%-8s%s\n", "Num", "What");
    while(wp_ptr != NULL) {
      printf("%-8d%s\n", wp_ptr->NO, wp_ptr->exp);
      wp_ptr = wp_ptr->next;
    }
  }
}

void WP_update()
{
  WP *wp_ptr = get_head();
  while(wp_ptr != NULL) {
    bool success;
    unsigned new_value = expr(wp_ptr->exp, &success);
    if(!success) {
      assert(0);
    }
    if(new_value != wp_ptr->value) {
      unsigned old_value = wp_ptr->value;
      wp_ptr->value = new_value;
      nemu_state.state = NEMU_STOP;
      printf("Trigger watch %s\nnew value:%d\nold value: %d\n",wp_ptr->exp, new_value, old_value);
      break;
    }

    wp_ptr = wp_ptr->next;
  }
}

