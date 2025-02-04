#include <excute.h>
#include <mem.h>

void single_cycle(Vcore *top, VerilatedVcdC *tfp, VerilatedContext *context_p)
{
  top->clk = 0;
  top->eval();
  tfp->dump(context_p->time());
  context_p->timeInc(1);

  top->clk = 1;
  int pc = top->fetch_PC;
  top->eval();
  top->inst = pmem_read(pc, 4);
  top->eval();
  tfp->dump(context_p->time());
  context_p->timeInc(1);
}

void reset(Vcore *top, VerilatedVcdC *tfp, VerilatedContext *context_p, int n)
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