#include <arch/mmu.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <mutex.h>
#include <proc.h>
#include <spinlock.h>
#include <thread.h>

struct mutex_t mutex;

static int mutex_test_1(void)
{
    while (1)
    {
        mutex_lock(&mutex);
        // debug("t1 get.\n");
        // int t = 100000000;
        // while(t--);

        // debug("t1 put.\n");
        mutex_unlock(&mutex);
    }
}

static int mutex_test_2(void)
{
    while (1)
    {
        mutex_lock(&mutex);

        // debug("t2 get.\n");
        // int t = 100000000;
        // while(t--);

        // debug("t2 put.\n");

        mutex_unlock(&mutex);
    }
}

struct spinlock_t lock;
static int spinlock_test_1(void)
{
    while (1)
    {
        spin_lock(&lock);
        spin_unlock(&lock);
    }
}

static int spinlock_test_2(void)
{
    while (1)
    {
        // mutex_lock(&mutex);
        spin_lock(&lock);
        // debug("t2 get.\n");
        // debug("t2 put.\n");
        spin_unlock(&lock);
        // mutex_unlock(&mutex);
    }
}

static int spinlock_test_3(void)
{
    while (1)
    {
        spin_lock(&lock);
        // debug("t2 get.\n");
        // debug("t2 put.\n");
        // asm volatile("rep;nop;");
        // debug(".\n");
        spin_unlock(&lock);
    }
}

static int spinlock_test_4(void)
{
    while (1)
    {
        spin_lock(&lock);
        // debug("t2 get.\n");
        // debug("t2 put.\n");
        // asm volatile("rep;nop;");
        // debug(".\n");
        spin_unlock(&lock);
    }
}

int proc_test(void)
{
    int err = E_OK;

    struct thread_t *t1, *t2, *t3, *t4;

    mutex_init(&mutex);

    spin_init(&lock);

    // thread_kern_new(&t1, spinlock_test_1, 0);

    // thread_kern_new(&t1, spinlock_test_2, 0);

    // thread_kern_new(&t3, spinlock_test_3, 0);

    // thread_kern_new(&t4, spinlock_test_4, 0);

    schd_schdule();
}