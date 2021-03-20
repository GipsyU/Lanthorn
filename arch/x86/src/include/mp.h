#ifndef _X86_MP_H_
#define _X86_MP_H_
#include <arch/basic.h>

void lapic_startap(int apicid, addr_t addr);

#endif