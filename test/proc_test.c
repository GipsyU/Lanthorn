#include <arch/mmu.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <mutex.h>
#include <proc.h>
#include <spinlock.h>
#include <thread.h>

volatile struct mutex_t mutex;

int mutex_test_1(void)
{
    while (1)
    {
        mutex_lock(&mutex);
        // debug("t1 get.\n");
        mutex_unlock(&mutex);
        // debug("t1 put.\n");
    }
}

int mutex_test_2(void)
{
    while (1)
    {
        mutex_lock(&mutex);
        // debug("t2 get.\n");
        mutex_unlock(&mutex);
        // debug("t2 put.\n");
    }
}

static int mutex_test_3(void)
{
    while (1)
    {
        mutex_lock(&mutex);
        // debug("t3 get.\n");
        mutex_unlock(&mutex);
        // debug("t3 put.\n");
    }
}

static int mutex_test_4(void)
{
    while (1)
    {
        mutex_lock(&mutex);
        // debug("t4 get.\n");
        mutex_unlock(&mutex);
        // debug("t4 put.\n");
    }
}

struct spinlock_t lock;

static int spinlock_test_1(void)
{
    while (1)
    {
        spin_lock(&lock);
        // debug("t1 get.\n");
        spin_unlock(&lock);
        // debug("t1 put.\n");
    }
}

static int spinlock_test_2(void)
{
    while (1)
    {
        spin_lock(&lock);
        // debug("t2 get.\n");
        spin_unlock(&lock);
        // debug("t2 put.\n");
    }
}
static int spinlock_test_3(void)
{
    while (1)
    {
        spin_lock(&lock);
        // debug("t3 get.\n");
        spin_unlock(&lock);
        // debug("t3 put.\n");
    }
}

static int spinlock_test_4(void)
{
    while (1)
    {
        spin_lock(&lock);
        // debug("t4 get.\n");
        spin_unlock(&lock);
        // debug("t4 put.\n");
    }
}

volatile struct thread_t * volatile t = NULL;

void bolck_test1(void)
{
    while(1){
        spin_lock(&lock);
        t = thread_now();
        schd_block(thread_now(), &lock);
        // debug("OK2\n");
    }
}

void bolck_test2(void)
{
    while(1){
        spin_lock(&lock);
        if (t){
            struct thread_t *f=t;
            t=0;
            schd_run(f);
        }
        spin_unlock(&lock);
    }
}
extern struct proc_t proc_0;

int proc_test(void)
{
    int err = E_OK;

    struct thread_t *t1, *t2, *t3, *t4;

    mutex_init(&mutex);

    spin_init(&lock);

    thread_kern_new(&proc_0, &t1, mutex_test_1, 0);

    thread_kern_new(&proc_0, &t2, mutex_test_2, 0);

    thread_kern_new(&proc_0, &t3, mutex_test_3, 0);

    thread_kern_new(&proc_0, &t4, mutex_test_4, 0);

    // thread_kern_new(&proc_0, &t1, spinlock_test_1, 0);

    // thread_kern_new(&proc_0, &t2, spinlock_test_2, 0);

    // thread_kern_new(&proc_0, &t3, spinlock_test_3, 0);
    
    // thread_kern_new(&proc_0, &t4, spinlock_test_4, 0);

    // thread_kern_new(&proc_0, &t1, bolck_test1, 0);

    // thread_kern_new(&proc_0, &t2, bolck_test2, 0);

    return E_OK;
}