#include <arch/task.h>
#include <error.h>
struct context_t
{
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    u32 eip;
};

extern void context_switch(struct context_t **o, struct context_t *n);

int task_init(struct task_t *task, addr_t saddr, size_t ssize, addr_t pre, addr_t run, addr_t suf)
{
    task->saddr = saddr;

    task->ssize = ssize;

    task->sp = saddr + ssize;

    addr_t *_sp = (void *)(task->sp -= sizeof(suf));

    *_sp = suf;

    _sp = (void *)(task->sp -= sizeof(run));

    *_sp = run;

    struct context_t *context = (void *)(task->sp -= sizeof(struct context_t));

    context->eip = pre;

    context->ebp = saddr + ssize;

    return E_OK;
}

extern addr_t intr_user_init(addr_t ksp, addr_t run, addr_t usp, addr_t ubp);

int task_user_init(struct task_t *task, addr_t ksa, size_t kss, addr_t usa, size_t uss, addr_t pre, addr_t run)
{
    task->saddr = ksa;

    task->ssize = kss;

    task->sp = ksa + kss;

    task->sp = intr_user_init(task->sp, run, usa + uss, usa + uss);

    struct context_t *context = (void *)(task->sp -= sizeof(struct context_t));

    context->eip = pre;

    context->ebp = ksa + kss;

    return E_OK;
}

void task_switch(struct task_t *o, struct task_t *n)
{
    context_switch((struct context_t **)&o->sp,(struct context_t *) n->sp);
}

