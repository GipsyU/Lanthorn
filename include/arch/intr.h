#ifndef _ARCH_INTR_H_
#define _ARCH_INTR_H_
#include <arch/basic.h>

#define NR_INTR 256
#define INTR_PGFAULT 14
#define INTR_TIMER 32
#define INTR_KBD 33
#define INTR_COM1 36
#define INTR_SYSCALL 128

int intr_register(int intrno, int (*hdl)(uint));

int intr_unregister(int intrno);

void intr_irq_enable(void);

void intr_irq_disable(void);

void intr_irq_restore(void);

void intr_irq_save(void);

int intr_irq_state(void);

void intr_end(void);

#endif