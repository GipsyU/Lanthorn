#ifndef _X86_TASK_H_

#define _X86_TASK_H_

#include <arch/basic.h>

struct context_t
{
    u32_t edi;
    u32_t esi;
    u32_t ebx;
    u32_t ebp;
    u32_t esp;
};

struct task_t
{
    addr_t saddr;
    size_t ssize;
    uint ncli;
    struct context_t context;
};

int task_kern_init(struct task_t *task, addr_t saddr, size_t ssize, addr_t exe, uint nargs, ...);

void task_switch(struct task_t *o, struct task_t *n, void (*func)(addr_t args), addr_t args);

int task_user_init(struct task_t *task, addr_t ksa, size_t kss, addr_t usa, size_t uss, addr_t pre, addr_t run,
                   addr_t arga, size_t argsz);

#endif