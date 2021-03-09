#include <error.h>
#include <log.h>
#include <proc.h>
#include <srv.h>
#include <stdio.h>
#include <string.h>

static int get_cmd(char cmd[], char *args[])
{
    cmd[0] = '\0';

    int err = gets(cmd + 1);

    assert(err == E_OK);

    int len = strlen(cmd + 1);

    int p = 0;

    for (int i = 1; i <= len; ++i)
    {
        if (cmd[i] == ' ') cmd[i] = '\0';

        if (cmd[i - 1] == '\0' && cmd[i] != '\0') args[p++] = cmd + i;
    }

    args[p] = NULL;

    return E_OK;
}

int main(void)
{
    char pwd[] = "/";

    while (1)
    {
        printf(COLOR_GREEN "gipsyh@Lanthorn" COLOR_NONE ":" COLOR_BLUE "%s$ " COLOR_NONE, pwd);

        char cmd[256];

        char *parse[32];

        get_cmd(cmd, parse);

        if (parse[0] == NULL) continue;

        char path[128];

        memcpy(path, "/bin/", strlen("/bin/"));

        memcpy(path + strlen("/bin/"), parse[0], strlen(parse[0]));

        memcpy(path + strlen("/bin/") + strlen(parse[0]), ".elf", strlen(".elf"));

        path[strlen("/bin/") + strlen(parse[0]) + strlen(".elf")] = '\0';

        struct proc_create_attr_t attr;

        strcpy(attr.name, "hello world proc", sizeof("hello world proc"));

        attr.argv = parse + 1;
        
        int err = proc_create(path, &attr);

        printf("%s.\n", strerror(err));
    }
}