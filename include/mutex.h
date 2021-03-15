#ifndef _MUTEX_H_
#define _MUTEX_H_
#include <arch/atomic.h>
#include <list.h>
#include <proc.h>
#include <spinlock.h>

struct mutex_t
{
    struct atomic_t count;
    struct spinlock_t wait_lock;
    struct list_node_t wait_ls;
    struct thread_t *owner;
};

void mutex_init(struct mutex_t *mutex);

void mutex_lock(struct mutex_t *mutex);

int mutex_trylock(struct mutex_t *mutex);

void mutex_unlock(struct mutex_t *mutex);

#endif