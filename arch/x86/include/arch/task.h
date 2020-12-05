#ifndef _X86_TASK_H_

#define _X86_TASK_H_

#include <arch/basic.h>

struct task_t
{
    addr_t saddr;
    size_t ssize;
    addr_t sp;
};

int task_init(struct task_t *task, addr_t saddr, size_t ssize, addr_t pre, addr_t run, addr_t suf);

void task_switch(struct task_t *o, struct task_t *n);

int task_user_init(struct task_t *task, addr_t ksa, size_t kss, addr_t usa, size_t uss, addr_t pre, addr_t run);

#endif