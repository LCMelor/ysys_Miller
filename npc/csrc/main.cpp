#include "Vcore.h"
#include "verilated.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "verilated_vcd_c.h"
#include <utils.h>


uint32_t mem[4096];
bool stop_flag = false;
uint32_t ret = 0;

extern "C" void stop_sim(bool stop, uint32_t ret_value)
{
  if (stop)
  {
    stop_flag = true;
    ret = ret_value;
  }
}

void log_image(const char *filename)
{
  if(filename == NULL)
  {
    printf("No image file\n");
    return;
  }
  FILE *fp = fopen(filename, "rb");
  assert(fp);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  printf("Image %s is opened with size %ld\n", filename, size);

  fseek(fp, 0 , SEEK_SET);
  int ret = fread(mem, size, 1, fp);
  assert(ret == 1);

  fclose(fp);
}

void mem_init(const char *filename)
{
  mem[0] = 0x00100093; // addi x1, x0, 1
  mem[1] = 0x00200093; // addi x1, x0, 2
  mem[2] = 0x00300093; // addi x1, x0, 3
  mem[3] = 0x00400093; // addi x1, x0, 4
  mem[4] = 0x00208113; // addi x2, x1, 2
  mem[5] = 0x00100073; // ebreak

  for (int i = 6; i < 1024; i++)
  {
    mem[i] = 0x00000013; // nop
  }

  log_image(filename);
}

uint32_t pmem_read(uint32_t vaddr)
{
  uint32_t paddr = vaddr - 0x80000000;
  if(paddr % 4 != 0)
  {
    printf("Invalid memory access : %x\n", vaddr);
    exit(1);
  }
  return mem[paddr / 4];
}

void single_cycle(Vcore *top, VerilatedVcdC *tfp, VerilatedContext *context_p)
{
  top->clk = 0;
  top->eval();
  tfp->dump(context_p->time());
  context_p->timeInc(1);

  top->clk = 1;
  int pc = top->fetch_PC;
  top->eval();
  top->inst = pmem_read(pc);
  top->eval();
  tfp->dump(context_p->time());
  context_p->timeInc(1);
}

void reset(Vcore *top, VerilatedVcdC *tfp, VerilatedContext *context_p, int n = 10)
{
  while(n--)
  {
    top->rst = 1;

    top->clk = 0;
    top->eval();
    tfp->dump(context_p->time());
    context_p->timeInc(1);

    top->clk = 1;
    top->eval();
    tfp->dump(context_p->time());
    context_p->timeInc(1);

    top->rst = 0;
  }
}

int main(int argc, char **argv)
{
  VerilatedContext *context_p = new VerilatedContext;
  context_p->commandArgs(argc, argv);

  Vcore *top = new Vcore{context_p};

  // initialize trace dump
  VerilatedVcdC *tfp = new VerilatedVcdC;
  context_p->traceEverOn(true);
  top->trace(tfp, 99);
  tfp->open("waveform.vcd");

  // initialize memory and load image
  mem_init(argv[1]);

  // reset
  reset(top, tfp, context_p, 2);

  // run simulation
  while (1)
  {
    single_cycle(top, tfp, context_p);
    if(stop_flag)
    {
      break;
    }
  }

  Log("npc: %s\n", (ret == 0 ? ANSI_FMT("HIT GOOD TRAP!", ANSI_FG_GREEN) : 
                    ANSI_FMT("HIT BAD TRAP!", ANSI_FG_RED)));
  tfp->close();
  return 0;
}
