#include <error.h>
#include <mm.h>
#include <proc.h>
#include <srv.h>
#include <stdio.h>
#include <string.h>
#include <type.h>

static void get_elf_path(char *parse[], char path[])
{
    memcpy(path, "/bin/", strlen("/bin/"));

    memcpy(path + strlen("/bin/"), parse[0], strlen(parse[0]));

    memcpy(path + strlen("/bin/") + strlen(parse[0]), ".elf", strlen(".elf"));

    path[strlen("/bin/") + strlen(parse[0]) + strlen(".elf")] = '\0';
}

int cmd_back(char *parse[], char *pwd)
{
    char path[128];

    get_elf_path(parse + 1, path);

    struct proc_create_attr_t attr;

    strcpy(attr.name, parse[1], strlen(parse[1]) + 1);

    attr.argv = parse + 2;

    char *envp[32];

    envp[0] = pwd;

    envp[1] = 0;

    attr.envp = envp;

    attr.iswait = 0;

    struct proc_create_res_t res;

    int err = proc_create(path, &attr, &res);

    if (err != E_OK)  return err;

    return res.err;
}