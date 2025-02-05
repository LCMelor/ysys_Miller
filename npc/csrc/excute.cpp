#include <excute.h>
#include <mem.h>

extern VerilatedContext context_p;
extern Vcore top;
extern VerilatedVcdC *tfp;
extern bool stop_flag;

static void single_cycle()
{
  top.clk = 0;
  top.eval();
  tfp->dump(context_p.time());
  context_p.timeInc(1);

  top.clk = 1;
  int pc = top.fetch_PC;
  top.eval();
  top.inst = pmem_read(pc, 4);
  top.eval();
  tfp->dump(context_p.time());
  context_p.timeInc(1);
}

void excute(uint32_t n)
{
  if(stop_flag)
  {
    printf("Simulation ended.To restart the program, please quit and restart\n");
    return;
  }
  while (n--)
  { 
    single_cycle();
    if (stop_flag)
    {
      Log("Simulation stopped");
      context_p.timeInc(1);
      return;
    }
  }
}

void reset(int n)
{
  while (n--)
  {
    top.rst = 1;

    top.clk = 0;
    top.eval();
    tfp->dump(context_p.time());
    context_p.timeInc(1);

    top.clk = 1;
    top.eval();
    tfp->dump(context_p.time());
    context_p.timeInc(1);

    top.rst = 0;
  }
}