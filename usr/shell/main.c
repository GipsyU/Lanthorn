#include <error.h>
#include <log.h>
#include <mm.h>
#include <proc.h>
#include <srv.h>
#include <stdio.h>
#include <string.h>

extern int cd(char *parse[], char *pwd);

static void get_cmd(char cmd[], char *args[])
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
}

static void get_elf_path(char *parse[], char path[])
{
    memcpy(path, "/bin/", strlen("/bin/"));

    memcpy(path + strlen("/bin/"), parse[0], strlen(parse[0]));

    memcpy(path + strlen("/bin/") + strlen(parse[0]), ".elf", strlen(".elf"));

    path[strlen("/bin/") + strlen(parse[0]) + strlen(".elf")] = '\0';
}

static int inner_cmd(char *parse[], char *pwd)
{
    if (strcmp(parse[0], "cd") == 0)
    {
        int err = cd(parse, pwd);

        if (err != E_OK) printf("cd failed, err = %s.\n", strerror(err));

        return E_OK;
    }

    return E_NOTFOUND;
}

int main(void)
{
    char pwd[128] = "/";

    while (1)
    {
        printf(COLOR_GREEN "gipsyh@Lanthorn" COLOR_NONE ":" COLOR_BLUE "%s" COLOR_NONE "$ ", pwd);

        char cmd[256];

        char *parse[32];

        char path[128];

        get_cmd(cmd, parse);

        get_elf_path(parse, path);

        if (inner_cmd(parse, pwd) == E_OK) continue;

        struct proc_create_attr_t attr;

        strcpy(attr.name, parse[0], strlen(parse[0]) + 1);

        attr.argv = parse + 1;

        char *envp[32];

        envp[0] = pwd;

        envp[1] = 0;

        attr.envp = envp;

        attr.iswait = 1;

        struct proc_create_res_t res;

        int err = proc_create(path, &attr, &res);

        if (err != E_OK) printf("shell exec filed, err = %s.\n", strerror(err));

        if (res.err != E_OK) printf("shell exec error, err = %s.\n", strerror(res.err));
    }
}