#ifndef _ARCH_INTR_H_
#define _ARCH_INTR_H_
#include <arch/basic.h>

int intr_register(int intrno, int (*hdl)(uint));

int intr_unregister(int intrno);

int intr_enable(void);

int intr_disable(void);

void intr_end(void);

#endif