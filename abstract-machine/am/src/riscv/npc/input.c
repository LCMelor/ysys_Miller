#include <am.h>

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  int key = inl(AM_KEYBD_ADDR);
  kbd->keydown = 0;
  kbd->keycode = AM_KEY_NONE;
}
