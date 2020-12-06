#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <arch/atomic.h>

struct spinlock_t
{
    struct atomic_t lock;
};

int spin_init(struct spinlock_t *spinlock);

int spin_lock(struct spinlock_t *spinlock);

int spin_unlock(struct spinlock_t *spinlock);

#endif