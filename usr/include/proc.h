#ifndef _USR_PROC_H_
#define _USR_PROC_H_
#include <type.h>
struct thread_attr_t
{
    uint tid;
    size_t stk_sz;
};

int fork(long *pid);

int thread_create(uint *tid, addr_t routine, struct thread_attr_t *attr, addr_t arg);

int thread_exit(void);

int thread_tid(uint *tid);

int thread_block(uint tid);

int thread_wake(uint tid);

#endif