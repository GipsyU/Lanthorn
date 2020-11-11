#include <basic.h>

#define PDE_P   1   // Present
#define PDE_W   2   // Writeable
#define PTE_U   4   // User access
#define PTE_PS  128 // Page size
__attribute__((__aligned__(PAGE_SIZE))) pde_t PDE[PAGE_SIZE/sizeof(pde_t)] = {
    [0] = (0) | PDE_P | PDE_W | PTE_PS,
    [KERN_BASE>>22] = (0) | PDE_P | PDE_W | PTE_PS
};
// __attribute__((__aligned__(PAGE_SIZE))) pte_t PTE0[PAGE_SIZE/sizeof(pte_t)];