#ifndef _DRIVES_INTR_H_
#define _DRIVES_INTR_H_

int intr_register(int cpuid, int intrid, void* hdl);

int intr_unregister(int cpuid, int intrid);

int intr_init();

#endif