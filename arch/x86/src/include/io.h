#ifndef _X86_SRC_IO_H_
#define _X86_SRC_IO_H_
#include <arch/basic.h>
static inline void outb(u8_t v, u16_t port)
{
    asm volatile("outb %0,%1"
                 :
                 : "a"(v), "dN"(port));
}

static inline u8_t inb(u16_t port)
{
    u8_t v;
    asm volatile("inb %1,%0"
                 : "=a"(v)
                 : "dN"(port));
    return v;
}

static inline void outw(u16_t v, u16_t port)
{
    asm volatile("outw %0,%1"
                 :
                 : "a"(v), "dN"(port));
}

static inline u16_t inw(u16_t port)
{
    u16_t v;
    asm volatile("inw %1,%0"
                 : "=a"(v)
                 : "dN"(port));
    return v;
}

static inline void outl(u32_t v, u16_t port)
{
    asm volatile("outl %0,%1"
                 :
                 : "a"(v), "dN"(port));
}

static inline u32_t inl(u32_t port)
{
    u32_t v;
    asm volatile("inl %1,%0"
                 : "=a"(v)
                 : "dN"(port));
    return v;
}
#endif