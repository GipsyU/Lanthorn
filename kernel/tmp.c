#include <arch/atomic.h>
#include <arch/intr.h>
#include <arch/uart.h>
#include <buf.h>
#include <error.h>
#include <spinlock.h>
#include <syscall.h>

struct buf_t buf;

int tmp_write(char *s)
{
    print("%s", s);

    return E_OK;
}

int tmp_read(char *s)
{
    char c;

    while (buf_read(&buf, &c) == E_OK)
    {
        if (c != '\n')
            *s++ = c;

        else
        {
            *s++ = '\0';

            return E_OK;
        }
    }

    panic("bug.\n");
}

#define C(x) ((x) - '@')

int com_intr(uint unused)
{
    char c;

    while (uart_getc(&c) == E_OK)
    {
        if (c == '\r') c = '\n';

        buf_write(&buf, c);

        uart_putc(c);
    }

    return E_OK;
}

int tmp_init(void)
{
    int err = syscall_register(SYS_write, tmp_write, 1);

    if (err != E_OK) return err;

    err = syscall_register(SYS_read, tmp_read, 1);

    if (err != E_OK) return err;

    err = intr_register(INTR_COM1, com_intr);

    if (err != E_OK) return err;

    buf_init(&buf, PAGE_SIZE);

    return err;
}