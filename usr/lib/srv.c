#include <srv.h>
#include <syscall.h>
#include <utils.h>

int srv_call(char *name, addr_t *cache, ...)
{
    addr_t param = (addr_t)&cache + sizeof(cache);

    return syscall(SYS_srv_call, name, param, cache);
}

int srv_called(char *name, struct srv_called_t *srvcalled)
{
    return syscall(SYS_srv_called, name, srvcalled);
}

int srv_reply(uint sid, addr_t addr, size_t size)
{
    return syscall(SYS_srv_reply, sid, addr, size);
}

int srv_register(char *name, uint nparam)
{
    return syscall(SYS_srv_register, name, nparam);
}

int srv_unregister(char *name)
{
    return syscall(SYS_srv_unregister);
}