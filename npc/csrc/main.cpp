#include "Vcore.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <common.h>
#include <excute.h>


bool stop_flag = false;
uint32_t ret = 0;

void init_monitor(int argc, char **argv, Vcore *top, VerilatedVcdC *tfp, VerilatedContext *context_p);

extern "C" void stop_sim(bool stop, uint32_t ret_value)
{
  if (stop)
  {
    stop_flag = true;
    ret = ret_value;
  }
}

int main(int argc, char **argv)
{
  VerilatedContext *context_p = new VerilatedContext;
  context_p->commandArgs(argc, argv);

  Vcore *top = new Vcore{context_p};

  VerilatedVcdC *tfp = new VerilatedVcdC;
  /* Initialize the monitor */
  init_monitor(argc, argv, top, tfp, context_p);

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
