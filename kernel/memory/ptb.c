#include <arch/mmu.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <proc.h>
#include <string.h>

// struct ptb_t
// {
//     addr_t pde;
//     struct spinlock_t lock;
//     struct rbt_t rbt;
// };

extern struct proc_t proc_0;

/**
 * FIXME:CLEAN UP
 */

int ptb_map(struct ptb_t *ptb, addr_t va, addr_t pa, uint usr, uint wtb)
{
    int err = E_OK;

    assert(va % PAGE_SIZE == 0 && pa % PAGE_SIZE == 0);

    spin_lock(&ptb->lock);

    addr_t pte = NULL;

    if (mmu_pte_get(ptb->pde, va, &pte) == E_NOTFOUND)
    {
        struct page_t *ptep;

        err = page_alloc(&ptep);

        if (err != E_OK) return err;

        mmu_pp_clean(ptep->addr);

        mmu_pte_set(ptb->pde, va, ptep->addr, usr, 1);
    }

    err = mmu_map(ptb->pde, va, pa, usr, wtb);

    spin_unlock(&ptb->lock);

    return err;
}

/**
 * FIXME: FREE PTE
 */
int ptb_unmap(struct ptb_t *ptb, addr_t va)
{
    assert(va % PAGE_SIZE == 0);

    spin_lock(&ptb->lock);

    int err = mmu_unmap(ptb->pde, va);

    addr_t pte;

    err = mmu_pte_get(ptb->pde, va, &pte);

    if (err != E_OK)
    {
        spin_unlock(&ptb->lock);

        return err;
    }

    // struct page_t *page;

    // err = page_get_ptr(pte, &page);

    // assert (err == E_OK);

    // page_put(page);

    spin_unlock(&ptb->lock);

    return err;
}

int ptb_init(struct ptb_t *tb, addr_t pde)
{
    spin_init(&tb->lock);

    tb->pde = pde;

    rbt_init(&tb->rbt);

    return E_OK;
}

int ptb_setup(struct ptb_t *tb)
{
    struct page_t *pde;

    int err = page_alloc(&pde);

    if (err != E_OK) return err;

    spin_init(&tb->lock);

    rbt_init(&tb->rbt);

    tb->pde = pde->addr;

    mmu_sync_kern_space(proc_0.ptb.pde, tb->pde, KERN_BASE, 0 - KERN_BASE);

    return err;
}

int ptb_fork(struct ptb_t *ptbo, struct ptb_t *ptbn)
{
    struct page_t *pden;

    int err = page_alloc(&pden);

    if (err != E_OK) return err;

    ptb_init(ptbn, pden->addr);

    mmu_sync_kern_space(proc_0.ptb.pde, ptbn->pde, KERN_BASE, 0 - KERN_BASE);

    return E_OK;
}