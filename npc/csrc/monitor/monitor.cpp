#include <common.h>
#include <mem.h>
#include <excute.h>

extern VerilatedContext context_p;
extern Vcore top;
extern VerilatedVcdC *tfp;

void log_image(const char *filename)
{
  if(filename == NULL)
  {
    printf("No image file\n");
    return;
  }
  FILE *fp = fopen(filename, "rb");
  assert(fp);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  Log("Image %s is opened with size %ld", filename, size);

  fseek(fp, 0 , SEEK_SET);
  int ret = fread(gutest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
}

void init_trace()
{
  context_p.traceEverOn(true);
  top.trace(tfp, 99);
  tfp->open("build/core/core_wave.vcd");
}


void init_monitor(int argc, char **argv)
{
    // Initialize memory
    init_mem();

    // load image
    if (argc > 1)
    {
        log_image(argv[1]);
    }

    // initialize the trace dump
    init_trace();

    // reset the core
    reset(2);
}