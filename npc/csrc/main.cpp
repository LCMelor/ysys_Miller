#include "Vcore.h"
#include "verilated.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "verilated_fst_c.h"


uint32_t mem[1024];
bool stop_flag = false;

extern "C" void stop_sim(bool stop)
{
  if (stop)
  {
    stop_flag = true;
  }
}

void mem_init()
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
}

uint32_t pmem_read(uint32_t vaddr)
{
  uint32_t paddr = vaddr - 0x80000000;
  assert(paddr % 4 == 0);
  return mem[paddr / 4];
}

void single_cycle(Vcore *top, VerilatedFstC *tfp, VerilatedContext *context_p)
{
  top->clk = 0;
  top->eval();
  tfp->dump(context_p->time());
  context_p->timeInc(1);

  top->clk = 1;
  top->inst = pmem_read(top->pc);
  top->eval();
  tfp->dump(context_p->time());
  context_p->timeInc(1);
}

void reset(Vcore *top, VerilatedFstC *tfp, VerilatedContext *context_p, int n = 10)
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

  VerilatedFstC *tfp = new VerilatedFstC;
  context_p->traceEverOn(true);
  top->trace(tfp, 99);
  tfp->open("waveform.fst");

  mem_init();
  reset(top, tfp, context_p, 2);

  while (1)
  {
    single_cycle(top, tfp, context_p);
    if(stop_flag)
    {
      break;
    }
  }

  printf("Simulation done\n");
  tfp->close();
  return 0;
}
