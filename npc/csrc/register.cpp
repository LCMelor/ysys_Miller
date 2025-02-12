#include <common.h>
#include <cpu.h>
#include "Vcore__Dpi.h"
#include <iostream>

extern cpu_state cpu;

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
        printf("x%02d: " FMT_WORD "\n", i, cpu.rf[i]);
    }
}