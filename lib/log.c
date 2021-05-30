#include <arch/basic.h>
#include <arch/uart.h>
#include <log.h>
#include <spinlock.h>
#include <util.h>

struct spinlock_t log_lock;

int log_on = 1;

static void itoa(int x, int redix)
{
    if (x == 0)
    {
        uart_putc('0');

        return;
    }

    if (x < 0)
    {
        uart_putc('-');

        x = -x;
    }

    char s[15];

    char *p = s;

    for (; x > 0; x /= redix, ++p) *p = "0123456789abcdef"[x % redix];

    for (--p; p >= s; --p) uart_putc(*p);

    return;
}

static void uitoa(u32_t x, int redix)
{
    if (x == 0)
    {
        uart_putc('0');

        return;
    }

    char s[15];

    char *p = s;

    for (; x > 0; x /= redix, ++p) *p = "0123456789abcdef"[x % redix];

    for (--p; p >= s; --p) uart_putc(*p);

    return;
}

static void console_put_str(const char *s)
{
    for (; *s; s++) uart_putc(*s);

    return;
}

void printk(const char *fmt, ...)
{
    char *args;

    va_start(args, fmt);

    for (; *fmt; ++fmt)
    {
        if (*fmt != '%')
        {
            uart_putc(*fmt);

            continue;
        }

        ++fmt;

        switch (*fmt)
        {
        case 'o':

            itoa(va_arg(args, int), 8);

            break;

        case 'd':

            itoa(va_arg(args, int), 10);

            break;

        case 'x':

            itoa(va_arg(args, int), 16);

            break;

        case 'p':

            uitoa(va_arg(args, addr_t), 16);

            break;

        case 'c':

            uart_putc(va_arg(args, char));

            break;

        case 's':

            console_put_str(va_arg(args, char *));

            break;

        case '%':

            uart_putc('%');

            break;

        default:

            break;
        }
    }

    va_end(args);

    return;
}

void print_regs(void)
{   
    u32_t eip, ebp, esp, eax, ebx, ecx, edx;
    
    asm volatile ("movl %%ebp, %0" : "=r" (ebp));
    
    asm volatile ("movl %%esp, %0" : "=r" (esp));
    
    asm volatile ("movl %%eax, %0" : "=r" (eax));
    
    asm volatile ("movl %%ebx, %0" : "=r" (ebx));
    
    asm volatile ("movl %%ecx, %0" : "=r" (ecx));
    
    asm volatile ("movl %%edx, %0" : "=r" (edx));
    
    printk("ebp: %p, esp: %p, eax: %p, ebx: %p, ecx: %p, edx: %p.\n", ebp, esp, eax, ebx, ecx, edx);
}
