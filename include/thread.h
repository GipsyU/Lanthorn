#ifndef _THREAD_H_
#define _THREAD_H_

#include <arch/basic.h>
#include <arch/cpu.h>
#include <arch/task.h>
#include <list.h>
#include <proc.h>

#define KERN_STACK_SIZE (8 * PAGE_SIZE)

#define DFT_STK_SZ (PAGE_SIZE * 1024)

enum T_STATE
{
    UNSCHDED,
    RUNNABEL,
    RUNNING,
    SLEEPING,
    BLOCKED,
    KILLED
};

struct thread_t
{
    uint cpuid;
    enum T_STATE state;
    addr_t ks_addr;
    size_t ks_size;
    long wake_sig;
    struct task_t task;
    struct proc_t *proc;
    struct spinlock_t lock;
    struct list_node_t schd_ln;
    struct list_node_t proc_ln;
    struct list_node_t mutex_wait_ln;
};

struct schd_t
{
    struct spinlock_t lock;
    struct list_node_t runnable;
    struct list_node_t sleeping;
};

struct thread_t *thread_now(void);

int thread_kern_new(struct proc_t *proc, struct thread_t **thread, addr_t exe, uint nargs, ...);

int thread_user_new(struct thread_t **thread, struct proc_t *proc, addr_t exe, size_t ustk_sz, addr_t arg);

int thread_init(void);

int thread_sleep(void);

int thread_fork(struct thread_t *thread, struct proc_t *proc, struct thread_t **res);

int schd_schdule(void);

int schd_run(struct thread_t *thread);

int schd_kill(struct thread_t *thread);

int schd_sleep(struct thread_t *thread, long wake_sig);

int schd_wake(long wake_sig);

int schd_block(struct thread_t *thread);

int schd_init(void);

#endif