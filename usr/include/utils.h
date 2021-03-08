#ifndef _USR_UTILS_H_
#define _USR_UTILS_H_

#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */

#define container_of(ptr, type, member)                                                                                \
    ({                                                                                                                 \
        const typeof(((type *)0)->member) *__mptr = (ptr);                                                             \
        (type *)((char *)__mptr - offsetof(type, member));                                                             \
    })

#define max(a, b) ({ ((a) > (b) ? (a) : (b)); })

#define min(a, b) ({ ((a) < (b) ? (a) : (b)); })

#define _INTSIZEOF(n) ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))

#define va_start(ap, v) (ap = (char *)&v + _INTSIZEOF(v))

#define va_arg(ap, t) (*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))

#define va_end(ap) (ap = (char *)0)

#endif