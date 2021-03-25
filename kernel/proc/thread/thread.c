#include <arch/cpu.h>
#include <arch/intr.h>
#include <arch/mmu.h>
#include <error.h>
#include <idx_aclt.h>
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

struct THREAD_T
{
    struct idx_alct_t idx_alct;

}THREAD;

static void pre(void)
{
    info("usr thread begin\n");

    spin_unlock(&scheduler.lock);

    intr_end();
}

static void thread_enclosure(addr_t func, uint nargs, ...)
{
    info("kern thread begin.\n");

    spin_unlock(&scheduler.lock);

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

    info("thread %p end, killed.\n", thread_now());

    schd_kill(thread_now());

    while (1)
        ;
}

struct thread_t *thread_now(void)
{
    intr_irq_save();

    struct task_t *task = cpu_get_task(cpu_id());

    intr_irq_restore();

    return container_of(task, struct thread_t, task);
}

int thread_id(void)
{
    return thread_now()->tid;
}

static int thread_new(struct thread_t **res)
{
    struct thread_t *thread = NULL;

    int err = kmalloc((addr_t *)&thread, sizeof(struct thread_t));

    if (err != E_OK) goto err1;

    thread->state = UNSCHDED;

    thread->proc = NULL;

    thread->wake_sig = 0;

    err = idx_alct_new(&THREAD.idx_alct, thread, &thread->tid);

    if (err != E_OK) goto err2;

    if (res != NULL) *res = thread;

    return err;

err2:
    kmfree(thread);

err1:
    return err;
}

int thread_kern_new(struct proc_t *proc, struct thread_t **thread, addr_t exe, uint nargs, ...)
{
    if (proc == NULL) proc = &proc_0;

    struct thread_t *_thread;

    int err = thread_new(&_thread);

    if (err != E_OK) return err;

    addr_t stack;

    err = kmalloc(&stack, KERN_STACK_SIZE);

    if (err != E_OK) return err;

    _thread->ks_addr = stack;

    _thread->ks_size = KERN_STACK_SIZE;

    long *args = (void *)(((addr_t)&nargs) + sizeof(nargs));

    if (nargs == 0)
    {
        task_kern_init(&_thread->task, stack, KERN_STACK_SIZE, thread_enclosure, nargs + 2, exe, nargs);
    }

    if (nargs == 1)
    {
        task_kern_init(&_thread->task, stack, KERN_STACK_SIZE, thread_enclosure, nargs + 2, exe, nargs, args[0]);
    }

    if (nargs == 2)
    {
        task_kern_init(&_thread->task, stack, KERN_STACK_SIZE, thread_enclosure, nargs + 2, exe, nargs, args[0],
                       args[1]);
    }

    if (nargs == 3)
    {
        task_kern_init(&_thread->task, stack, KERN_STACK_SIZE, thread_enclosure, nargs + 2, exe, nargs, args[0],
                       args[1], args[2]);
    }

    if (nargs == 4)
    {
        task_kern_init(&_thread->task, stack, KERN_STACK_SIZE, thread_enclosure, nargs + 2, exe, nargs, args[0],
                       args[1], args[2], args[3]);
    }

    if (nargs == 5)
    {
        task_kern_init(&_thread->task, stack, KERN_STACK_SIZE, thread_enclosure, nargs + 2, exe, nargs, args[0],
                       args[1], args[2], args[3], args[4]);
    }

    if (nargs > 5)
    {
        error("thread kern new error.\n");
    }

    _thread->proc = proc;

    list_push_back(&proc->thread_ls, &_thread->proc_ln);

    schd_run(_thread);

    if (thread != NULL) *thread = _thread;

    return err;
}

int thread_user_new(struct thread_t **thread, struct proc_t *proc, addr_t routine, size_t ustk_sz, addr_t arga,
                    size_t argsz)
{
    struct thread_t *_thread = NULL;

    int err = thread_new(&_thread);

    if (err != E_OK) return err;

    mmu_sync_kern_space(proc_0.ptb.pde, proc->ptb.pde, _thread, sizeof(struct thread_t));

    addr_t kstack = NULL;

    err = kmalloc(&kstack, KERN_STACK_SIZE);

    if (err != E_OK) return err;

    _thread->ks_addr = kstack;

    _thread->ks_size = KERN_STACK_SIZE;

    mmu_sync_kern_space(proc_0.ptb.pde, proc->ptb.pde, kstack, KERN_STACK_SIZE);

    addr_t ustk_a = NULL;

    err = umalloc(&proc->um, &ustk_a, ustk_sz);

    if (err != E_OK) return err;

    task_user_init(&_thread->task, kstack, KERN_STACK_SIZE, ustk_a, ustk_sz, pre, routine, arga, argsz);

    _thread->proc = proc;

    list_push_back(&proc->thread_ls, &_thread->proc_ln);

    schd_run(_thread);

    if (thread != NULL) *thread = _thread;

    return err;
}

int thread_free(struct thread_t *thread)
{
    int err = E_OK;

    err = kmfree(thread);

    return err;
}

static int thread_sys_new(uint *tid, addr_t routine, struct thread_attr_t *attr)
{
    size_t ustk_sz = DFT_STK_SZ;

    if (attr != NULL && attr->stk_sz != NULL) ustk_sz = attr->stk_sz;

    return thread_user_new((struct thread_t **)tid, proc_now(), routine, ustk_sz, attr->arga, attr->argsz);
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

    // threadn->task.sp = thread->task.sp - thread->task.saddr + stack;

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

static int thread_sys_exit(void)
{
    schd_kill(thread_now());

    panic("Bug.\n");
}

static int thread_sys_block(uint tid)
{
    struct thread_t *thread = (void *)tid;

    if (thread->proc != proc_now()) return E_INVAL;

    return schd_block(tid, NULL);
}

static int thread_sys_tid(uint *tid)
{
    *tid = (uint)thread_now();

    return E_OK;
}

static int thread_sys_wake(uint tid)
{
    struct thread_t *thread = (void *)tid;

    if (thread->proc != proc_now()) return E_INVAL;

    return schd_run(tid);
}

int thread_init(void)
{
    syscall_register(SYS_thread_create, thread_sys_new, 3);

    syscall_register(SYS_thread_block, thread_sys_block, 1);

    syscall_register(SYS_thread_wake, thread_sys_wake, 1);

    syscall_register(SYS_thread_tid, thread_sys_tid, 1);

    syscall_register(SYS_thread_exit, thread_sys_exit, 0);

    idx_aclt_init(&THREAD.idx_alct);

    int err = schd_init();

    return err;
}