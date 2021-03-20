#include <arch/basic.h>
#include <arch/mmu.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <proc.h>
#include <string.h>
#include <util.h>

#define PDE_PS 128 // Page size
#define PDE_MASK ((((u32)-1) >> 22) << 22)
#define PTE_MASK ((((u32)-1) >> 22) << 12)
#define PXE_ATTR_MASK (((u32)-1) >> 20)
#define NR_PXE PAGE_SIZE / sizeof(addr_t)
#define PDE_IDX(addr) (((addr)&PDE_MASK) >> 22)
#define PTE_IDX(addr) (((addr)&PTE_MASK) >> 12)
#define PDE_OFFSET(addr) (PDE_IDX(addr) * 4)
#define PTE_OFFSET(addr) (PTE_IDX(addr) * 4)

#define DEV_BASE 0xFE000000

__attribute__((__aligned__(PAGE_SIZE))) addr_t volatile PDE[NR_PXE] = {[0] = (0) | PT_P | PT_W | PDE_PS,

                                                                       [KERN_BASE >> 22] = (0) | PT_P | PT_W | PDE_PS};

__attribute__((__aligned__(PAGE_SIZE))) pte_t volatile PTE[CONFIG_NR_BOOT_PTE][NR_PXE];

__attribute__((__aligned__(PAGE_SIZE))) u8 volatile TMP1[PAGE_SIZE / sizeof(u8)];

__attribute__((__aligned__(PAGE_SIZE))) u8 volatile TMP2[PAGE_SIZE / sizeof(u8)];

struct spinlock_t tmp1_lock;

struct spinlock_t tmp2_lock;

void mmu_pde_switch(addr_t pde)
{
    asm volatile("mov %0, %%cr3" : : "r"(pde));
}

static void mmu_reflush(addr_t addr)
{
    asm volatile("invlpg (%0)" ::"a"(addr));
}

void mmu_pm_set(addr_t pa, u32 val)
{
    assert(pa % sizeof(u32) == 0);

    spin_lock(&tmp1_lock);

    PTE[PDE_IDX((addr_t)TMP1 - KERN_BASE)][PTE_IDX((addr_t)TMP1)] = ROUND_DOWN(pa, PAGE_SIZE) | PT_P | PT_W;

    mmu_reflush(TMP1);

    u32 *res = (u32 *)((addr_t)TMP1 + pa % PAGE_SIZE);

    *res = val;

    spin_unlock(&tmp1_lock);
}

void mmu_pp_clean(addr_t pa)
{
    assert(pa % PAGE_SIZE == 0);

    spin_lock(&tmp1_lock);

    PTE[PDE_IDX((addr_t)TMP1 - KERN_BASE)][PTE_IDX((addr_t)TMP1)] = ROUND_DOWN(pa, PAGE_SIZE) | PT_P | PT_W;

    mmu_reflush(TMP1);

    for (uint i = 0; i < PAGE_SIZE; ++i) TMP1[i] = 0;

    spin_unlock(&tmp1_lock);
}

addr_t mmu_get_tmp1(addr_t pa)
{
    assert(pa % PAGE_SIZE == 0);

    spin_lock(&tmp1_lock);

    PTE[PDE_IDX((addr_t)TMP1 - KERN_BASE)][PTE_IDX((addr_t)TMP1)] = ROUND_DOWN(pa, PAGE_SIZE) | PT_P | PT_W;

    mmu_reflush(TMP1);

    return (addr_t)TMP1;
}

addr_t mmu_get_tmp2(addr_t pa)
{
    assert(pa % PAGE_SIZE == 0);

    spin_lock(&tmp2_lock);

    PTE[PDE_IDX((addr_t)TMP2 - KERN_BASE)][PTE_IDX((addr_t)TMP2)] = ROUND_DOWN(pa, PAGE_SIZE) | PT_P | PT_W;

    mmu_reflush(TMP2);

    return (addr_t)TMP2;
}

void mmu_put_tmp1(addr_t tmp)
{
    assert(tmp == (addr_t)TMP1);

    PTE[PDE_IDX((addr_t)TMP1 - KERN_BASE)][PTE_IDX((addr_t)TMP1)] = 0;

    mmu_reflush(TMP1);

    spin_unlock(&tmp1_lock);
}

void mmu_put_tmp2(addr_t tmp)
{
    assert(tmp == (addr_t)TMP2);

    PTE[PDE_IDX((addr_t)TMP2 - KERN_BASE)][PTE_IDX((addr_t)TMP2)] = 0;

    mmu_reflush(TMP2);

    spin_unlock(&tmp2_lock);
}

u32 mmu_pm_get(addr_t pa)
{
    assert(pa % sizeof(u32) == 0);

    spin_lock(&tmp1_lock);

    PTE[PDE_IDX((addr_t)TMP1 - KERN_BASE)][PTE_IDX((addr_t)TMP1)] = ROUND_DOWN(pa, PAGE_SIZE) | PT_P | PT_W;

    mmu_reflush(TMP1);

    u32 *res = (u32 *)((addr_t)TMP1 + pa % PAGE_SIZE);

    spin_unlock(&tmp1_lock);

    return *res;
}

