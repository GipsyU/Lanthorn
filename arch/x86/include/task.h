#ifndef _X86_TASK_H_

#define _X86_TASK_H_

#include <basic.h>

struct task_t
{
    addr_t saddr;
    size_t ssize;
    addr_t sp;
};

int task_init(struct task_t *task, addr_t saddr, size_t ssize, addr_t pre, addr_t run, addr_t suf);

void task_switch(struct task_t *o, struct task_t *n);


#endif