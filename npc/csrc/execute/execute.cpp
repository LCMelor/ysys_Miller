#include <execute.h>
#include <mem.h>
#include <cpu.h>
#include <trace.h>

extern VerilatedContext context_p;
extern Vcore top;
extern VerilatedVcdC *tfp;
extern bool stop_flag;

#define MAX_PRINT 10

cpu_state cpu;
char log_buf[128];

static int g_npc_n_inst = 0;
static bool g_print_step = false;

void difftest_step(uint32_t pc);
void get_reg();

static void single_cycle()
{
  top.clk = 0;
  top.eval();
  tfp->dump(context_p.time());
  context_p.timeInc(1);

  top.clk = 1;
  int pc = top.fetch_PC;
  top.eval();
  top.inst = pmem_read(pc);
  top.eval();
  tfp->dump(context_p.time());
  context_p.timeInc(1);

  // update status of cpu in sim
  cpu.pc = pc;
  cpu.inst = top.inst;
  get_reg();
  // void reg_display();
  // reg_display();
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

#ifdef CONFIG_FTRACE
  // ftrace
  set_sv_scope();
  uint32_t pc = cpu.pc;
  uint32_t tar_pc = jump_target();
  uint32_t jump = jump_flag();
  if(jump) {
    ftrace_write(pc, tar_pc);
  }
#endif

#ifdef CONFIG_DIFFTEST
if(g_npc_n_inst > 1)
  difftest_step(cpu.pc);
#endif
}

static void exe_once()
{
  single_cycle();
  g_npc_n_inst++;
  trace_and_difftest();
}

static void npc_exe(uint32_t n)
{
  while (n--)
  {
    exe_once();
    if(g_print_step) {
      printf("%s\n", log_buf);
    }

    if(npc_state.state == NPC_ABORT) {
      break;
    }

    if (stop_flag){
      npc_state.halt_pc = cpu.pc;
      if(npc_state.halt_ret != 0) {
        npc_state.state = NPC_ABORT;
      } else {
        npc_state.state = NPC_END;
      }
      break;
    }
  }
}

void execute(uint32_t n)
{
  g_print_step = (n < MAX_PRINT);
  switch(npc_state.state) {
    case NPC_END: case NPC_ABORT: case NPC_QUIT:
    printf("Program has ended.\n");
    return;
    default: npc_state.state = NPC_RUN;
  }

  npc_exe(n);
  
  switch(npc_state.state) {
    case NPC_RUN: npc_state.state = NPC_STOP; break;
    case NPC_END: case NPC_ABORT:
     Log("npc: %s at pc = " FMT_WORD,
          (npc_state.state == NPC_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (npc_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          npc_state.halt_pc);
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