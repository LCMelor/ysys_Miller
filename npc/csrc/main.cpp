#include <common.h>


bool stop_flag = false;
uint32_t ret = 1;

VerilatedContext context_p;
Vcore top;
VerilatedVcdC *tfp = NULL;

void init_monitor(int argc, char **argv);
void sdb_mainloop();
int is_exit_status_bad();

extern "C" void stop_sim(bool stop, uint32_t ret_value)
{
  if (stop)
  {
    stop_flag = true;
    npc_state.halt_ret = ret_value;
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

  tfp->close();
  delete tfp;
  return is_exit_status_bad();
}
