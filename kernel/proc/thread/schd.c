#include <arch/intr.h>
#include <error.h>
#include <log.h>
#include <thread.h>
#include <util.h>

static void set_runnable(struct thread_t *thread)
{
    thread->state = RUNNABEL;

    list_push_back(&scheduler.runnable, &thread->schd_ln);
}

int schd_run(struct thread_t *thread)
{
    spin_lock(&scheduler.lock);

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
        error("schd run bug.\n");
    }

    spin_unlock(&scheduler.lock);

    return E_OK;
}

int schd_sleep(struct thread_t *thread, long wake_sig)
{
    assert(wake_sig != 0);

    assert(thread == thread_now());

    spin_lock(&scheduler.lock);

    if (thread->state == KILLED || thread->state == UNSCHDED || thread->state == SLEEPING)
    {
        error("schd sleep bug.\n");
    }
    else if (thread->state == RUNNABEL)
    {
        thread->state = SLEEPING;

        thread->wake_sig = wake_sig;

        list_delete(&thread->schd_ln);

        list_push_back(&scheduler.sleeping, &thread->schd_ln);
    }
    else if (thread->state == RUNNING)
    {
        uint cpuid = cpu_id();

        thread->state = SLEEPING;

        thread->wake_sig = wake_sig;

        list_push_back(&scheduler.sleeping, &thread->schd_ln);

        spin_unlock(&scheduler.lock);

        task_switch(&thread->task, cpu_schd(cpuid));

        return E_OK;
    }
    else
    {
        error("schd sleep bug.\n");
    }

    spin_unlock(&scheduler.lock);

    return E_OK;
}

int schd_kill(struct thread_t *thread)
{
    spin_lock(&scheduler.lock);

    if (thread->state == KILLED)
    {
        warn("killed twice.\n");
    }
    else if (thread->state == RUNNABEL || thread->state == SLEEPING)
    {
        thread->state = KILLED;

        list_delete(&thread->schd_ln);
    }
    else if (thread->state == RUNNING)
    {
        assert(thread == thread_now());

        uint cpuid = cpu_id();

        thread->state = KILLED;

        spin_unlock(&scheduler.lock);

        task_switch(&thread->task, cpu_schd(cpuid));

        panic("bug.\n");
    }
    else if (thread->state == UNSCHDED)
    {
        error("schd error.\n");
    }
    else
    {
        error("schd error.\n");
    }

    spin_unlock(&scheduler.lock);

    return E_OK;
}

int schd_wake(long wake_sig)
{
    spin_lock(&scheduler.lock);

    list_rep_s(&scheduler.sleeping, p)
    {
        struct thread_t *thread = container_of(p, struct thread_t, schd_ln);

        assert(thread->state == SLEEPING);

        if (thread->wake_sig == wake_sig)
        {
            thread->wake_sig = 0;

            list_delete(p);

            set_runnable(thread);
        }
    }

    spin_unlock(&scheduler.lock);

    return E_OK;
}

int schd_schdule(void)
{
    info("begin to schedule threads.\n");

    uint cpuid = cpu_id();

    while (1)
    {
        spin_lock(&scheduler.lock);

        if (list_isempty(&scheduler.runnable) == 0)
        {
            struct thread_t *thread = container_of(list_pop_front(&scheduler.runnable), struct thread_t, schd_ln);

            thread->state = RUNNING;

            thread->cpuid = cpu_id();

            info("schedule thread %p.\n", thread);

            spin_unlock(&scheduler.lock);

            proc_switch(thread->proc);

            cpu_set_task(cpuid, &thread->task);

            task_switch(cpu_schd(cpuid), &thread->task);
        }
        else
        {
            spin_unlock(&scheduler.lock);
        }
    }
}

static int schd_intr(uint errno)
{
    uint cpuid = cpu_id();

    struct task_t *task = cpu_get_task(cpuid);

    struct thread_t *thread = container_of(task, struct thread_t, task);

    spin_lock(&scheduler.lock);

    assert(thread->state == RUNNING);

    set_runnable(thread);

    spin_unlock(&scheduler.lock);

    task_switch(task, cpu_schd(cpuid));

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

int schd_block(struct thread_t *thread)
{
    assert(thread == thread_now());

    spin_lock(&scheduler.lock);

    if (thread->state == KILLED || thread->state == UNSCHDED || thread->state == SLEEPING || thread->state == BLOCKED)
    {
        error("schd block bug.\n");
    }
    else if (thread->state == RUNNABEL)
    {
        thread->state = BLOCKED;

        list_delete(&thread->schd_ln);

        list_push_back(&scheduler.sleeping, &thread->schd_ln);
    }
    else if (thread->state == RUNNING)
    {
        uint cpuid = cpu_id();

        thread->state = BLOCKED;

        list_push_back(&scheduler.sleeping, &thread->schd_ln);

        spin_unlock(&scheduler.lock);

        task_switch(&thread->task, cpu_schd(cpuid));

        return E_OK;
    }
    else
    {
        error("schd block bug.\n");
    }

    spin_unlock(&scheduler.lock);

    return E_OK;
}