#ifndef _SRV_H_
#define _SRV_H_
#include <arch/basic.h>
#include <msg.h>
#include <proc.h>
#include <rbt.h>

#define MAX_CALL_PARAM 10
#define MAX_SRV_NAME_LEN 32

/**
 * FIXME: remove const
 */

struct srv_t
{
    char name[MAX_SRV_NAME_LEN];
    struct proc_t *owner;
    uint boxid;
    struct rbt_node_t rbt_node;
    uint nparam;
};

struct srv_call_t
{
    uint retboxid;
    uint arg_sz;
    uint sz[MAX_CALL_PARAM];
};

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

struct srv_reply_t
{
    int err;
    uint narg;
    uint sz[MAX_CALL_PARAM];
};

struct srv_replyee_t
{
    int err;
    uint narg;
    uint sz[MAX_CALL_PARAM];
    addr_t cache;
};

int srv_init(void);

int srv_kern_call(char *name, struct srv_replyee_t *replyee, ...);

#endif