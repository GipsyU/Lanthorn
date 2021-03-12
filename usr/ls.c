#include <error.h>
#include <srv.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 0) return E_INVAL;

    struct srv_replyee_t replyee;

    int err = srv_call("fssrv/subfile", &replyee, envp[0], strlen(envp[0]) + 1);

    if (err != E_OK) return err;

    if (replyee.err != E_OK) return err;

    for (size_t offset = 0; offset < replyee.sz[0]; offset += strlen(replyee.cache + offset) + 1)
    {
        printf("%s\n", replyee.cache + offset);
    }

    return E_OK;
}
