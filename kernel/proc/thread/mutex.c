#include <mutex.h>
#include <thread.h>
#include <util.h>

void mutex_init(struct mutex_t *mutex)
{
    atomic_set(&mutex->count, 1);

    spin_init(&mutex->wait_lock);

    list_init(&mutex->wait_ls);

    mutex->owner = NULL;
}

void mutex_lock(struct mutex_t *mutex)
{
    if (atomic_sub_and_test(&mutex->count, 1))
    {
        mutex->owner = thread_now();

        return;
    }

    spin_lock(&mutex->wait_lock);

    list_push_back(&mutex->wait_ls, &thread_now()->mutex_wait_ln);

    spin_unlock(&mutex->wait_lock);

    schd_block(thread_now());

    assert(mutex->owner == thread_now()); 
}

int mutex_trylock(struct mutex_t *mutex)
{
    if (atomic_sub_and_test(&mutex->count, 1))
    {
        mutex->owner = thread_now();

        return 1;
    }

    return 0;
}

void mutex_unlock(struct mutex_t *mutex)
{
    assert(mutex->owner == thread_now());

    mutex->owner == NULL;

    if (atomic_add_return(&mutex->count, 1) == 1) return;

    spin_lock(&mutex->wait_lock);

    struct thread_t *thread = container_of(list_pop_front(&mutex->wait_ls), struct thread_t, mutex_wait_ln);

    spin_unlock(&mutex->wait_lock);

    mutex->owner = thread;

    schd_run(thread);
}