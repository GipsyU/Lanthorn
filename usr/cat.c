#include <error.h>
#include <mm.h>
#include <srv.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 1) return E_INVAL;

    struct srv_replyee_t replyee;

    char *path = argv[0];

    int err = E_OK;

    if (path[0] != '/')
    {
        err = malloc((void *)&path, strlen(envp[0]) + strlen(argv[0]) + 1);

        if (err != E_OK) return err;

        memcpy(path, envp[0], strlen(envp[0]));

        memcpy(path + strlen(envp[0]), argv[0], strlen(argv[0]));

        path[strlen(envp[0]) + strlen(argv[0])] = 0;
    }

    err = srv_call("fssrv/read", &replyee, path, strlen(path) + 1);

    if (err != E_OK) return err;

    if (replyee.err != E_OK) return replyee.err;

    char *res = replyee.cache;

    for (uint i = 0; i < replyee.sz[0]; ++i)
        printf("%c", res[i]);

    return E_OK;
}