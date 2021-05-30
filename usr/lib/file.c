#include <file.h>
#include <error.h>
#include <srv.h>
#include <string.h>

int file_create(char *name)
{
    struct srv_replyee_t replyee;

    int err = srv_call("fssrv/create", &replyee, name, strlen(name) + 1);

    if (err != E_OK) return err;

    return replyee.err;
}

int file_delete(char *name)
{
    struct srv_replyee_t replyee;

    int err = srv_call("fssrv/delete", &replyee, name, strlen(name) + 1);

    if (err != E_OK) return err;

    return replyee.err;
}


int file_write(char *name, addr_t addr, size_t size)
{
    struct srv_replyee_t replyee;

    int err = srv_call("fssrv/write", &replyee, name, strlen(name) + 1, addr, size);

    if (err != E_OK) return err;

    return replyee.err;
}