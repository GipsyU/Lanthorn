#ifndef _ARCH_MMU_H_

#define _ARCH_MMU_H_

#define MMU_PTE_USED 1

#define MMU_PTE_UNUSED 2

#define MMU_MAP_STATE_MASK 3

#include <basic.h>

int mmu_map(addr_t pp, addr_t vp, addr_t pte);

// int mmu_unmap();

#endif