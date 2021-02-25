#ifndef _PROC_PROC_H_
#define _PROC_PROC_H_
#include <proc.h>

static struct
{
    struct rbt_t proc_rbt;

    struct spin_rwlock_t rbt_rwlock;

} PROC;

int proc_rbt_insert(struct proc_t *proc, uint is_alloc_pid);

int proc_rbt_find(uint pid, struct proc_t **res);

int proc_rbt_delete(uint pid);

int proc_rbt_init(void);

#endif