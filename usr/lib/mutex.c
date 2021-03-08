#include <mutex.h>
#include <proc.h>
#include <mm.h>
#include <log.h>
#include <utils.h>

struct mutex_tid_t
{
    uint tid;
    struct list_node_t wait_ln;
};

void mutex_init(struct mutex_t *mutex)
{
    atomic_set(&mutex->count, 1);

    spin_init(&mutex->wait_lock);

    list_init(&mutex->wait_ls);
}

void mutex_lock(struct mutex_t *mutex)
{
    if (atomic_sub_and_test(&mutex->count, 1)) return;

    spin_lock(&mutex->wait_lock);

    struct mutex_tid_t *tid;

    malloc((void *)&tid, sizeof(struct mutex_tid_t));

    thread_tid(&tid->tid);

    list_push_back(&mutex->wait_ls, &tid->wait_ln);

    spin_unlock(&mutex->wait_lock);

    thread_block(tid->tid);
}

uint mutex_islock(struct mutex_t *mutex)
{
    return atomic_read(&mutex->count) != 1;
}

void mutex_unlock(struct mutex_t *mutex)
{
    uint _tid;

    thread_tid(&_tid);

    if (atomic_add_return(&mutex->count, 1) == 1) return;

    spin_lock(&mutex->wait_lock);

    struct mutex_tid_t *tid = container_of(list_pop_front(&mutex->wait_ls), struct mutex_tid_t, wait_ln);

    spin_unlock(&mutex->wait_lock);

    thread_wake(tid->tid);

    return;
}