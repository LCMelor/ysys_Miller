#ifndef MEMORY_H
#define MEMORY_H

#include <mem.h>
#include <trace.h>
#include <time.h>

#define RTC_MMIO 0xa0000048
#define SERIAL_MMIO 0xa00003f8

uint64_t get_time();
uint64_t us = 0;
static uint8_t *pmem = NULL;


uint8_t* guest_to_host(uint32_t paddr)
{
  return pmem + paddr - PMEM_MBASE;
}

static inline bool in_mem(uint32_t addr) {
  return addr - PMEM_MBASE < PMEM_SIZE;
}

void init_mem()
{
  pmem = (uint8_t*)malloc(PMEM_SIZE);
  assert(pmem);
  Log("Memory initialized with [" FMT_PADDR ", " FMT_PADDR "]", PMEM_MBASE, PMEM_MBASE + PMEM_SIZE - 1);

  uint32_t *p = (uint32_t*)pmem;
  p[0] = 0x00100093; // addi x1, x0, 1
  p[1] = 0x00200093; // addi x1, x0, 2
  p[2] = 0x00300093; // addi x1, x0, 3
  p[3] = 0x00400093; // addi x1, x0, 4
  p[4] = 0x00208113; // addi x2, x1, 2
  p[5] = 0x00100073; // ebreak
}
uint32_t pmem_read(uint32_t paddr)
{
  if(paddr % 4 != 0)
  {
    printf("Invalid memory access : %x\n", paddr);
    exit(1);
  }

  #ifdef CONFIG_MTRACE
  mtrace_read(paddr, 4, *(uint32_t*)(pmem + paddr - PMEM_MBASE));
  #endif

  return *(uint32_t*)(pmem + paddr - PMEM_MBASE);
}

uint32_t pmem_read_sim(int raddr)
{
  if(raddr == RTC_MMIO || raddr == RTC_MMIO + 4) {
    if(raddr == RTC_MMIO) {
      us = get_time();
      return (uint32_t)us;
    }
    else {
      return us >> 32;
    }
  }

  int raddr_t = raddr & ~0x3u;
  if(in_mem(raddr)) {
    #ifdef CONFIG_MTRACE
    mtrace_read(raddr_t, 4, *(uint32_t*)guest_to_host(raddr_t));
    #endif
    return *(uint32_t*)guest_to_host(raddr_t);
  }
  else {
    printf("Invalid memory access in read_sim : %x\n", raddr);
    return 0;
  }
}

void pmem_write(int waddr, int wdata, char wmask)
{
  uint8_t *p_mem = guest_to_host(waddr);

  if(waddr == SERIAL_MMIO && wmask == 0x1) {
    uint8_t *ch = (uint8_t*)&wdata;
    putchar(*ch);
    return;
  }

  if(in_mem(waddr)) {
    #ifdef CONFIG_MTRACE
    mtrace_write(waddr, 4, wdata);
    #endif

    switch(wmask) {
      case 0x1: *(uint8_t*)p_mem = wdata; break;
      case 0x3: *(uint16_t*)p_mem = wdata; break;
      case 0xf: *(uint32_t*)p_mem = wdata; break;
      default: 
        printf("Wmask %x is not correct value, please check\n", wmask);
        assert(0);
    }

    // printf("waddr " FMT_PADDR " write " FMT_WORD " and wmask is %x""\n", waddr, *(uint32_t*)p_wdata, wmask);
    // printf("p_mem is " FMT_WORD " and p_wdata is " FMT_WORD "\n", *(uint16_t*)p_mem, *(uint16_t*)p_wdata);
    // printf("j_space is %d\n", j_space);
    // printf("p_mem is " FMT_PADDR " and p_mem + 2 is " FMT_PADDR "\n",guest_to_host(waddr_f), p_mem);
    // printf("Write factly is " FMT_WORD "\n", *(uint32_t*)p_mem);

  } else {
    printf("Invalid memory access in pmem_write : %x\n", waddr);
    return;
  }
}
#endif
