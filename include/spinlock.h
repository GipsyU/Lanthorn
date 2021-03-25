#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <arch/atomic.h>
#include <arch/cpu.h>
#include <arch/intr.h>
#include <log.h>

struct spinlock_t
{
    struct atomic_t ticket_get;
    struct atomic_t ticket_now;
    struct thread_t *owner;
};

struct spin_rwlock_t
{
    struct atomic_t lock;
};

static inline void spin_init(struct spinlock_t *spinlock)
{
    atomic_set(&spinlock->ticket_get, 0);

    atomic_set(&spinlock->ticket_now, 0);

    spinlock->owner = NULL;
}

static inline void spin_lock(struct spinlock_t *spinlock)
{
    barrier();

    int ticket = atomic_add_return(&spinlock->ticket_get, 1) - 1;

    while (atomic_read(&spinlock->ticket_now) != ticket) cpu_relax();

    barrier();

    spinlock->owner = thread_now();

    barrier();
}

static inline void spin_unlock(struct spinlock_t *spinlock)
{
    barrier();

    atomic_add(&spinlock->ticket_now, 1);
    
    barrier();

    spinlock->owner = NULL;

    barrier();
}

static inline void spin_lock_irqsave(volatile struct spinlock_t *spinlock)
{
    intr_irq_save();

    int ticket = atomic_add_return(&spinlock->ticket_get, 1) - 1;

    while (atomic_read(&spinlock->ticket_now) != ticket) cpu_relax();

    barrier();

    spinlock->owner = thread_now();
}

static inline void spin_unlock_irqrestore(struct spinlock_t *spinlock)
{
    barrier();

    atomic_add(&spinlock->ticket_now, 1);
    
    barrier();

    spinlock->owner = NULL;

    intr_irq_restore();
}

// static inline int spin_trylock(volatile struct spinlock_t *spinlock)
// {
//     barrier();
    

//     // return atomic_xchg(&spinlock->lock, 1) == 0;
// }



static inline void spin_rwlock_init(struct spin_rwlock_t *lock)
{
    atomic_set(&lock->lock, 0);
}

static inline void spin_read_lock(struct spin_rwlock_t *lock)
{
    while (atomic_add_unless(&lock->lock, 1, -1) == 0) cpu_relax();

    assert(atomic_read(&lock->lock) > 0);
}

static inline void spin_write_lock(struct spin_rwlock_t *lock)
{
    while (atomic_add_if(&lock->lock, -1, 0) == 0) cpu_relax();

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