#include <trace.h>
#include <cpu.h>

extern cpu_state cpu;

/* ------------------ ITRACE -----------------*/

/* ------------------ MTRACE -----------------*/
FILE *mtrace_fp = NULL;

void init_mtrace()
{
    const char *mtrace_file = "/home/miller/ysyx-workbench/npc/build/core/mtrace.log";

    mtrace_fp = fopen(mtrace_file, "w");
    Assert(mtrace_fp, "Can not open '%s'", mtrace_file);
    Log("Mtrace log is written to %s", mtrace_file);
}

void mtrace_write(uint32_t addr, int len, uint32_t data)
{
    Assert(mtrace_fp, "Mtrace log is not initialized");
    fprintf(mtrace_fp, "Inst PC " FMT_WORD " Write  to " FMT_PADDR " with len %d: 0x%08x\n", cpu.pc, addr, len, data);
    fflush(mtrace_fp);
}

void mtrace_read(uint32_t addr, int len, uint32_t data)
{
    Assert(mtrace_fp, "Mtrace log is not initialized");
    fprintf(mtrace_fp, "Inst PC " FMT_WORD " Read from " FMT_PADDR " with len %d: 0x%08x\n", cpu.pc, addr, len, data);
    fflush(mtrace_fp);
}

/* ------------------ FTRACE -----------------*/
#define MAX_FUN_TRACE 4096

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
    assert(strtab);
    fseek(fp, strtab_off, SEEK_SET);
    ret = fread(strtab, strtab_size, 1, fp);
    assert(ret == 1);

    // traverse the signal table
    fseek(fp, symtab_off, SEEK_SET);
    Elf32_Sym symtab_entry;
    assert(symtab_num < MAX_FUN_TRACE);
    for(int i = 0; i < symtab_num; i++)
    {
        ret = fread(&symtab_entry, sizeof(Elf32_Sym), 1, fp);
        assert(ret == 1);
        if((ELF32_ST_TYPE(symtab_entry.st_info)) == STT_FUNC)
        {
            char *name = &strtab[symtab_entry.st_name];
            fun_trace[ft_num].fun_strat = symtab_entry.st_value;
            fun_trace[ft_num].fun_size = symtab_entry.st_size;

            assert(strlen(name) < 128);
            strcpy(fun_trace[ft_num].fun_name, name);
            ft_num++;
        }
    }

    free(strtab);
    fclose(fp);
}

void init_ftrace()
{
    const char *ftrace_file = "/home/miller/ysyx-workbench/npc/build/core/ftrace.log";

    ftrace_fp = fopen(ftrace_file, "w");
    Assert(ftrace_fp, "Can not open %s", ftrace_file);
    Log("Ftrace log is written to %s", ftrace_file);
}

void ftrace_write(uint32_t pc, uint32_t tar_pc)
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
