#ifndef COMMON_H
#define COMMON_H

// state
typedef struct {
    int state;
    int halt_pc;
    int halt_ret;
} NPC_state;
enum {NPC_STOP, NPC_RUN, NPC_END, NPC_ABORT, NPC_QUIT};

extern NPC_state npc_state;

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <utils.h>

#include "Vcore.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include <config.h>

void set_sv_scope();

#endif