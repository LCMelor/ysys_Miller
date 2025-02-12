#include <common.h>

NPC_state npc_state = { .state = NPC_STOP};

int is_exit_status_bad() {
  if(npc_state.state == NPC_ABORT) {
    Log("The program get an error and can not continue to run.");
  }

  int good = (npc_state.state == NPC_END && npc_state.halt_ret == 0) ||
    (npc_state.state == NPC_QUIT);
  return !good;
}