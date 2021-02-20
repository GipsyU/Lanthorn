#include <arch/cpu.h>
#include <arch/intr.h>
#include <arch/mmu.h>
#include <error.h>
#include <list.h>
#include <log.h>
#include <memory.h>
#include <proc.h>
#include <spinlock.h>
#include <string.h>
#include <syscall.h>
#include <thread.h>
#include <util.h>

extern struct proc_t proc_0;

#define DFT_STK_SZ (PAGE_SIZE * 1024)

static void pre(void)
{
    info("thread begin\n");
    intr_end();
}

static void thread_enclosure(addr_t func, uint nargs, ...)
{
    info("thread begin.\n");

    intr_end();

    int err = E_OK;

    long *args = (void *)(((addr_t)&nargs) + sizeof(nargs));

    if (nargs == 0)
    {
        int (*handler)(void) = (void *)func;

        err = handler();
    }

    if (nargs == 1)
    {
        int (*handler)(long) = (void *)func;

        err = handler(args[0]);
    }

    if (nargs == 2)
    {
        int (*handler)(long, long) = (void *)func;

        err = handler(args[0], args[1]);
    }

    if (nargs == 3)
    {
        int (*handler)(long, long, long) = (void *)func;

        err = handler(args[0], args[1], args[2]);
    }

    if (nargs == 4)
    {
        int (*handler)(long, long, long, long) = (void *)func;

        err = handler(args[0], args[1], args[2], args[3]);
    }

    if (nargs == 5)
    {
        int (*handler)(long, long, long, long, long) = (void *)func;

        err = handler(args[0], args[1], args[2], args[3], args[4]);
    }

    if (nargs > 5)
    {
        error("thread_enclosure error.\n");
    }

    info("thread end, killed.\n");

    schd_kill(thread_now());

    while (1)
        ;
}

struct thread_t *thread_now(void)
{
    struct task_t *task = cpu_get_task(cpu_id());

    return container_of(task, struct thread_t, task);
}

static int thread_new(struct thread_t **res)
{
    struct thread_t *thread = NULL;

    int err = kmalloc((addr_t *)&thread, sizeof(struct thread_t));

    if (err != E_OK) return err;

    thread->state = UNSCHDED;

    thread->proc = NULL;

    thread->wake_sig = 0;

    *res = thread;

    return err;
}

int thread_kern_new(struct thread_t **thread, addr_t exe, uint nargs, ...)
{
    int err = thread_new(thread);

    if (err != E_OK) return err;

    addr_t stack;

    err = kmalloc(&stack, KERN_STACK_SIZE);

    if (err != E_OK) return err;

    (*thread)->ks_addr = stack;

    (*thread)->ks_size = KERN_STACK_SIZE;

    long *args = (void *)(((addr_t)&nargs) + sizeof(nargs));

    if (nargs == 0)
    {
        task_kern_init(&(*thread)->task, stack, KERN_STACK_SIZE, thread_enclosure, nargs + 2, exe, nargs);
    }

    if (nargs == 1)
    {
        task_kern_init(&(*thread)->task, stack, KERN_STACK_SIZE, thread_enclosure, nargs + 2, exe, nargs, args[0]);
    }

    if (nargs == 2)
    {
        task_kern_init(&(*thread)->task, stack, KERN_STACK_SIZE, thread_enclosure, nargs + 2, exe, nargs, args[0],
                       args[1]);
    }

    if (nargs == 3)
    {
        task_kern_init(&(*thread)->task, stack, KERN_STACK_SIZE, thread_enclosure, nargs + 2, exe, nargs, args[0],
                       args[1], args[2]);
    }

    if (nargs == 4)
    {
        task_kern_init(&(*thread)->task, stack, KERN_STACK_SIZE, thread_enclosure, nargs + 2, exe, nargs, args[0],
                       args[1], args[2], args[3]);
    }

    if (nargs == 5)
    {
        task_kern_init(&(*thread)->task, stack, KERN_STACK_SIZE, thread_enclosure, nargs + 2, exe, nargs, args[0],
                       args[1], args[2], args[3], args[4]);
    }

    if (nargs > 5)
    {
        error("thread kern new error.\n");
    }

    (*thread)->proc = &proc_0;

    list_push_back(&proc_0.thread_ls, &(*thread)->proc_ln);

    schd_run(*thread);

    return err;
}

/**
 * FIXME:CLEAN UP
 */
int thread_user_new(struct thread_t **thread, struct proc_t *proc, addr_t routine, size_t ustk_sz, addr_t arg)
{
    int err = thread_new(thread);

    if (err != E_OK) return err;

    mmu_sync_kern_space(proc_0.ptb.pde, proc->ptb.pde, *thread, sizeof(struct thread_t));

    addr_t kstack = NULL;

    err = kmalloc(&kstack, KERN_STACK_SIZE);

    if (err != E_OK) return err;

    (*thread)->ks_addr = kstack;

    (*thread)->ks_size = KERN_STACK_SIZE;

    mmu_sync_kern_space(proc_0.ptb.pde, proc->ptb.pde, kstack, KERN_STACK_SIZE);

    addr_t ustk_a = NULL;

    err = umalloc(&proc->um, &ustk_a, ustk_sz);

    if (err != E_OK) return err;

    task_user_init(&(*thread)->task, kstack, KERN_STACK_SIZE, ustk_a, ustk_sz, pre, routine);

    (*thread)->proc = proc;

    list_push_back(&proc->thread_ls, &(*thread)->proc_ln);

    schd_run(*thread);

    return err;
}

int thread_free(struct thread_t *thread)
{
    int err = E_OK;

    err = kmfree(thread);

    return err;
}

static int sys_thread_new(uint *tid, addr_t routine, struct thread_attr_t *attr, addr_t arg)
{
    size_t ustk_sz = DFT_STK_SZ;

    if (attr != NULL) ustk_sz = attr->stk_sz;

    return thread_user_new((struct thread_t **)tid, proc_now(), routine, ustk_sz, arg);
}

int thread_fork(struct thread_t *thread, struct proc_t *proc, struct thread_t **res)
{
    struct thread_t *threadn = NULL;

    int err = kmalloc((addr_t *)&threadn, sizeof(struct thread_t));

    if (err != E_OK) return err;

    addr_t stack;

    err = kmalloc(&stack, thread->ks_size);

    if (err != E_OK) return err;

    threadn->ks_addr = stack;

    threadn->ks_size = thread->ks_size;

    threadn->task.saddr = stack;

    threadn->task.ssize = threadn->ks_size;

    threadn->task.sp = thread->task.sp - thread->task.saddr + stack;

    addr_t *src = thread->ks_addr;

    addr_t *dst = threadn->ks_addr;

    for (uint i = 0; i < KERN_STACK_SIZE / 4; ++i)
    {
        if (src[i] >= thread->ks_addr && src[i] < thread->ks_addr + thread->ks_size)
        {
            dst[i] = src[i] - thread->ks_addr + threadn->ks_addr;
        }
        else
        {
            dst[i] = src[i];
        }
    }

    threadn->proc = proc;

    list_push_back(&proc->thread_ls, &threadn->proc_ln);

    *res = threadn;

    return err;
}

static int sys_thread_exit(void)
{
    schd_kill(thread_now());

    panic("Bug.\n");
}

// int thread_kill(struct thread_t *thread)
// {

// }

int thread_init(void)
{
    syscall_register(SYS_thread_new, sys_thread_new, 4);

    syscall_register(SYS_thread_exit, sys_thread_exit, 0);

    return E_OK;
}