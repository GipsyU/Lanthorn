#ifndef _ARCH_INTR_H_

#define _ARCH_INTR_H_

#define INTR_TIMER 32

#define INTR_KBD 33

int intr_register(int intrno, int (*hdl)(void));

int intr_unregister(int intrno);

#endif