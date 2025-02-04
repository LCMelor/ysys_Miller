#ifndef MEMORY_H
#define MEMORY_H

#include <mem.h>

static uint8_t *pmem = NULL;

uint8_t* gutest_to_host(uint32_t paddr)
{
  return pmem + paddr - PMEM_MBASE;
}

void init_mem()
{
  pmem = (uint8_t*)malloc(PMEM_SIZE);
  assert(pmem);
  Log("Memory initialized with [" FMT_PADDR ", " FMT_PADDR "]", PMEM_MBASE, PMEM_MBASE + PMEM_SIZE - 1);
  // pmem[0] = 0x00100093; // addi x1, x0, 1
  // pmem[1] = 0x00200093; // addi x1, x0, 2
  // pmem[2] = 0x00300093; // addi x1, x0, 3
  // pmem[3] = 0x00400093; // addi x1, x0, 4
  // pmem[4] = 0x00208113; // addi x2, x1, 2
  // pmem[5] = 0x00100073; // ebreak

  // for (int i = 6; i < PMEM_SIZE; i++)
  // {
  //   pmem[i] = 0x00000013; // nop
  // }
}
uint32_t pmem_read(uint32_t paddr, int len)
{
  if(paddr % 4 != 0 && len != 4)
  {
    printf("Invalid memory access : %x\n", paddr);
    exit(1);
  }
  return *(uint32_t*)(pmem + paddr - PMEM_MBASE);
}
#endif