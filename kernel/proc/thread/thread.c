#include <thread.h>
#include <list.h>
#include <slot.h>
#include <error.h>
#include <util.h>
#include <arch/cpu.h>
#include <memory.h>
#include <log.h>
#include <arch/intr.h>

static struct list_node_t runnable;

static struct slot_alct_t alct;

static u8 fm[12 * PAGE_SIZE];

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

int thread_new(struct thread_t **thread, addr_t run)
{
    int err = E_OK;

    err = slot_new(&alct, (addr_t *)thread);

    if (err != E_OK)
    {
        return err;
    }

    addr_t stack;

    err = kalloc(&stack, PAGE_SIZE);

    if (err != E_OK)
    {
        slot_free(&alct, *thread);

        return err;
    }

    task_init(&(*thread)->task, stack, PAGE_SIZE, pre, run, suf);

    (*thread)->state = RUNNABEL;

    list_push_back(&runnable, &(*thread)->schd_ln);

    return err;
}

int thread_user_new(struct thread_t **thread, addr_t run)
{
    int err = E_OK;

    err = slot_new(&alct, (addr_t *)thread);

    if (err != E_OK)
    {
        return err;
    }

    addr_t stack;

    err = kalloc(&stack, PAGE_SIZE);

    if (err != E_OK)
    {
        slot_free(&alct, *thread);

        return err;
    }

    task_user_init(&(*thread)->task, stack, PAGE_SIZE, 0, PAGE_SIZE, pre, run);

    (*thread)->state = RUNNABEL;

    list_push_back(&runnable, &(*thread)->schd_ln);

    return err;
}

static int thread_intr(void)
{
    uint cpuid = cpu_id();

    struct task_t *task = cpu_get_task(cpuid);

    struct thread_t *thread = container_of(task, struct thread_t, task);

    thread->state = RUNNABEL;

    list_push_back(&runnable, &thread->schd_ln);

    task_switch(task, cpu_schd(cpuid));

    return E_OK;
}
extern pde_t volatile PDE[];
int thread_schd(void)
{
    uint cpuid = cpu_id();

    while (1)
    {
        if (list_isempty(&runnable) == 0)
        {
            struct thread_t *thread = container_of(list_pop_front(&runnable), struct thread_t, schd_ln);

            thread->state = RUNNING;

            thread->cpuid = cpu_id;

            cpu_set_task(cpuid, &thread->task);

            // debug("%p\n", thread->proc->pde);

            // proc_switch(thread->proc);

            task_switch(cpu_schd(cpuid), &thread->task);
        }
    }
    return E_OK;
}

int thread_init(void)
{
    slot_init(&alct, sizeof(struct thread_t));

    slot_insert(&alct, fm, sizeof(fm));

    list_init(&runnable);

    intr_register(INTR_TIMER, thread_intr);

    return E_OK;
}