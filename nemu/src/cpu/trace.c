#include <cpu/trace.h>

void init_iringbuf(iringbuf *ringbuf)
{
    ringbuf->head = 0;
    ringbuf->tail = 0;
}


void iringbuf_print(iringbuf *ringbuf)
{
    int i;
    for (i = ringbuf->head; i != ringbuf->tail; i = (i + 1) % MAX_INST_TO_PRINT)
    {
        printf("%s\n", ringbuf->buf[i]);
    }
}

FILE *mtrace_fp = NULL;

void init_mtrace()
{
    const char *mtrace_file = "/home/miller/ysyx-workbench/nemu/build/mtrace.log";

    mtrace_fp = fopen(mtrace_file, "w");
    Assert(mtrace_fp, "Can not open '%s'", mtrace_file);
    Log("Mtrace log is written to %s", mtrace_file);
}

void mtrace_write(paddr_t addr, int len, word_t data)
{
    Assert(mtrace_fp, "Mtrace log is not initialized");
    fprintf(mtrace_fp, "Write  to " FMT_PADDR " with len %d: 0x%08x\n", addr, len, data);
    fflush(mtrace_fp);
}

void mtrace_read(paddr_t addr, int len, word_t data)
{
    Assert(mtrace_fp, "Mtrace log is not initialized");
    fprintf(mtrace_fp, "Read from " FMT_PADDR " with len %d: 0x%08x\n", addr, len, data);
    fflush(mtrace_fp);
}