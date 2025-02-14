#include <dlfcn.h>

#include <mem.h>
#include <utils.h>
#include <difftest.h>
#include <common.h>
#include <cpu.h>

void (*ref_difftest_memcpy)(uint32_t addr, void *buf, size_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;

void reg_display();

#ifdef CONFIG_DIFFTEST

void init_difftest(char *ref_so_file, long img_size, int port) {
  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY);
  assert(handle);

  ref_difftest_memcpy = reinterpret_cast<void (*)(uint32_t addr, void *buf, size_t n, bool direction)>(dlsym(handle, "difftest_memcpy"));
  assert(ref_difftest_memcpy);

  ref_difftest_regcpy = reinterpret_cast<void (*)(void *dut, bool direction)>(dlsym(handle, "difftest_regcpy"));
  assert(ref_difftest_regcpy);

  ref_difftest_exec = reinterpret_cast<void (*)(uint64_t n)>(dlsym(handle, "difftest_exec"));
  assert(ref_difftest_exec);

  void (*ref_difftest_init)(int) = reinterpret_cast<void (*)(int)>(dlsym(handle, "difftest_init"));
  assert(ref_difftest_init);

  Log("Differential testing: %s", ANSI_FMT("ON", ANSI_FG_GREEN));
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in menuconfig.", ref_so_file);

  ref_difftest_init(port);
  ref_difftest_memcpy(RESET_VECTOR, guest_to_host(RESET_VECTOR), img_size, DIFFTEST_TO_REF);
  ref_difftest_regcpy(&cpu.rf, DIFFTEST_TO_REF);
}

static bool difftest_checkregs(cpu_state *ref, uint32_t pc)
{
  for(int i = 1; i < 32; i++) {
    if(ref->rf[i] != cpu.rf[i]) {
      printf("regs are not equal\n");
      printf("ref %d is " FMT_WORD "\ndut %d is " FMT_WORD "\n" , i, ref->rf[i], i, cpu.rf[i]);
      return false;
    }
  }
  if(ref->pc != pc) {
    printf("PC is not equal\n");
    printf("ref pc is " FMT_WORD " npc pc is " FMT_WORD "\n", ref->pc, pc);
    return false;
  }
  
  return true;
}

static void checkregs(cpu_state *ref, uint32_t pc) {
  if (!difftest_checkregs(ref, pc)) {
    npc_state.state = NPC_ABORT;
    npc_state.halt_pc = pc;
    reg_display();
  }
}

void difftest_step(uint32_t pc) {
  cpu_state ref_r;

  ref_difftest_exec(1);
  ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);

  checkregs(&ref_r, pc);
}
#else
void init_difftest(char *ref_so_file, long img_size, int port) { }
#endif
