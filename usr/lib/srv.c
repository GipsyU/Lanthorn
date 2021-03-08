#include <error.h>
#include <mm.h>
#include <srv.h>
#include <syscall.h>
#include <utils.h>

int srv_call(char *name, struct srv_replyee_t *replyee, ...)
{
    addr_t param = (addr_t)&replyee + sizeof(replyee);

    return syscall(SYS_srv_call, name, param, replyee);
}

int srv_called(char *name, struct srv_callee_t *srvcalled)
{
    return syscall(SYS_srv_called, name, srvcalled);
}

/**
 * @...: addr and size
 */

int srv_reply(uint sid, int err, uint narg, ...)
{
    struct srv_replyer_t *replyer;

    int _err = malloc((void *)&replyer, sizeof(struct srv_replyer_t));

    if (_err != E_OK) return _err;

    replyer->narg = narg;

    replyer->err = err;

    replyer->sid = sid;

    addr_t *arg = (addr_t)&narg + sizeof(narg);

    for (uint i = 0; i < narg; ++i)
    {
        replyer->ad[i] = arg[i * 2];

        replyer->sz[i] = arg[i * 2 + 1];
    }

    return syscall(SYS_srv_reply, replyer);
}

int srv_register(char *name, uint nparam)
{
    return syscall(SYS_srv_register, name, nparam);
}

int srv_unregister(char *name)
{
    return syscall(SYS_srv_unregister);
}