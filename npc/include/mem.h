#ifndef MEM_H
#define MEM_H

#define PMEM_MBASE 0x80000000
#define PMEM_SIZE 0x8000000
#define RESET_VECTOR 0x80000000
#include <common.h>

void init_mem();
uint8_t* guest_to_host(uint32_t paddr);
uint32_t pmem_read(uint32_t paddr);

extern "C" {
    int pmem_read_sim(int raddr);
    void pmem_write(int waddr, int wdata, char wmask);
}

#endif 