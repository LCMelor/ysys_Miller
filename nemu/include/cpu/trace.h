#ifndef TRACE_H
#define TRACE_H

#include <common.h>
#include <elf.h>
#define MAX_INST_TO_PRINT 10

/* -----------itrace----------- */
typedef struct
{
    int head;
    int tail;
    char buf[MAX_INST_TO_PRINT][128];
} iringbuf;

// initialize the ring buffer
void init_iringbuf(iringbuf *ringbuf);

// print the content of the ring buffer
void iringbuf_print(iringbuf *ringbuf);

/* -----------mtrace----------- */
void init_mtrace();

void mtrace_write(paddr_t addr, int len, word_t data);

void mtrace_read(paddr_t addr, int len, word_t data);

/* -----------ftrace----------- */



// parse the elf file
void parse_elf(const char *elf_file);

void init_ftrace();

void ftrace_write(vaddr_t pc, vaddr_t tar_pc);

/* -----------dtrace----------- */
void init_dtrace();

void dtrace_write(paddr_t addr, int len, word_t data, const char *str);

void dtrace_read(paddr_t addr, int len, word_t data, const char *str);
#endif
