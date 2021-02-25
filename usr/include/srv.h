#ifndef _USR_SRV_H_
#define _USR_SRV_H_
#include <type.h>

#define MAX_CALL_PARAM 10

struct srv_called_t
{
    uint sid;
    uint arg_sz;
    uint sz[MAX_CALL_PARAM];
    addr_t cache;
};

int srv_call(char *name, addr_t *cache, ...);

int srv_called(char *name, struct srv_called_t *srvcalled);

int srv_reply(uint sid, addr_t addr, size_t size);

int srv_register(char *name, uint nparam);

#endif