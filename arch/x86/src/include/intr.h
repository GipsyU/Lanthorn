#ifndef _X86_SRC_INTR_H_
#define _X86_SRC_INTR_H_

#define IRQ_BASE 32
#define IRQ_TIMER (IRQ_BASE + 0)
#define IRQ_KBD (IRQ_BASE + 1)
#define IRQ_COM1 (IRQ_BASE + 4)
#define IRQ_IDE (IRQ_BASE + 14)
#define IRQ_ERROR (IRQ_BASE + 19)
#define IRQ_MAX (IRQ_BASE + 31)

#endif