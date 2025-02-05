#include <common.h>
#include "Vcore__Dpi.h"
#include <iostream>

uint32_t reg[32];

// extern "C" void get_reg(uint32_t regs[])
// {
//     for (int i = 0; i < 32; i++)
//     {
//         reg[i] = regs[i];
//     }
//     printf("reg[0] = %d\n", reg[0]);
// }

void set_sv_scope() {
    const svScope scope = svGetScopeFromName("TOP.core");
    if (!scope) {
        std::cerr << "Error: Unable to find SV scope!\n";
        exit(1);
    }
    svSetScope(scope);
}

void get_reg()
{
    set_sv_scope();
    for(int i = 0; i < 32; i++)
    {
        reg[i] = get_sv_reg(i);
    }
}

void reg_display()
{
    for (int i = 0; i < 32; i++)
    {
        printf("x%02d: " FMT_WORD "\n", i, reg[i]);
    }
}