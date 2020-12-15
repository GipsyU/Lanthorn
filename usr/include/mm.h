#ifndef _USR_MM_H_
#define _USR_MM_H_
#include <type.h>

int malloc(addr_t *addr, size_t size);

int mfree(addr_t addr);

#endif