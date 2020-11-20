#include <drivers/console.h>
#include <x86.h>

#define DEFAULT_PORT 0x3f8 /* ttyS0 */
#define DEFAULT_BAUD 9600
#define XMTRDY          0x20
#define DLAB		0x80
#define TXR             0       /*  Transmit register (WRITE) */
#define RXR             0       /*  Receive register  (READ)  */
#define IER             1       /*  Interrupt Enable          */
#define IIR             2       /*  Interrupt ID              */
#define FCR             2       /*  FIFO control              */
#define LCR             3       /*  Line control              */
#define MCR             4       /*  Modem control             */
#define LSR             5       /*  Line Status               */
#define MSR             6       /*  Modem Status              */
#define DLL             0       /*  Divisor Latch Low         */
#define DLH             1       /*  Divisor latch High        */


#define XMTRDY          0x20

#define TXR             0       /*  Transmit register (WRITE) */
#define LSR             5       /*  Line Status               */

static void serial_init(int port, int baud)
{
	unsigned char c;
	unsigned divisor;

	outb(0x3, port + LCR);	/* 8n1 */
	outb(0, port + IER);	/* no interrupt */
	outb(0, port + FCR);	/* no fifo */
	outb(0x3, port + MCR);	/* DTR + RTS */

	divisor	= 115200 / baud;
	c = inb(port + LCR);
	outb(c | DLAB, port + LCR);
	outb(divisor & 0xff, port + DLL);
	outb((divisor >> 8) & 0xff, port + DLH);
	outb(c & ~DLAB, port + LCR);
}

static void serial_putchar(char ch)
{
	unsigned timeout = 0xffff;

	while ((inb(DEFAULT_PORT + LSR) & XMTRDY) == 0 && --timeout)
		cpu_relax();

	outb(ch, DEFAULT_PORT + TXR);
}

int console_init(void)
{   
    serial_init(DEFAULT_PORT, DEFAULT_BAUD);
    return 0;
}

int console_put_char(char c)
{   
    serial_putchar(c);
    return 0;
}

