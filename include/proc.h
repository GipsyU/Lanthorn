#ifndef _PROC_H_
#define _PROC_H_
#include <arch/basic.h>
#include <rbt.h>
struct proc_t
{
    uint pid;

    char name[CONFIG_SZ_PROC_NAME];

    uint flag;

    uint state;

    addr_t pde;

    struct rbt_node_t pid_rbn;

    struct rbt_t pg_rbt;

    
};

static struct proc_t p0;

int proc_init(addr_t pde);

int proc_fork(void);

int proc_switch(struct proc_t *proc);

int proc_user_init(struct proc_t **proc);

#endif