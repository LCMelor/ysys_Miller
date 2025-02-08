#define PMEM_MBASE 0x80000000
#define PMEM_SIZE 0x8000000
#define RESET_VECTOR 0x80000000
#include <common.h>

void init_mem();
uint8_t* gutest_to_host(uint32_t paddr);
uint32_t pmem_read(uint32_t vaddr, int len);