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
#include <difftest-def.h>
#include <memory/paddr.h>

extern CPU_state cpu;

__EXPORT void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
  if(direction == DIFFTEST_TO_REF) {
    memcpy(guest_to_host(addr), buf, n);
    return;
  }
  else if(direction == DIFFTEST_TO_DUT) {
    memcpy(buf, guest_to_host(addr), n);
    return;
  }
  assert(0);
}

__EXPORT void difftest_regcpy(void *dut, bool direction) {
  if(direction == DIFFTEST_TO_REF) {
    uint32_t *p = dut;
    for(int i = 0; i < 32; i++) {
      cpu.gpr[i] = p[i];
    }
    return;
  }
  else if(direction == DIFFTEST_TO_DUT) {
    uint32_t *p = dut;
    for(int i = 0; i < 32; i++) {
      p[i] = cpu.gpr[i];
    }
    p[32] = cpu.pc;
    return;
  }
  assert(0);
}

__EXPORT void difftest_exec(uint64_t n) {
  cpu_exec(n);
}

__EXPORT void difftest_raise_intr(word_t NO) {
  assert(0);
}

__EXPORT void difftest_init(int port) {
  void init_mem();
  init_mem();
  /* Perform ISA dependent initialization. */
  init_isa();
}
