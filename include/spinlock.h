#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <arch/atomic.h>
#include <arch/cpu.h>
#include <log.h>

struct spinlock_t
{
    struct atomic_t lock;
};

static inline int trylock(struct spinlock_t *spinlock)
{
    return atomic_cmpxchg(&spinlock->lock, 0, 1) == 0;
}

static inline void spin_init(struct spinlock_t *spinlock)
{
    atomic_set(&spinlock->lock, 0);
}

static inline void spin_lock(struct spinlock_t *spinlock)
{
    while(atomic_xchg(&spinlock->lock, 1) == 1);
}

static inline int spin_trylock(struct spinlock_t *spinlock)
{
    return trylock(spinlock);
}

static inline void spin_unlock(struct spinlock_t *spinlock)
{
    cpu_pushcli();

    if (atomic_xchg(&spinlock->lock, 0) == 0)
    {
        panic("spin unlock panic\n");
    }

    cpu_popcli();
}

#endif