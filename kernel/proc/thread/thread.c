#include <arch/cpu.h>
#include <arch/intr.h>
#include <error.h>
#include <list.h>
#include <log.h>
#include <memory.h>
#include <thread.h>
#include <util.h>
#include <proc.h>
#include <spinlock.h>
#include <arch/mmu.h>
static struct schd_q_t
{
    struct spinlock_t lock;
    struct list_node_t queue;
}runnable;

static void pre(void)
{
    info("thread begin\n");
    intr_end();
}

static void suf(void)
{
    info("thread end\n");
    while (1)
        ;
}

static int queue_init(struct schd_q_t *q)
{
    spin_init(&q->lock);

    list_init(&q->queue);

    return E_OK;
}

struct thread_t *thread_now(void)
{
    struct task_t *task = cpu_get_task(cpu_id());

    return container_of(task, struct thread_t, task);
}

int thread_kern_new(struct thread_t **thread, struct proc_t *proc, addr_t exe)
{
    int err = kmalloc((addr_t *)thread, sizeof(struct thread_t));

    if (err != E_OK) return err;

    err = mmu_sync_kern_space(proc->pagetb.pde->addr, *thread);

    if (err != E_OK) return err;
    
    addr_t stack;

    err = kmalloc(&stack, PAGE_SIZE);

    if (err != E_OK)
    {
        kmfree(*thread);

        return err;
    }

    err = mmu_sync_kern_space(proc->pagetb.pde->addr, stack);

    if (err != E_OK) return err;

    task_init(&(*thread)->task, stack, PAGE_SIZE, pre, exe, suf);

    (*thread)->state = RUNNABEL;

    (*thread)->proc = proc;

    list_push_back(&proc->thread_ls, &(*thread)->proc_ln);

    list_push_back(&runnable.queue, &(*thread)->schd_ln);

    return err;
}

/**
 * FIXME:CLEAN UP
 */
int thread_user_new(struct thread_t **thread, struct proc_t *proc, addr_t exe, size_t ustk_sz)
{
    int err = kmalloc((addr_t *)thread, sizeof(struct thread_t));

    if (err != E_OK) return err;

    err = mmu_sync_kern_space(proc->pagetb.pde->addr, *thread);

    if (err != E_OK) return err;
    
    addr_t kstack = NULL;

    err = kmalloc(&kstack, PAGE_SIZE);

    if (err != E_OK) return err;

    err = mmu_sync_kern_space(proc->pagetb.pde->addr, kstack);

    if (err != E_OK) return err;
    
    addr_t ustk_a = NULL;

    err = um_stack_alloc(&proc->um, &ustk_a, PAGE_SIZE);

    if (err != E_OK) return err;

    struct page_t *page;
    
    err = page_alloc(&page);

    if (err != E_OK) return err;

    err = mmu_user_map(proc->pagetb.pde->addr, page->addr, ustk_a, 1);

    if (err != E_OK) return err;

    task_user_init(&(*thread)->task, kstack, PAGE_SIZE, ustk_a, PAGE_SIZE, pre, exe);

    (*thread)->state = RUNNABEL;

    (*thread)->proc = proc;

    list_push_back(&proc->thread_ls, &(*thread)->proc_ln);

    list_push_back(&runnable.queue, &(*thread)->schd_ln);

    return err;
}

int thread_free(struct thread_t *thread)
{
    int err = E_OK;

    err = kmfree(thread);

    return err;
}

static int thread_intr(uint errno)
{
    uint cpuid = cpu_id();

    struct task_t *task = cpu_get_task(cpuid);

    struct thread_t *thread = container_of(task, struct thread_t, task);

    thread->state = RUNNABEL;

    list_push_back(&runnable.queue, &thread->schd_ln);

    task_switch(task, cpu_schd(cpuid));

    return E_OK;
}

int thread_schd(void)
{
    uint cpuid = cpu_id();

    while (1)
    {
        if (list_isempty(&runnable.queue) == 0)
        {
            struct thread_t *thread = container_of(list_pop_front(&runnable.queue), struct thread_t, schd_ln);

            thread->state = RUNNING;

            thread->cpuid = cpu_id();

            cpu_set_task(cpuid, &thread->task);

            proc_switch(thread->proc);

            task_switch(cpu_schd(cpuid), &thread->task);
        }
    }
    return E_OK;
}

int thread_init(void)
{
    queue_init(&runnable);

    intr_register(INTR_TIMER, thread_intr);

    return E_OK;
}