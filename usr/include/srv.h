#ifndef _USR_SRV_H_
#define _USR_SRV_H_
#include <type.h>

#define MAX_CALL_PARAM 10

struct srv_callee_t
{
    uint sid;
    uint caller;
    uint arg_sz;
    uint sz[MAX_CALL_PARAM];
    addr_t cache;
};
struct srv_replyer_t
{
    int err;
    uint sid;
    uint narg;
    uint sz[MAX_CALL_PARAM];
    uint ad[MAX_CALL_PARAM];
};

struct srv_replyee_t
{
    int err;
    uint narg;
    uint sz[MAX_CALL_PARAM];
    addr_t cache;
};

int srv_call(char *name, struct srv_replyee_t *replyee, ...);

int srv_called(char *name, struct srv_callee_t *srvcalled);

int srv_reply(uint sid, int err, uint narg, ...);

int srv_register(char *name, uint nparam);

#endif