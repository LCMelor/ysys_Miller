#ifndef TRACE_H
#define TRACE_H

#include <common.h>
#include <elf.h>
#include "Vcore__Dpi.h"
#include <iostream>

/* ------------------ itrace -----------------*/

/* ------------------ ftrace -----------------*/
typedef struct 
{
    uint32_t fun_strat;
    uint32_t fun_size;
    char fun_name[128];
}fun_info;

void parse_elf(const char *elf_file);

void init_ftrace();

void ftrace_write(uint32_t pc, uint32_t tar_pc);

#endif