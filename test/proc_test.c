#include <arch/mmu.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <mutex.h>
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

    while (1)
    {
        spin_lock(&lock);

        spin_unlock(&lock);
    }
}

struct mutex_t mutex;

static int mutex_test_1(void)
{
    while(1)
    {
        mutex_lock(&mutex);
        debug("t1 get.\n");
        int t = 100000000;
        while(t--);

        debug("t1 put.\n");
        mutex_unlock(&mutex);
    }
}

static int mutex_test_2(void)
{
    while(1)
    {
        mutex_lock(&mutex);

        debug("t2 get.\n");
        int t = 100000000;
        while(t--);

        debug("t2 put.\n");

        mutex_unlock(&mutex);
    }

}

static int mutex_test_3(void)
{
    while(1)
    {
        mutex_lock(&mutex);
        debug("t2 get.\n");
        debug("t2 put.\n");
        mutex_unlock(&mutex);
    }

}
int proc_test(void)
{
    int err = E_OK;

    struct thread_t *t1, *t2;

    mutex_init(&mutex);

    // thread_kern_new(&t1, mutex_test_1, 0);

    // thread_kern_new(&t1, mutex_test_2, 0);
    
    thread_kern_new(&t1, mutex_test_3, 0);
    
    schd_schdule();
}