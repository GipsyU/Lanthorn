#ifndef _ARCH_MMU_H_
#define _ARCH_MMU_H_
#include <arch/basic.h>

#define PT_P 1 // Present
#define PT_W 2 // Writeable
#define PT_U 4 // User access
#define PF_P 1
#define PF_W 2


void mmu_pp_clean(addr_t pa);

void mmu_pm_set(addr_t pa, u32 val);

u32 mmu_pm_get(addr_t pa);

int mmu_map(addr_t pde, addr_t va, addr_t pa, uint usr, uint wtb);

int mmu_unmap(addr_t pde, addr_t va);

void mmu_sync_kern_space(addr_t kpde, addr_t pde, addr_t addr);

void mmu_pde_switch(addr_t pde);

int mmu_v2p(addr_t pde, addr_t v, addr_t *p);

addr_t mmu_get_pde(void);

addr_t mmu_err_addr(void);

void mmu_pte_set(addr_t pde, addr_t va, addr_t ptepa, uint usr, uint wtb);

int mmu_pte_get(addr_t pde, addr_t va, addr_t *pte);

addr_t mmu_get_tmp1(addr_t pa);

addr_t mmu_get_tmp2(addr_t pa);

void mmu_put_tmp1(addr_t tmp);

void mmu_put_tmp2(addr_t tmp);

#endif