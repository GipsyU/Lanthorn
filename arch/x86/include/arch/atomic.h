#ifndef _ARCH_ATOMIC_H_
#define _ARCH_ATOMIC_H_

#include <arch/basic.h>

#define raw__xchg(x, ptr, size)                                                                                        \
    ({                                                                                                                 \
        __typeof(*(ptr)) __x = (x);                                                                                    \
        switch (size)                                                                                                  \
        {                                                                                                              \
        case 1: {                                                                                                      \
            volatile u8 *__ptr = (volatile u8 *)(ptr);                                                                 \
            asm volatile("xchgb %0,%1" : "=q"(__x), "+m"(*__ptr) : "0"(__x) : "memory");                               \
            break;                                                                                                     \
        }                                                                                                              \
        case 2: {                                                                                                      \
            volatile u16 *__ptr = (volatile u16 *)(ptr);                                                               \
            asm volatile("xchgw %0,%1" : "=r"(__x), "+m"(*__ptr) : "0"(__x) : "memory");                               \
            break;                                                                                                     \
        }                                                                                                              \
        case 4: {                                                                                                      \
            volatile u32 *__ptr = (volatile u32 *)(ptr);                                                               \
            asm volatile("xchgl %0,%1" : "=r"(__x), "+m"(*__ptr) : "0"(__x) : "memory");                               \
            break;                                                                                                     \
        }                                                                                                              \
        }                                                                                                              \
        __x;                                                                                                           \
    })

#define xchg(ptr, v) raw__xchg((v), (ptr), sizeof(*ptr))

#define raw_cmpxchg(ptr, old, new, size)                                                                               \
    ({                                                                                                                 \
        __typeof__(*(ptr)) __ret;                                                                                      \
        __typeof__(*(ptr)) __old = (old);                                                                              \
        __typeof__(*(ptr)) __new = (new);                                                                              \
        switch (size)                                                                                                  \
        {                                                                                                              \
        case 1: {                                                                                                      \
            volatile u8 *__ptr = (volatile u8 *)(ptr);                                                                 \
            asm volatile("cmpxchgb %2,%1" : "=a"(__ret), "+m"(*__ptr) : "q"(__new), "0"(__old) : "memory");            \
            break;                                                                                                     \
        }                                                                                                              \
        case 2: {                                                                                                      \
            volatile u16 *__ptr = (volatile u16 *)(ptr);                                                               \
            asm volatile("cmpxchgw %2,%1" : "=a"(__ret), "+m"(*__ptr) : "r"(__new), "0"(__old) : "memory");            \
            break;                                                                                                     \
        }                                                                                                              \
        case 4: {                                                                                                      \
            volatile u32 *__ptr = (volatile u32 *)(ptr);                                                               \
            asm volatile("cmpxchgl %2,%1" : "=a"(__ret), "+m"(*__ptr) : "r"(__new), "0"(__old) : "memory");            \
            break;                                                                                                     \
        }                                                                                                              \
        }                                                                                                              \
        __ret;                                                                                                         \
    })

#define cmpxchg(ptr, old, new) raw_cmpxchg((ptr), (old), (new), sizeof(*ptr))

struct atomic_t
{
    int counter;
};

#define ATOMIC_INIT(i)                                                                                                 \
    {                                                                                                                  \
        (i)                                                                                                            \
    }

static inline int atomic_read(const struct atomic_t *v)
{
    return (*(volatile int *)&(v)->counter);
}

static inline void atomic_set(struct atomic_t *v, int i)
{
    v->counter = i;
}

static inline void atomic_add(struct atomic_t *v, int i)
{
    asm volatile("addl %1,%0" : "+m"(v->counter) : "ir"(i));
}

static inline void atomic_sub(struct atomic_t *v, int i)
{
    asm volatile("subl %1,%0" : "+m"(v->counter) : "ir"(i));
}

/**
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic_sub_and_test(struct atomic_t *v, int i)
{
    unsigned char c;

    asm volatile("subl %2,%0; sete %1" : "+m"(v->counter), "=qm"(c) : "ir"(i) : "memory");
    
    return c;
}

static inline int atomic_cmpxchg(struct atomic_t *v, int old, int new)
{
    return cmpxchg(&v->counter, old, new);
}

static inline int atomic_xchg(struct atomic_t *v, int new)
{
    return xchg(&v->counter, new);
}

#endif