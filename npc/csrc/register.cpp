#include <common.h>
#include <cpu.h>
#include "Vcore__Dpi.h"
#include <iostream>

extern cpu_state cpu;

const char *regs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
  };

void set_sv_scope() {
    const svScope scope = svGetScopeFromName("TOP.core");
    if (!scope) {
        std::cerr << "Error: Unable to find SV scope!\n";
        exit(1);
    }
    svSetScope(scope);
}

// update the register record in cpu
void get_reg()
{
    set_sv_scope();
    for(int i = 0; i < 32; i++)
    {
        cpu.rf[i] = get_sv_reg(i);
    }
}

void reg_display()
{
    for (int i = 0; i < 32; i++)
    {
        printf("%s: " FMT_WORD "\n", regs[i], cpu.rf[i]);
    }
}