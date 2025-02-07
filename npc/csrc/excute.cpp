#include <excute.h>
#include <mem.h>
#include <cpu.h>
#include <trace.h>

extern VerilatedContext context_p;
extern Vcore top;
extern VerilatedVcdC *tfp;
extern bool stop_flag;

cpu_state cpu;

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

  cpu.pc = pc;
  cpu.inst = top.inst;
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
    itrace_write();
    if (stop_flag)
    {
      Log("Simulation stopped");
      return;
    }

    // ftrace
    set_sv_scope();
    uint32_t pc = cpu.pc;
    uint32_t tar_pc = jump_target();
    uint32_t jump = jump_flag();
    if(jump)
    {
      ftrace_write(pc, tar_pc);
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