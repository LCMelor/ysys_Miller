#ifndef TRACE_H
#define TRACE_H

#include <common.h>
#define MAX_INST_TO_PRINT 10

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


void init_mtrace();

void mtrace_write(paddr_t addr, int len, word_t data);

void mtrace_read(paddr_t addr, int len, word_t data);

#endif
