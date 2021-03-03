#ifndef _ARCH_ATOMIC_H_
#define _ARCH_ATOMIC_H_

#include <type.h>

#define raw__xchg(x, ptr, size)                                                                                        \
    ({                                                                                                                 \
        __typeof(*(ptr)) __x = (x);                                                                                    \
        switch (size)                                                                                                  \
        {                                                                                                              \
        case 1: {                                                                                                      \
            volatile char *__ptr = (volatile char *)(ptr);                                                                 \
            asm volatile("lock;\nxchgb %0,%1" : "=q"(__x), "+m"(*__ptr) : "0"(__x) : "memory");                        \
            break;                                                                                                     \
        }                                                                                                              \
        case 2: {                                                                                                      \
            volatile short *__ptr = (volatile short *)(ptr);                                                               \
            asm volatile("lock;\nxchgw %0,%1" : "=r"(__x), "+m"(*__ptr) : "0"(__x) : "memory");                        \
            break;                                                                                                     \
        }                                                                                                              \
        case 4: {                                                                                                      \
            volatile uint *__ptr = (volatile uint *)(ptr);                                                               \
            asm volatile("lock;\nxchgl %0,%1" : "=r"(__x), "+m"(*__ptr) : "0"(__x) : "memory");                        \
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
            volatile char *__ptr = (volatile char *)(ptr);                                                                 \
            asm volatile("lock;\ncmpxchgb %2,%1" : "=a"(__ret), "+m"(*__ptr) : "q"(__new), "0"(__old) : "memory");     \
            break;                                                                                                     \
        }                                                                                                              \
        case 2: {                                                                                                      \
            volatile short *__ptr = (volatile short *)(ptr);                                                               \
            asm volatile("lock;\ncmpxchgw %2,%1" : "=a"(__ret), "+m"(*__ptr) : "r"(__new), "0"(__old) : "memory");     \
            break;                                                                                                     \
        }                                                                                                              \
        case 4: {                                                                                                      \
            volatile uint *__ptr = (volatile uint *)(ptr);                                                               \
            asm volatile("lock;\ncmpxchgl %2,%1" : "=a"(__ret), "+m"(*__ptr) : "r"(__new), "0"(__old) : "memory");     \
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
    return (*(int *)&(v)->counter);
}

static inline void atomic_set(struct atomic_t *v, int i)
{
    v->counter = i;
}

static inline void atomic_add(struct atomic_t *v, int i)
{
    asm volatile("lock;\naddl %1,%0" : "+m"(v->counter) : "ir"(i));
}
/**
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic_sub_and_test(struct atomic_t *v, int i)
{
    unsigned char c;

    asm volatile("lock;\nsubl %2,%0; sete %1" : "+m"(v->counter), "=qm"(c) : "ir"(i) : "memory");

    return c;
}

static inline int atomic_cmpxchg(struct atomic_t *v, int old, int new)
{
    return cmpxchg(&v->counter, old, new);
}

static inline int __xchg(volatile int *addr, int newval)
{
    uint result;

    // The + in "+m" denotes a read-modify-write operand.
    asm volatile("lock; xchgl %0, %1" : "+m"(*addr), "=a"(result) : "1"(newval) : "cc");
    return result;
}

static inline int atomic_xchg(struct atomic_t *v, int new)
{
    return __xchg(&v->counter, new);
}

static inline int atomic_add_return(struct atomic_t *v, int i)
{
    int __i;
    __i = i;
    asm volatile("lock;\nxaddl %0, %1" : "+r"(i), "+m"(v->counter) : : "memory");
    return i + __i;
}

/**
 * atomic_add_unless - add unless the number is already a given value
 * @v: pointer of type atomic_t
 * @a: the amount to add to v...
 * @u: ...unless v is equal to u.
 *
 * Atomically adds @a to @v, so long as @v was not already @u.
 * Returns non-zero if @v was not @u, and zero otherwise.
 */
static inline int atomic_add_unless(struct atomic_t *v, int a, int u)
{
    int c, old;
    c = atomic_read(v);
    for (;;)
    {
        if (c == (u)) break;
        old = atomic_cmpxchg((v), c, c + (a));
        if (old == c) break;
        c = old;
    }
    return c != (u);
}

static inline int atomic_add_if(struct atomic_t *v, int a, int i)
{
    int c, old;
    c = atomic_read(v);
    for (;;)
    {
        if (c != (i)) break;
        old = atomic_cmpxchg((v), c, c + (a));
        if (old == c) break;
        c = old;
    }
    return c == (i);
}

#endif