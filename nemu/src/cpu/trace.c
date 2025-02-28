#include <cpu/trace.h>

/* -------------- ITRACE -------------- */

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

/* -------------- MTRACE -------------- */
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

/* -------------- FTRACE -------------- */

typedef struct 
{
    uint32_t fun_strat;
    uint32_t fun_size;
    char fun_name[128];
}fun_info;

#define MAX_FUN_TRACE 2048

static fun_info fun_trace[MAX_FUN_TRACE];
static int ft_num = 0;
static int call_num = 1;

FILE *ftrace_fp = NULL;

void parse_elf(const char *elf_file)
{
    FILE *fp = fopen(elf_file, "rb");
    Assert(fp, "Can not open '%s'", elf_file);
    fseek(fp, 0, SEEK_SET);

    // get the section header offset、entry size、number of entries and the index of the section header string table
    uint32_t shoff = 0, shentsize = 0, shnum = 0, shstrndx = 0;
    Elf32_Ehdr ehdr;
    int ret = fread(&ehdr, sizeof(ehdr), 1, fp);
    assert(ret == 1);

    shoff = ehdr.e_shoff; // section header offset
    shentsize = ehdr.e_shentsize; // section header entry size
    shnum = ehdr.e_shnum;  // section header entry num
    shstrndx = ehdr.e_shstrndx;  // section header string table index


    // get the section header string table entry
    fseek(fp, shoff + shentsize * shstrndx, SEEK_SET);

    Elf32_Shdr shstrtab_en;
    ret = fread(&shstrtab_en, sizeof(Elf32_Shdr), 1, fp);
    assert(ret == 1);

    // get the section header string table
    fseek(fp, shstrtab_en.sh_offset, SEEK_SET);

    char *shstrtab = (char *)malloc(shstrtab_en.sh_size);
    ret = fread(shstrtab, shstrtab_en.sh_size, 1, fp);
    assert(ret == 1);

    // get the string table and symbol table
    Elf32_Shdr shdr;
    uint32_t symtab_off = 0, symtab_num = 0;
    uint32_t strtab_off = 0, strtab_size = 0;

    fseek(fp, shoff, SEEK_SET);
    for(int i = 0; i < shnum; i++)
    {
        ret = fread(&shdr, sizeof(Elf32_Shdr), 1, fp);
        assert(ret == 1);

        char *name = &shstrtab[shdr.sh_name];
        if(strcmp(name, ".symtab") == 0)
        {
            symtab_off = shdr.sh_offset;
            symtab_num = shdr.sh_size / shdr.sh_entsize;
        }
        else if(strcmp(name, ".strtab") == 0)
        {
            strtab_off = shdr.sh_offset;
            strtab_size = shdr.sh_size;
        }
    }

    free(shstrtab);

    // read the string table
    char *strtab = (char *)malloc(strtab_size);
    fseek(fp, strtab_off, SEEK_SET);
    ret = fread(strtab, strtab_size, 1, fp);
    assert(ret == 1);

    // traverse the signal table
    fseek(fp, symtab_off, SEEK_SET);
    Elf32_Sym symtab_entry;
    for(int i = 0; i < symtab_num; i++)
    {
        ret = fread(&symtab_entry, sizeof(Elf32_Sym), 1, fp);
        assert(ret == 1);
        if((ELF32_ST_TYPE(symtab_entry.st_info)) == STT_FUNC)
        {
            assert(ft_num < MAX_FUN_TRACE);
            char *name = &strtab[symtab_entry.st_name];
            fun_trace[ft_num].fun_strat = symtab_entry.st_value;
            fun_trace[ft_num].fun_size = symtab_entry.st_size;
            strcpy(fun_trace[ft_num].fun_name, name);
            ft_num++;
        }
    }

    free(strtab);
    fclose(fp);
}

void init_ftrace()
{
    const char *ftrace_file = "/home/miller/ysyx-workbench/nemu/build/ftrace.log";

    ftrace_fp = fopen(ftrace_file, "w");
    Assert(ftrace_fp, "Can not open %s", ftrace_file);
    Log("Ftrace log is written to %s", ftrace_file);
}

void ftrace_write(vaddr_t pc, vaddr_t tar_pc)
{
    for(int i = 0; i < ft_num; i++)
    {
        if(tar_pc == fun_trace[i].fun_strat)
        {
            fprintf(ftrace_fp, FMT_PADDR, pc);
            for(int j = 0; j < call_num; j++)
            {
                fprintf(ftrace_fp, " ");
            }
            fprintf(ftrace_fp, "call [%s@" FMT_PADDR "]\n", fun_trace[i].fun_name, fun_trace[i].fun_strat);
            call_num++;
            break;
        }
        else if(tar_pc > fun_trace[i].fun_strat && tar_pc < fun_trace[i].fun_strat + fun_trace[i].fun_size)
        {
            call_num--;
            fprintf(ftrace_fp, FMT_PADDR, pc);
            for(int j = 0; j < call_num; j++)
            {
                fprintf(ftrace_fp, " ");
            }
            fprintf(ftrace_fp, "ret [%s]\n", fun_trace[i].fun_name);
            break;
        }
    }
}

/* -------------- DTRACE -------------- */
FILE *dtrace_fp = NULL;

void init_dtrace()
{
    const char *dtrace_file = "/home/miller/ysyx-workbench/nemu/build/dtrace.log";

    dtrace_fp = fopen(dtrace_file, "w");
    Assert(dtrace_fp, "Can not open %s", dtrace_file);
    Log("Dtrace log is written to %s", dtrace_file);
}

void dtrace_write(paddr_t addr, int len, word_t data, const char *str)
{
    Assert(dtrace_fp, "Dtrace log is not initialized");
    fprintf(dtrace_fp, "%s: Write to " FMT_PADDR " with len %d:" FMT_WORD "\n", str, addr, len, data);
    fflush(dtrace_fp);
}

void dtrace_read(paddr_t addr, int len, word_t data, const char *str)
{
    Assert(dtrace_fp, "Dtrace log is not initialized");
    fprintf(dtrace_fp, "%s: Read from " FMT_PADDR " with len %d:" FMT_WORD "\n", str, addr, len, data);
    fflush(dtrace_fp);
}