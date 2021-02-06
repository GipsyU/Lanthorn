#include <arch/mmu.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <proc.h>
#include <spinlock.h>
#include <thread.h>
void T11(void)
{
    for (uint i = 0;; ++i)
    {
        if (i % 100000000 == 0)
        {
            debug("t11\n");
        }
    }

    while (1)
        ;
}
void T12(void)
{
    for (uint i = 0;; ++i)
    {
        if (i % 100000000 == 0)
        {
            debug("t12\n");
        }
    }
}

void T21(void)
{
    for (uint i = 0;; ++i)
    {
        if (i % 100000000 == 0)
        {
            debug("t21\n");
        }
    }
}

void T22(void)
{
    for (uint i = 0;; ++i)
    {
        if (i % 100000000 == 0)
        {
            debug("t22\n");
        }
    }
}

void lock(void)
{
    struct spinlock_t lock;

    spin_init(&lock);

    while(1)
    {
        spin_lock(&lock);

        spin_unlock(&lock);
    }
}

int proc_test(void)
{
    int err = E_OK;

    struct thread_t *t11, *t12, *t21, *t22;

    // err = thread_kern_new(&t11, lock, 0);

    // err = thread_kern_new(&t12, T12);

    // err = thread_kern_new(&t21, T21);

    // err = thread_kern_new(&t22, T22);

    schd_schdule();
}