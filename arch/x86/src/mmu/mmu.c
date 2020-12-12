#include <arch/basic.h>
#include <arch/mmu.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <proc.h>
#include <string.h>

#define PXE_P 1    // Present
#define PXE_W 2    // Writeable
#define PXE_U 4    // User access
#define PDE_PS 128 // Page size
#define PDE_MASK ((((u32)-1) >> 22) << 22)
#define PTE_MASK ((((u32)-1) >> 22) << 12)
#define PXE_ATTR_MASK (((u32)-1) >> 20)
#define NR_PXE PAGE_SIZE / sizeof(addr_t)
#define PDE_IDX(addr) (((addr)&PDE_MASK) >> 22)
#define PTE_IDX(addr) (((addr)&PTE_MASK) >> 12)

#define DEV_BASE 0xFE000000

__attribute__((__aligned__(PAGE_SIZE))) addr_t volatile PDE[NR_PXE] = {[0] = (0) | PXE_P | PXE_W | PDE_PS,

                                                                      [KERN_BASE >> 22] = (0) | PXE_P | PXE_W | PDE_PS};

__attribute__((__aligned__(PAGE_SIZE))) pte_t volatile PTE[CONFIG_NR_BOOT_PTE][NR_PXE];

__attribute__((__aligned__(PAGE_SIZE))) pxe_t volatile TMP[NR_PXE]; // virtual memory sapce

/**
 * FIXME
 */

int mmu_pde_switch(addr_t pde)
{
    asm volatile("mov %0, %%cr3" : : "r"(pde));

    return E_OK;
}

static int enable_4k_page(void)
{
    for (int i = 0; i < CONFIG_NR_BOOT_PTE; ++i)
    {
        for (uint j = 0; j < NR_PXE; ++j)
        {
            PTE[i][j] = (PAGE_SIZE * NR_PXE * i + PAGE_SIZE * j) | PXE_P | PXE_W;
        }
    }

    for (int i = 0; i < CONFIG_NR_BOOT_PTE; ++i)
    {
        PDE[PDE_IDX(KERN_BASE + PAGE_SIZE * NR_PXE * i)] = ((addr_t)PTE[i] - KERN_BASE) | PXE_P | PXE_W;
    }

    mmu_pde_switch((addr_t)PDE - KERN_BASE);

    return E_OK;
}

static int disable_low_map(void)
{
    PDE[0] = 0;

    mmu_pde_switch((addr_t)PDE - KERN_BASE);

    return E_OK;
}

int map_dev(void)
{
    for (addr_t addr = DEV_BASE; addr != 0; addr += NR_PXE * PAGE_SIZE)
    {
        PDE[addr >> 22] = addr | PXE_P | PXE_W | PDE_PS;
    }

    mmu_pde_switch((addr_t)PDE - KERN_BASE);

    return E_OK;
}

int mmu_init(addr_t *pde)
{
    int err = E_OK;

    err = enable_4k_page();

    if (err != E_OK) return err;

    err = disable_low_map();

    if (err != E_OK) return err;

    err = map_dev();

    if (err != E_OK) return err;

    *pde = (addr_t)PDE;

    return err;
}

int mmu_kern_map(addr_t pp, addr_t vp)
{
    int err = E_OK;

    info("mmu kern map %p to %p.\n", pp, vp);

    if (pp % PAGE_SIZE != 0 || vp % PAGE_SIZE != 0) return E_INVAL;

    if (vp < KERN_BASE) return E_INVAL;

    if (vp >= KERN_BASE && vp < KERN_BASE + CONFIG_NR_BOOT_PTE * NR_PXE * PAGE_SIZE)
    {
        assert(vp == (addr_t)TMP, "mmu bug.\n");

        PTE[PDE_IDX(vp - KERN_BASE)][PTE_IDX(vp)] = pp | PXE_P | PXE_W;

        /**
         * FIXME: FLUSH
         */

        mmu_pde_switch((addr_t)PDE - KERN_BASE);

        return E_OK;
    }

    addr_t pte;

    if ((PDE[PDE_IDX(vp)] & PXE_P) == 0)
    {
        struct page_t *ptep;

        err = page_alloc(&ptep);

        if (err != E_OK) return err;

        page_get(ptep);

        PDE[PDE_IDX(vp)] = ptep->addr | PXE_P | PXE_W;

        pte = ptep->addr;
    }
    else
    {
        pte = PDE[PDE_IDX(vp)] & (~PXE_ATTR_MASK);
    }

    mmu_kern_map(pte, (addr_t)TMP);

    TMP[PTE_IDX(vp)] = pp | PXE_P | PXE_W;

    /**
     * FIXME: FLAUSH
     */
    mmu_pde_switch((addr_t)PDE - KERN_BASE);

    return err;
}

