#include <basic.h>
#include <arch/mmu.h>
#include <log.h>
#include <error.h>
#include <memory.h>
#include <proc.h>

#define PXE_P 1    // Present
#define PXE_W 2    // Writeable
#define PXE_U 4    // User access
#define PDE_PS 128 // Page size
#define PDE_MASK ((((u32)-1) >> 22) << 22)
#define PTE_MASK ((((u32)-1) >> 22) << 12)
#define PXE_ATTR_MASK (((u32)-1) >> 20)
#define NR_PXE PAGE_SIZE / sizeof(pde_t)
#define PDE_IDX(addr) (((addr) & PDE_MASK) >> 22)
#define PTE_IDX(addr) (((addr) & PTE_MASK) >> 12)

#define DEV_BASE 0xFE000000


__attribute__((__aligned__(PAGE_SIZE))) pde_t PDE[NR_PXE] =
    {
        [0] = (0) | PXE_P | PXE_W | PDE_PS,

        [KERN_BASE >> 22] = (0) | PXE_P | PXE_W | PDE_PS};

__attribute__((__aligned__(PAGE_SIZE))) pte_t PTE[CONFIG_NR_BOOT_PTE][NR_PXE];

__attribute__((__aligned__(PAGE_SIZE))) pxe_t TMP[NR_PXE]; // virtual memory sapce

/**
 * FIXME
 */

static int enable_4k_page(void)
{
    for (int i = 0; i < CONFIG_NR_BOOT_PTE; ++i)
    {
        for (int j = 0; j < NR_PXE; ++j)
        {
            PTE[i][j] = (PAGE_SIZE * NR_PXE * i + PAGE_SIZE * j) | PXE_P | PXE_W;
        }
    }

    PDE[0] = 0;

    for (int i = 0; i < CONFIG_NR_BOOT_PTE; ++i)
    {
        PDE[PDE_IDX(KERN_BASE + PAGE_SIZE * NR_PXE * i)] = ((addr_t)PTE[i] - KERN_BASE) | PXE_P | PXE_W;
    }

    asm volatile("mov %0, %%cr3"
                 :
                 : "r"((addr_t)PDE - KERN_BASE));

    return E_OK;
}

static int disable_low_map(void)
{
    PDE[0] = 0;

    return E_OK;
} 

int map_dev(void)
{
    for (addr_t addr = DEV_BASE; addr != 0; addr += NR_PXE * PAGE_SIZE)
    {
        PDE[addr >> 22] = addr | PXE_P | PXE_W | PDE_PS;
    }

    return E_OK;
}

int mmu_init(void)
{
    int err = E_OK;

    err = enable_4k_page();

    if (err != E_OK)
    {
        return err;
    }

    err = disable_low_map();

    if (err != E_OK)
    {
        return err;
    }

    err = map_dev();

    if(err != E_OK)
    {
        return err;
    }

    return err;

}

/*
 * FIXME: just sup kernel space map
 */

int mmu_map(addr_t pp, addr_t vp, addr_t pte)
{
    int err = E_OK;

    if (pp % PAGE_SIZE != 0 || vp % PAGE_SIZE != 0)
    {
        return E_INVAL;
    }

    if (vp < KERN_BASE)
    {
        return E_INVAL;
    }
    
    if (vp < KERN_BASE + CONFIG_NR_BOOT_PTE * NR_PXE * PAGE_SIZE)
    {
        assert(vp == (addr_t)TMP, "mmu bug.\n");

        PTE[PDE_IDX(vp - KERN_BASE)][PTE_IDX(vp)] = pp | PXE_P | PXE_W;
            
        return E_OK;
    }

    addr_t _pte;

    if ((PDE[PDE_IDX(vp)] & PXE_P) == 0)
    {
        debug("%p %p %p\n", pp, vp, pte);

        PDE[PDE_IDX(vp)] = pte | PXE_P | PXE_W;

        _pte = pte;
    }
    else
    {
        /*
         * FIXME
         */
        debug("%p %p\n", pp, vp);

        _pte = PDE[PDE_IDX(vp)] & (~PXE_ATTR_MASK);
    }

    mmu_map(_pte, (addr_t)TMP, 0);

    TMP[PTE_IDX(vp)] = pp | PXE_P | PXE_W;

    return err;
}