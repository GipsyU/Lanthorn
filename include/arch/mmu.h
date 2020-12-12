#ifndef _ARCH_MMU_H_
#define _ARCH_MMU_H_
#include <arch/basic.h>

#define PF_P 1

int mmu_kern_map(addr_t pp, addr_t vp);

int mmu_user_map(addr_t pde, addr_t pp, addr_t vp, addr_t pte);

int mmu_sync_kern_space(addr_t pde, addr_t addr);

void mmu_pde_switch(addr_t pde);

int mmu_v2p(addr_t pde, addr_t v, addr_t *p);

addr_t mmu_get_pde(void);

addr_t mmu_err_addr(void);

#endif