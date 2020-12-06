#include <spinlock.h>
#include <error.h>
#include <arch/intr.h>
#include <log.h>

#define try_lock(spinlock)                               \
    ({                                                   \
        intr_disable();                                  \
        atomic_read(&(spinlock)->lock) == 0 ? 1 : ({ intr_enable(); 0; }); \
    })

static inline void lock(struct spinlock_t *spinlock)
{
    while (atomic_cmpxchg(&spinlock->lock, 0, 1) != 0)
    {
        asm volatile("rep");
    }
}

int spin_init(struct spinlock_t *spinlock)
{
    atomic_set(&spinlock->lock, 0);

    return E_OK;
}

int spin_lock(struct spinlock_t *spinlock)
{
    lock(spinlock);

    return E_OK;
}

int spin_unlock(struct spinlock_t *spinlock)
{
    if (atomic_read(&spinlock->lock) == 0)
    {
        panic("spin unlock panic\n");
    }

    atomic_set(&spinlock->lock, 0);

    return E_OK;
}