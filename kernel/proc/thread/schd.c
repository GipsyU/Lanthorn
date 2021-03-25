#include <arch/intr.h>
#include <error.h>
#include <log.h>
#include <thread.h>
#include <util.h>

struct schd_t scheduler;

static void set_runnable(struct thread_t *thread)
{
    thread->state = RUNNABEL;

    list_push_back(&scheduler.runnable, &thread->schd_ln);
}

int schd_run(struct thread_t *thread)
{
    spin_lock_irqsave(&scheduler.lock);

    // info("set thread %p runnable.\n", thread);

    if (thread->state == RUNNABEL || thread->state == RUNNING || thread->state == SLEEPING || thread->state == KILLED)
    {
        debug("%d", thread->state);

        panic("schd run bug.\n");
    }
    else if (thread->state == UNSCHDED || thread->state == BLOCKED)
    {
        set_runnable(thread);
    }
    else
    {
        panic("schd run bug.\n");
    }

    spin_unlock_irqrestore(&scheduler.lock);

    return E_OK;
}

int schd_sleep(struct thread_t *thread, long wake_sig)
{
    // assert(wake_sig != 0);

    // assert(thread == thread_now());

    // spin_lock_irqsave(&scheduler.lock);

    // if (thread->state == KILLED || thread->state == UNSCHDED || thread->state == SLEEPING)
    // {
    //     error("schd sleep bug.\n");
    // }
    // else if (thread->state == RUNNABEL)
    // {
    //     thread->state = SLEEPING;

    //     thread->wake_sig = wake_sig;

    //     list_delete(&thread->schd_ln);

    //     list_push_back(&scheduler.sleeping, &thread->schd_ln);
    // }
    // else if (thread->state == RUNNING)
    // {
    //     uint cpuid = cpu_id();

    //     thread->state = SLEEPING;

    //     thread->wake_sig = wake_sig;

    //     list_push_back(&scheduler.sleeping, &thread->schd_ln);

    //     spin_unlock_irqrestore(&scheduler.lock);

    //     // task_switch(&thread->task, cpu_schd(cpuid));

    //     return E_OK;
    // }
    // else
    // {
    //     error("schd sleep bug.\n");
    // }

    // spin_unlock_irqrestore(&scheduler.lock);

    return E_OK;
}

int schd_kill(struct thread_t *thread)
{
    // spin_lock_irqsave(&scheduler.lock);

    // if (thread->state == KILLED)
    // {
    //     panic("killed twice.\n");
    // }
    // else if (thread->state == RUNNABEL || thread->state == SLEEPING)
    // {
    //     thread->state = KILLED;

    //     list_delete(&thread->schd_ln);
    // }
    // else if (thread->state == RUNNING)
    // {
    //     assert(thread == thread_now());

    //     uint cpuid = cpu_id();

    //     thread->state = KILLED;

    //     int first = 1;

    //     // task_save(&thread->task);

    //     if (first)
    //     {
    //         first = 0;
            
    //         cpu_set_task(cpuid, cpu_schd(cpuid));

    //         spin_unlock(&scheduler.lock);

    //         // task_load(cpu_schd(cpuid));
    //     }

    //     panic("bug.\n");
    // }
    // else if (thread->state == UNSCHDED)
    // {
    //     panic("schd error.\n");
    // }
    // else
    // {
    //     panic("schd error.\n");
    // }

    // spin_unlock_irqrestore(&scheduler.lock);

    return E_OK;
}

int schd_wake(long wake_sig)
{
    // spin_lock_irqsave(&scheduler.lock);

    // list_rep_s(&scheduler.sleeping, p)
    // {
    //     struct thread_t *thread = container_of(p, struct thread_t, schd_ln);

    //     assert(thread->state == SLEEPING);

    //     if (thread->wake_sig == wake_sig)
    //     {
    //         thread->wake_sig = 0;

    //         list_delete(p);

    //         set_runnable(thread);
    //     }
    // }

    // spin_unlock_irqrestore(&scheduler.lock);

    return E_OK;
}

static void schd_schdule_mid_func(addr_t args)
{
    int cpuid = cpu_now();

    struct thread_t *thread = (void *)args;

    thread->state = RUNNING;

    // info("schedule cpu:%p thread:%p process: %p.\n", cpuid, thread, thread->proc);

    assert(intr_irq_state() == 0);

    proc_switch(thread->proc);

    thread->cpuid = cpuid;
    
    cpu_set_task(cpuid, &thread->task);
}

int schd_schdule(void)
{
    info("begin to schedule threads.\n");

    int cpuid = cpu_now();

    while (1)
    {
        spin_lock(&scheduler.lock);

        if (list_isempty(&scheduler.runnable) == 0)
        {
            struct thread_t *thread = container_of(list_pop_front(&scheduler.runnable), struct thread_t, schd_ln);

            task_switch(cpu_schd(cpuid), &thread->task, schd_schdule_mid_func, (addr_t)thread);
        }
        else
        {
            // debug("cpu free\n");
        }

        spin_unlock(&scheduler.lock);

    }
}

static void schd_intr_mid_func(addr_t args)
{
    int cpuid = cpu_now();

    volatile struct task_t *task = cpu_get_task(cpuid);

    volatile struct thread_t *thread = container_of(task, struct thread_t, task);

    spin_lock(&scheduler.lock);

    assert(thread->state == RUNNING);

    // info("schedule thread: %p finished.\n", thread);

    set_runnable(thread);

    cpu_set_task(cpuid, cpu_schd(cpuid));
}

static int schd_intr(uint errno)
{
    int cpuid = cpu_now();

    struct task_t *task = cpu_get_task(cpuid);

    task_switch(task, cpu_schd(cpuid), schd_intr_mid_func, NULL);

    spin_unlock(&scheduler.lock);

    return E_OK;
}

int schd_init(void)
{
    spin_init(&scheduler.lock);

    list_init(&scheduler.runnable);

    list_init(&scheduler.sleeping);

    intr_register(INTR_TIMER, schd_intr);

    return E_OK;
}


static void schd_block_mid_func(addr_t args)
{
    int cpuid = cpu_now();

    cpu_set_task(cpuid, cpu_schd(cpuid));
}

int schd_block(struct thread_t *thread, struct spinlock_t *lock)
{
    assert(thread == thread_now());

    assert(thread->state == RUNNING);

    spin_lock_irqsave(&scheduler.lock);

    thread->state = BLOCKED;

    if (lock) spin_unlock(lock);

    barrier();

    // info("block thread %p.\n", thread);

    task_switch(&thread->task, cpu_schd(cpu_id()), schd_block_mid_func, NULL);

    spin_unlock(&scheduler.lock);

    intr_end();

    return E_OK;
}