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

struct srv_called_t
{
    uint sid;
    uint arg_sz;
    uint sz[MAX_CALL_PARAM];
    addr_t cache;
};

int srv_init(void);

#endif