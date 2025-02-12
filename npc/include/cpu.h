#ifndef CPU_H
#define CPU_H

#include <common.h>
typedef struct {
    uint32_t rf[32];
    uint32_t pc;
    uint32_t inst;
    bool jump_flag;
    uint32_t jump_addr;
} cpu_state;

extern cpu_state cpu;

#endif