#ifndef _ARCH_PHYOPS_H_
#define _ARCH_PHYOPS_H_
#include <arch/basic.h>

void phyops_memcpy_v2p(addr_t pa, addr_t va, size_t size);

void phyops_memcpy_p2p(addr_t pad, addr_t pas, size_t size);

void phyops_memcpy_p2v(addr_t va, addr_t pa, size_t size);

#endif