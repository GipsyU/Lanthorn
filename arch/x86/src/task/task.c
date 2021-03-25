#include <arch/task.h>
#include <error.h>
#include <log.h>
#include <spinlock.h>

extern void context_switch(struct context_t *o, struct context_t *n, void (*func)(addr_t args), addr_t args);
extern addr_t intr_user_init(addr_t ksp, addr_t run, addr_t usp, addr_t ubp, addr_t arga, size_t argsz);

int task_kern_init(struct task_t *task, addr_t saddr, size_t ssize, addr_t exe, uint nargs, ...)
{
    task->saddr = saddr;

    task->ssize = ssize;

    addr_t sp = saddr + ssize;

    long *_sp = (void *)(sp -= sizeof(long) * nargs);

    long *args = (void *)(((addr_t)&nargs) + sizeof(nargs));

    for (uint i = 0; i < nargs; ++i) _sp[i] = args[i];

    _sp = (void *)(sp -= sizeof(exe));

    *_sp = NULL;

    _sp = (void *)(sp -= sizeof(exe));

    *_sp = exe;

    task->context.esp = sp;

    task->context.ebp = saddr + ssize;

    task->ncli = 1;

    return E_OK;
}

int task_user_init(struct task_t *task, addr_t ksa, size_t kss, addr_t usa, size_t uss, addr_t pre, addr_t run,
                   addr_t arga, size_t argsz)
{
    task->saddr = ksa;

    task->ssize = kss;

    task->context.esp = ksa + kss;

    task->context.esp = intr_user_init(task->context.esp, run, usa + uss, usa + uss, arga, argsz);

    long *sp = (void *)(task->context.esp -= sizeof(pre));

    *sp = pre;

    task->context.ebp = ksa + kss;

    task->ncli = 1;

    return E_OK;
}

void task_switch(struct task_t *o, struct task_t *n, void (*func)(addr_t args), addr_t args)
{
    context_switch(&o->context, &n->context, func, args);
}

int task_init(struct task_t *task)
{
    task->ncli = 1;

    return E_OK;
}
