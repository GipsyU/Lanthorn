#include <arch/cpu.h>
#include <arch/uart.h>
#include <error.h>
#include <io.h>

#define COM1 0x3f8

int uart_init(void)
{
    char *p;

    // Turn off the FIFO
    outb(0, COM1 + 2);
    // 9600 baud, 8 data bits, 1 stop bit, parity off.
    outb(0x80, COM1 + 3); // Unlock divisor
    outb(115200 / 9600, COM1 + 0);
    outb(0, COM1 + 1);
    outb(0x03, COM1 + 3); // Lock divisor, 8 data bits.
    outb(0, COM1 + 4);
    outb(0x01, COM1 + 1); // Enable receive interrupts.

    // If status is 0xFF, no serial port.
    if (inb(COM1 + 5) == 0xFF) return E_FAULT;

    // Acknowledge pre-existing interrupt conditions;
    // enable interrupts.
    inb(COM1 + 2);
    inb(COM1 + 0);

    for (char *p = "\x1b[2J\x1b[H"; *p; ++p) uart_putc(*p);
}

int uart_putc(char c)
{
    for (int i = 0; i < 128 && !(inb(COM1 + 5) & 0x20); i++) cpu_relax();

    outb(c, COM1 + 0);

    return E_OK;
}

int uart_getc(char *c)
{
    if (!(inb(COM1 + 5) & 0x01)) return E_NOTFOUND;

    *c = inb(COM1 + 0);

    return E_OK;
}