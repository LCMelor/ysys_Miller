#include <excute.h>
#include <mem.h>
#include <cpu.h>
#include <trace.h>

extern VerilatedContext context_p;
extern Vcore top;
extern VerilatedVcdC *tfp;
extern bool stop_flag;

cpu_state cpu;
char log_buf[128];

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

static void inst_disasm()
{
  char *p = log_buf;
  p += snprintf(p, sizeof(log_buf), FMT_WORD ":", cpu.pc);

  uint8_t *inst = (uint8_t*)&cpu.inst;
  for(int i = 3; i >=0; i--) {
    p += snprintf(p, 4, " %02x", inst[i]);
  }

  memset(p, ' ', 1);
  p += 1;

  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, log_buf + sizeof(log_buf) - p, cpu.pc, (uint8_t *)&cpu.inst, 4);
}

static void trace_and_difftest()
{
  // itrace
  inst_disasm();
  log_write("%s\n", log_buf);

  // ftrace
  set_sv_scope();
  uint32_t pc = cpu.pc;
  uint32_t tar_pc = jump_target();
  uint32_t jump = jump_flag();
  if(jump) {
    ftrace_write(pc, tar_pc);
  }
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
    trace_and_difftest();
    if (stop_flag){
      Log("Simulation stopped");
      break;
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