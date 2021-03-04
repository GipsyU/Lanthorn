#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#include <arch/atomic.h>
#include <log.h>
#include <mutex.h>

struct rwlock_t
{
    struct mutex_t queue;
    struct mutex_t rwlock;
    struct atomic_t count;
};

static inline void rwlock_init(struct rwlock_t *lock)
{
    mutex_init(&lock->queue);

    mutex_init(&lock->rwlock);

    atomic_set(&lock->count, 0);
}

static inline void rwlock_read_lock(struct rwlock_t *lock)
{
    mutex_lock(&lock->queue);

    assert(atomic_read(&lock->count) >= 0);

    if (atomic_add_return(&lock->count, 1) == 1) mutex_lock(&lock->rwlock);

    mutex_unlock(&lock->queue);
}

static inline void rwlock_write_lock(struct rwlock_t *lock)
{
    mutex_lock(&lock->queue);

    mutex_lock(&lock->rwlock);

    assert(atomic_add_return(&lock->count, -1) == -1);
}

static inline void rwlock_read_unlock(struct rwlock_t *lock)
{
    assert(atomic_read(&lock->count) > 0);

    if (atomic_add_return(&lock->count, -1) == 0) mutex_unlock(&lock->rwlock);
}

static inline void rwlock_write_unlock(struct rwlock_t *lock)
{
    assert(atomic_add_return(&lock->count, 1) == 0);

    mutex_unlock(&lock->rwlock);

    mutex_unlock(&lock->queue);
}

#endif