#ifndef EXCUTE_H
#define EXCUTE_H

#include <common.h>
#include "Vcore.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

void single_cycle(Vcore *top, VerilatedVcdC *tfp, VerilatedContext *context_p);
void reset(Vcore *top, VerilatedVcdC *tfp, VerilatedContext *context_p, int n);

#endif