addr_t mmu_get_pde(void)
{
    addr_t pde;

    asm volatile("mov %%cr3, %0" : "=r"(pde));

    return pde;
}

static int enable_4k_page(void)
{
    for (int i = 0; i < CONFIG_NR_BOOT_PTE; ++i)
    {
        for (uint j = 0; j < NR_PXE; ++j)
        {
            PTE[i][j] = (PAGE_SIZE * NR_PXE * i + PAGE_SIZE * j) | PT_P | PT_W;
        }
    }

    for (int i = 0; i < CONFIG_NR_BOOT_PTE; ++i)
    {
        PDE[PDE_IDX(KERN_BASE + PAGE_SIZE * NR_PXE * i)] = ((addr_t)PTE[i] - KERN_BASE) | PT_P | PT_W;
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

static int map_dev(void)
{
    for (addr_t addr = DEV_BASE; addr != 0; addr += NR_PXE * PAGE_SIZE)
    {
        PDE[addr >> 22] = addr | PT_P | PT_W | PDE_PS;
    }

    mmu_reflush(DEV_BASE);

    return E_OK;
}

int mmu_init(addr_t *pde)
{
    spin_init(&tmp1_lock);

    spin_init(&tmp2_lock);

    int err = enable_4k_page();

    if (err != E_OK) return err;

    // err = disable_low_map();

    if (err != E_OK) return err;

    err = map_dev();

    if (err != E_OK) return err;

    *pde = (addr_t)PDE - KERN_BASE;

    return err;
}

/**
 * QUESTION:REFLASH
 */

int mmu_map(addr_t pde, addr_t va, addr_t pa, uint usr, uint wtb)
{
    assert(pde % PAGE_SIZE == 0 && va % PAGE_SIZE == 0 && pa % PAGE_SIZE == 0);

    addr_t pte = mmu_pm_get(pde + PDE_OFFSET(va));

    if ((pte & PT_P) == 0) return E_NOTFOUND;

    pte = pte & (~PXE_ATTR_MASK);

    addr_t pg = mmu_pm_get(pte + PTE_OFFSET(va));

    if ((pg & PT_P) != 0) return E_EXIST;

    mmu_pm_set(pte + PTE_OFFSET(va), pa | PT_P | (usr ? PT_U : NULL) | (wtb ? PT_W : NULL));

    mmu_reflush(va);

    return E_OK;
}

int mmu_unmap(addr_t pde, addr_t va)
{
    assert(pde % PAGE_SIZE == 0 && va % PAGE_SIZE == 0);

    addr_t pte = mmu_pm_get(pde + PDE_OFFSET(va));

    if ((pte & PT_P) == 0) return E_NOTFOUND;

    pte = pte & (~PXE_ATTR_MASK);

    addr_t pg = mmu_pm_get(pte + PTE_OFFSET(va));

    if ((pg & PT_P) == 0) return E_NOTFOUND;

    mmu_pm_set(pte + PTE_OFFSET(va), 0);

    mmu_reflush(va);

    return E_OK;
}

void mmu_sync_kern_space(addr_t kpde, addr_t pde, addr_t addr, size_t size)
{
    assert(addr >= KERN_BASE && !(addr + size > 0 && addr + size < KERN_BASE));

    for (addr_t offset = 0; offset < size; offset += PAGE_SIZE * PAGE_SIZE / sizeof(addr_t))
    {
        mmu_pm_set(pde + PDE_OFFSET(addr + offset), mmu_pm_get(kpde + PDE_OFFSET(addr + offset)));
    }
}

int mmu_v2p(addr_t pde, addr_t v, addr_t *p)
{
    int err = E_OK;

    addr_t pte = mmu_pm_get(pde + PDE_OFFSET(v));

    if ((pte & PT_P) == 0) return E_NOTFOUND;

    pte = pte & (~PXE_ATTR_MASK);

    addr_t pa = mmu_pm_get(pte + PTE_OFFSET(v));

    if ((pa & PT_P) == 0) return E_NOTFOUND;

    *p = pa & (~PXE_ATTR_MASK);

    return err;
}

addr_t mmu_err_addr(void)
{
    addr_t addr;

    asm volatile("movl %%cr2, %0" : "=a"(addr));

    return addr;
}

static uint mmu_pte_exist(addr_t pde, addr_t va)
{
    assert(pde % PAGE_SIZE == 0);

    return (mmu_pm_get(pde + PDE_OFFSET(va)) & PT_P) != 0;
}

int mmu_pte_get(addr_t pde, addr_t va, addr_t *pte)
{
    assert(pde % PAGE_SIZE == 0);

    addr_t _pte = mmu_pm_get(pde + PDE_OFFSET(va));

    if ((_pte & PT_P) == 0)
        return E_NOTFOUND;

    else
        *pte = _pte & (~PXE_ATTR_MASK);

    return E_OK;
}

void mmu_pte_set(addr_t pde, addr_t va, addr_t pte, uint usr, uint wtb)
{
    assert(mmu_pte_exist(pde, va) == 0);

    mmu_pm_set(pde + PDE_OFFSET(va), pte | PT_P | (usr ? PT_U : NULL) | (wtb ? PT_W : NULL));
}