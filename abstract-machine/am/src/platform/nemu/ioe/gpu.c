#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {\
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = inw(VGACTL_ADDR + 2), .height = inw(VGACTL_ADDR),
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x, y, w, h;
  uint32_t *pixels = ctl->pixels;
  x = ctl->x;
  y = ctl->y;
  w = ctl->w;
  h = ctl->h;

  for(int i = 0; i < h; i ++) {
    for(int j = 0; j < w; j ++) {
      outl(FB_ADDR + (y * 400 + x) * 4, pixels[i * w + j]);
      x ++;
    }
    x = ctl->x;
    y ++;
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
