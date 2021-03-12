#include <error.h>
#include <mm.h>
#include <srv.h>
#include <string.h>
#include <type.h>

int cmd_cd(char *parse[], char *pwd)
{
    if (!(parse[1] != NULL && parse[2] == NULL)) return E_INVAL;

    char *name = parse[1];

    if (name[strlen(name)] != '/')
    {
        int err = malloc((void *)&name, strlen(parse[1]) + 2);

        if (err != E_OK) return err;

        memcpy(name, parse[1], strlen(parse[1]));

        name[strlen(parse[1])] = '/';

        name[strlen(parse[1]) + 1] = 0;
    }

    char *s = NULL;

    int err = malloc((void *)&s, strlen(pwd) + strlen(name) + 1);

    if (err != E_OK) return err;

    memcpy(s, pwd, strlen(pwd));

    memcpy(s + strlen(pwd), name, strlen(name));

    s[strlen(pwd) + strlen(name)] = 0;

    struct srv_replyee_t replyee;

    err = srv_call("fssrv/find", &replyee, s, strlen(s) + 1);

    if (err != E_OK) return err;

    if (replyee.err != E_OK) return replyee.err;

    memcpy(pwd + strlen(pwd), name, strlen(name) + 1);

    return err;
}