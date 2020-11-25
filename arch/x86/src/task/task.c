#include <task.h>

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

    addr_t *_sp = (task->sp -= sizeof(suf));

    *_sp = suf;

    _sp = (task->sp -= sizeof(run));

    *_sp = run;

    struct context_t *context = (task->sp -= sizeof(struct context_t));

    context->eip = pre;

    context->ebp = saddr + ssize;

    return E_OK;
}



void task_switch(struct task_t *o, struct task_t *n)
{
    context_switch((struct context_t **)&o->sp,(struct context_t *) n->sp);
}

