#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <arch/atomic.h>
#include <log.h>

struct spinlock_t
{
    struct atomic_t lock;
};

struct spin_rwlock_t
{
    struct atomic_t lock;
};

static inline void spin_init(struct spinlock_t *spinlock)
{
    atomic_set(&spinlock->lock, 0);
}

static inline void spin_lock(struct spinlock_t *spinlock)
{
    while (atomic_xchg(&spinlock->lock, 1) == 1)
        ;
}

static inline int spin_trylock(struct spinlock_t *spinlock)
{
    __sync_synchronize();

    ;

    return atomic_xchg(&spinlock->lock, 1) == 0;
}

static inline void spin_unlock(struct spinlock_t *spinlock)
{
    assert(atomic_read(&spinlock->lock) == 1);

    atomic_set(&spinlock->lock, 0);
}

static inline void spin_rwlock_init(struct spin_rwlock_t *lock)
{
    atomic_set(&lock->lock, 0);
}

static inline void spin_read_lock(struct spin_rwlock_t *lock)
{
    while (atomic_add_unless(&lock->lock, 1, -1) == 0)
        ;

    assert(atomic_read(&lock->lock) > 0);
}

static inline void spin_write_lock(struct spin_rwlock_t *lock)
{
    while (atomic_add_if(&lock->lock, -1, 0) == 0)
        ;

    assert(atomic_read(&lock->lock) == -1);
}

static inline void spin_read_unlock(struct spin_rwlock_t *lock)
{
    assert(atomic_read(&lock->lock) > 0);

    atomic_add(&lock->lock, -1);
}

static inline void spin_write_unlock(struct spin_rwlock_t *lock)
{
    assert(atomic_read(&lock->lock) == -1);

    atomic_set(&lock->lock, 0);
}

static inline int spin_read_islock(struct spin_rwlock_t *lock)
{
    return atomic_read(&lock->lock) > 0;
}

static inline int spin_write_islock(struct spin_rwlock_t *lock)
{
    return atomic_read(&lock->lock) == -1;
}
#endif