#include <am.h>
#include <npc.h>

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  // int key = inl(KBD_ADDR);
  kbd->keydown = 0;
  kbd->keycode = AM_KEY_NONE;
}
