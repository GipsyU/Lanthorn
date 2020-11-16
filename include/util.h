#ifndef _UTIL_H_
#define _UTIL_H_

#define ROUND_UP(x, y) (          \
    {                             \
        const typeof(y) _y = (y); \
        ((x) + _y - 1) / _y *_y;  \
    })

#define ROUND_DOWN(x, y) (        \
    {                             \
        const typeof(y) _y = (y); \
        (x) / _y *_y;             \
    })

#define offset(p, t) (           \
    {                            \
        (addr_t)(p) + sizeof(t); \
    })

#endif