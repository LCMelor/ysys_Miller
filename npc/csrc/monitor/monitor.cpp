#include <mem.h>
#include <excute.h>
#include <getopt.h>
#include <trace.h>

extern VerilatedContext context_p;
extern Vcore top;
extern VerilatedVcdC *tfp;

static const char *img_file = NULL;
static char *log_file = NULL;

void init_disasm();

FILE *log_fp = NULL;

static long log_image(const char *filename)
{
  if(filename == NULL)
  {
    printf("No image file, use the default img\n");
    return 48;
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
  return size;
}

void init_trace()
{
  context_p.traceEverOn(true);
  top.trace(tfp, 99);
  tfp->open("build/core/core_wave.vcd");
}

void init_log(const char *filename)
{
  log_fp = stdout;
  if(filename)
  {
    FILE *fp = fopen(filename, "w");
    assert(fp);
    log_fp = fp;
  }
  Log("Log file %s is opened", filename ? filename : "stdout");
}

static int parse_args(int argc, char **argv)
{
  const struct option table[] = {
    {"log", required_argument, NULL, 'l'},
    {"pelf", required_argument, NULL, 'e'},
    {NULL, 0, NULL, 0}
  };

  int o;
  while((o = getopt_long(argc, argv, "-l:e:", table, NULL)) != -1) {
    switch(o)
    {
      case 'l': log_file = optarg; break;
      case 'e': parse_elf(optarg); break;
      case 1: img_file = optarg; break;
      default:
      printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        exit(0);
    }
  }
  return 0;
}

void init_monitor(int argc, char **argv)
{
    // Initialize memory
    init_mem();

    // parse the arguments
    parse_args(argc, argv);

    // log the image
    long img_size = log_image(img_file);

    // initialize the log file
    init_log(log_file);

    // initialize the trace dump
    init_trace();

    //initialize the ftrace
    init_ftrace();

    // initialize the disasm
    init_disasm();

    // reset the core
    reset(2);
}