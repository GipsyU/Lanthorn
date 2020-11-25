#ifndef _ARCH_INTR_H_

#define _ARCH_INTR_H_

int intr_register(int intrno, int (*hdl)(void));

int intr_unregister(int intrno);

void intr_end(void);

#endif