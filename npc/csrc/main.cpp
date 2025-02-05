#include <common.h>
#include <excute.h>


bool stop_flag = false;
uint32_t ret = 1;

VerilatedContext context_p;
Vcore top;
VerilatedVcdC *tfp = NULL;

void init_monitor(int argc, char **argv);
void sdb_mainloop();

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
  context_p.commandArgs(argc, argv);
  tfp = new VerilatedVcdC;

  /* Initialize the monitor */
  init_monitor(argc, argv);

  // run simulation
  sdb_mainloop();

  Log("NPC: %s\n", (ret == 0 ? ANSI_FMT("HIT GOOD TRAP!", ANSI_FG_GREEN) : 
                    ANSI_FMT("HIT BAD TRAP!", ANSI_FG_RED)));
  tfp->close();
  delete tfp;
  return 0;
}
