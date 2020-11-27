#ifndef _ARCH_MMU_H_

#define _ARCH_MMU_H_

#define MMU_PTE_USED 1

#define MMU_PTE_UNUSED 2

#define mmu_kern_map_STATE_MASK 3

#include <basic.h>

int mmu_kern_map(addr_t pp, addr_t vp, addr_t pte);

int mmu_user_map(addr_t pde, addr_t pp, addr_t vp, addr_t pte);

int mmu_pde_init(addr_t pde);

int mmu_pde_switch(addr_t pde);

int mmu_v2p(addr_t pde, addr_t v, addr_t *p);

#endif