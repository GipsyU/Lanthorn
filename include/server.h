#ifndef _SERVER_H_
#define _SERVER_H_
#include <arch/basic.h>
#include <proc.h>
#include <rbt.h>

/**
 * FIXME: remove const
 */

struct server_t
{
    char name[20];
    uint narg;
    struct proc_t *proc;
    addr_t hdl;
    struct rbt_node_t rbt_node;
    uint arg_ptr_size[10];
};

#endif