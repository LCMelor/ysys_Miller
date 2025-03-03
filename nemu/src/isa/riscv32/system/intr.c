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
#define csr(idx) (cpu.csr[csr_map(idx)])
extern void etrace_write(vaddr_t mepc, vaddr_t mtvec, vaddr_t mcause);

int csr_map(int csr_num) {
  switch(csr_num) {
    case 0x300: return 0; // mstatus
    case 0x341: return 1; // mepc
    case 0x342: return 2; // mcause
    case 0x305: return 3; // mtvec
    default: Assert(0, "csr num invalid is %x\n", csr_num);
  }
  return -1;
}

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  #ifdef CONFIG_ETRACE
  etrace_write(epc, csr(CSR_MTVEC), NO);
  #endif
  
  csr(CSR_MEPC) = epc;
  csr(CSR_MCAUSE) = NO;
  return csr(CSR_MTVEC);
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
