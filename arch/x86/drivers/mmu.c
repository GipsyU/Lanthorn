#include <basic.h>
#include <drivers/mmu.h>
#include <log.h>
#include <error.h>

#define PXE_P   1   // Present
#define PXE_W   2   // Writeable
#define PXE_U   4   // User access
#define PDE_PS  128 // Page size
#define PXENR   PAGE_SIZE / sizeof(pde_t)

__attribute__((__aligned__(PAGE_SIZE))) pde_t PDE[PXENR] = {
    [0] = (0) | PXE_P | PXE_W | PDE_PS,
    [KERN_BASE >> 22] = (0) | PXE_P | PXE_W | PDE_PS
};

__attribute__((__aligned__(PAGE_SIZE))) pte_t PTE0[PXENR];

int mmu_enable_4k_page(void)
{
    for (int i = 0; i < PXENR; ++i) {
        PTE0[i] = (PAGE_SIZE * i) | PXE_P | PXE_W;
    }
    PDE[0] = 0;
    PDE[KERN_BASE >> 22] = ((addr_t)PTE0-KERN_BASE) | PXE_P | PXE_W;
    asm volatile("mov %0, %%cr3" : : "r"((addr_t)PDE - KERN_BASE));
    return E_OK;
}

