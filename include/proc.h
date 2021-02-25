#ifndef _PROC_H_
#define _PROC_H_
#include <arch/basic.h>
#include <memory.h>
#include <rbt.h>

#define PROC_NAME_MAX_LEN 32
struct proc_t
{
    uint pid;
    char name[PROC_NAME_MAX_LEN];
    uint rbt_node_sum;
    struct spin_rwlock_t lock;
    struct rbt_node_t pid_rbt;
    struct ptb_t ptb;
    struct list_node_t thread_ls;
    struct um_t um;
};

struct thread_attr_t
{
    uint tid;
    size_t stk_sz;
};

struct proc_t *proc_now(void);

int proc_init(void);

int proc_switch(struct proc_t *proc);

#endif