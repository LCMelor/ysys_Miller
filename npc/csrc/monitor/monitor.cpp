#include <common.h>
#include <mem.h>
#include <excute.h>


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

void init_trace(Vcore *top, VerilatedVcdC *tfp, VerilatedContext *context_p)
{
  context_p->traceEverOn(true);
  top->trace(tfp, 99);
  tfp->open("waveform.vcd");
}


void init_monitor(int argc, char **argv, Vcore *top, VerilatedVcdC *tfp, VerilatedContext *context_p)
{
    // Initialize memory
    init_mem();

    // load image
    if (argc > 1)
    {
        log_image(argv[1]);
    }

    // initialize the trace dump
    init_trace(top, tfp, context_p);

    // reset the core
    reset(top, tfp, context_p, 1);
}