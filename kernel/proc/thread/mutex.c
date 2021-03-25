#include <error.h>
#include <mutex.h>
#include <thread.h>
#include <util.h>

void mutex_init(struct mutex_t *mutex)
{
    atomic_set(&mutex->count, 1);

    spin_init(&mutex->lock);

    list_init(&mutex->wait_ls);

    mutex->owner = NULL;
}

void mutex_lock(struct mutex_t *mutex)
{
    spin_lock(&mutex->lock);

    assert(mutex->owner != thread_now());

    barrier();

    if (atomic_sub_and_test(&mutex->count, 1))
    {
        barrier();
        
        mutex->owner = thread_now();

        spin_unlock(&mutex->lock);

        return;
    }

    barrier();

    list_push_back(&mutex->wait_ls, &thread_now()->mutex_wait_ln);

    schd_block(thread_now(), &mutex->lock);

    assert(mutex->owner == thread_now());
}

void mutex_unlock(struct mutex_t *mutex)
{
    assert(mutex->owner == thread_now());

    barrier();

    mutex->owner = NULL;
    
    barrier();
    
    if (atomic_add_return(&mutex->count, 1) == 1)
    {
        barrier();

        return;
    }

    barrier();

    spin_lock(&mutex->lock);

    struct thread_t *thread = container_of(list_pop_front(&mutex->wait_ls), struct thread_t, mutex_wait_ln);

    mutex->owner = thread;

    spin_unlock(&mutex->lock);

    schd_run(thread);
}