// int mmu_kern_unmap(addr_t vp)
// {
//     int err = E_OK;

//     debug("mmu kern unmap %p.\n", vp);

//     if (vp < KERN_BASE) return E_INVAL;

//     if (vp >= KERN_BASE && vp < KERN_BASE + CONFIG_NR_BOOT_PTE * NR_PXE * PAGE_SIZE)
//     {
//         assert(vp == (addr_t)TMP, "mmu bug.\n");

//         PTE[PDE_IDX(vp - KERN_BASE)][PTE_IDX(vp)] = pp | PXE_P | PXE_W;

//         mmu_pde_switch((addr_t)PDE - KERN_BASE);

//         return E_OK;
//     }

//     assert(PDE[PDE_IDX(vp)] & PXE_P) != 0, "mmu bug!\n");

//     addr_t pte = PDE[PDE_IDX(vp)] & (~PXE_ATTR_MASK);

//     mmu_kern_map(_pte, (addr_t)TMP, 0);

//     TMP[PTE_IDX(vp)] = pp | PXE_P | PXE_W;

//     mmu_pde_switch((addr_t)PDE - KERN_BASE);

//     return err;
// }

int mmu_user_map(addr_t pde, addr_t pp, addr_t vp, addr_t pte)
{
    int err = E_OK;

    if (pde % PAGE_SIZE != 0 || pp % PAGE_SIZE != 0 || vp % PAGE_SIZE != 0)
    {
        return E_INVAL;
    }

    if (vp >= KERN_BASE)
    {
        return E_INVAL;
    }

    mmu_kern_map(pde, (addr_t)TMP);

    addr_t _pte;

    if ((TMP[PDE_IDX(vp)] & PXE_P) == 0)
    {
        if (pte == NULL)
        {
            return E_AGAIN;
        }

        TMP[PDE_IDX(vp)] = pte | PXE_P | PXE_W;

        _pte = pte;
    }
    else
    {
        /*
         * FIXME
         */
        _pte = TMP[PDE_IDX(vp)] & (~PXE_ATTR_MASK);
    }

    mmu_kern_map(_pte, (addr_t)TMP);

    TMP[PTE_IDX(vp)] = pp | PXE_P | PXE_W;

    return err;
}

int mmu_sync_kern_space(addr_t pde, addr_t addr)
{
    int err = mmu_kern_map(pde, (addr_t)TMP);

    if (err != E_OK) return err;

    TMP[PDE_IDX(addr)] = PDE[PDE_IDX(addr)];

    return err;
}

int mmu_v2p(addr_t pde, addr_t v, addr_t *p)
{
    int err = E_OK;

    err = mmu_kern_map(pde, (addr_t)TMP);

    if (err != E_OK)
    {
        return err;
    }

    if ((TMP[PDE_IDX(v)] & PXE_P) == 0)
    {
        return E_NOTFOUND;
    }

    addr_t pte = TMP[PDE_IDX(v)] & (~PXE_ATTR_MASK);

    err = mmu_kern_map(pte, (addr_t)TMP);

    if (err != E_OK)
    {
        return err;
    }

    if ((TMP[PTE_IDX(v)] & PXE_P) == 0)
    {
        return E_NOTFOUND;
    }

    *p = TMP[PTE_IDX(v)] & (~PXE_ATTR_MASK);

    return err;
}