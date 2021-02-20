#ifndef _PROC_H_
#define _PROC_H_
#include <arch/basic.h>
#include <memory.h>
#include <rbt.h>
struct proc_t
{
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

int proc_new(struct proc_t **proc);

int proc_switch(struct proc_t *proc);

#endif