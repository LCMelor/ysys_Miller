#include <common.h>
#include <excute.h>
#include <mem.h>
#include <readline/readline.h>
#include <readline/history.h>

static int cmd_help(char *args);
static int cmd_exit(char *args);
static int cmd_continue(char *args);
static int cmd_s(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);

void reg_display();
void get_reg();


#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))
static struct {
    const char *name;
    const char *despription;
    int (*handler) (char *);
} cmd_table[] = {
    {"help", "Print all available commands", cmd_help},
    {"q", "Exit the program", cmd_exit},
    {"c", "Continue the execution until finish", cmd_continue},
    {"s", "Single step execution", cmd_s},
    {"info", "Print the information of the register", cmd_info},
    {"x", "Examine memory", cmd_x}
};

static int cmd_help(char *args)
{
    for(int i = 0; i < NR_CMD; i++)
    {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].despription);
    }
    return 0;
}

static int cmd_exit(char *args)
{
    return -1;
}

static int cmd_s(char *args)
{
    char *arg = strtok(NULL, " ");
    int steps = 1;

    if(arg != NULL)
    {
        steps = atoi(arg);
    }

    excute(steps);
    return 0;
}

static int cmd_continue(char *args)
{
    excute(-1);
    return 0;
}

static int cmd_x(char *args)
{
    char *arg = strtok(NULL, " ");

    uint32_t addr = strtol(arg, NULL, 0);

    uint32_t val = pmem_read(addr, 4);
    printf(FMT_PADDR":" FMT_PADDR "\n", addr, val);

    return 0;
}

static int cmd_info(char *args)
{
    char *arg = strtok(NULL, " ");
    if(strcmp(arg, "r") == 0)
    {
        get_reg();
        reg_display();
    }
    return 0;
}

static char *rl_gets()
{
    static char *line_read = NULL;
    if(line_read)
    {
        free(line_read);
        line_read = NULL;
    }

    line_read = readline("(sdb) ");
    if(line_read && *line_read)
    {
        add_history(line_read);
    }
    return line_read;
}

void sdb_mainloop()
{
    for(char *cmd; (cmd = rl_gets()) != NULL;)
    {
        char *args = strtok(cmd, " ");
        if(args == NULL)
        {
            continue;
        }

        int i = 0;
        for(i = 0; i < NR_CMD; i++)
        {
            if(strcmp(args, cmd_table[i].name) == 0)
            {
                if(cmd_table[i].handler(args) < 0)
                {
                    return;
                }
                break;
            }
        }

        if(i == NR_CMD)
        {
            printf("Unknown command '%s'\n", args);
        }
    }
}