#include <arch/mmu.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <proc.h>
#include <string.h>
#include <thread.h>
#include <util.h>
#include <elf.h>

static struct proc_t proc_0;

static struct proc_t *proc_1;

static int p0_init(addr_t pde)
{
    int err = E_OK;

    // pagetb_init(&proc_0.pagetb, pde);

    // list_init(&proc_0.thraed_ls);

    return err;
}

/**
 * FIXME: CLEAN UP
 */

static int p1_init(void)
{
    int err = proc_new(&proc_1);

    if (err != E_OK) return err;

    struct thread_t *thread = NULL;

    err = thread_user_new(&thread, proc_1, 0, PAGE_SIZE);

    if (err != E_OK) return err;

    addr_t exea = NULL;

    err = um_heap_alloc(&proc_1->um, &exea, PAGE_SIZE);

    assert(exea == 0);

    if (err != E_OK) return err;

    struct page_t *page;

    err = page_alloc(&page);

    if (err != E_OK) return err;

    err = mmu_user_map(proc_1->pagetb.pde->addr, page->addr, exea, 1);

    addr_t t;

    err = mmu_v2p(proc_1->pagetb.pde->addr, 0, &t);

    if (err != E_OK) return err;

    addr_t tmp;

    err = kmalloc(&tmp, PAGE_SIZE);

    assert(tmp %PAGE_SIZE == 0);

    if (err != E_OK) return err;

    err = mmu_kern_map(page->addr, tmp);

    if (err != E_OK) return err;

    extern char _binary_usr_init_elf_start[];
    
    extern char _binary_usr_init_elf_end[];

    err = elf_read((void *)_binary_usr_init_elf_start, NULL);

    if (err != E_OK) return err;

    elf_load((void *)_binary_usr_init_elf_start, tmp);

    return err;
}

struct proc_t *proc_now(void)
{
    struct thread_t *thread = thread_now();

    return thread->proc;
}

// int proc_user_init(struct proc_t **proc)
// {
//     int err = E_OK;

//     err = mmu_pde_init((*proc)->pde);

//     if (err != E_OK)
//     {
//         // kfree(proc);
//         return err;
//     }

//     addr_t pte, init;

//     // err = page_alloc(&pte);

//     if (err != E_OK)
//     {
//         return err;
//     }

//     err = kmalloc(&init, PAGE_SIZE);

//     if (err != E_OK)
//     {
//         return err;
//     }

//     addr_t initp;

//     err = mmu_v2p((addr_t)PDE - KERN_BASE, init, &initp);

//     if (err != E_OK)
//     {
//         return err;
//     }

//     err = mmu_user_map((*proc)->pde, initp, 0, 0);

//     if (err != E_OK)
//     {
//         return err;
//     }

//     // memcpy(init, (addr_t)init_start, PAGE_SIZE);

//     return err;
// }

/**
 * FIXME: CLEAN UP RSC
 */
int proc_new(struct proc_t **proc)
{
    int err = kmalloc((addr_t *)proc, sizeof(struct proc_t));

    if (err != E_OK) return err;

    err = pagetb_init(&(*proc)->pagetb);

    if (err != E_OK) return err;

    err = page_alloc(&(*proc)->pagetb.pde);

    if (err != E_OK) return err;

    for (addr_t addr = KERN_BASE; addr != 0; addr += PAGE_SIZE * PAGE_SIZE / sizeof(addr_t))
    {
        err = mmu_sync_kern_space((*proc)->pagetb.pde->addr, addr);

        if (err != E_OK) return err;
    }

    list_init(&(*proc)->thread_ls);

    err = um_init(&(*proc)->um);

    if (err != E_OK) return err;

    return err;
}

// int proc_free(struct proc_t *proc)
// {
//     int err ;

//     err = kmfree(proc);

// }

int proc_init(addr_t pde)
{
    int err = p0_init(pde);

    if (err != E_OK) return err;

    err = thread_init();
    
    if (err != E_OK) return err;

    err = p1_init();

    if (err != E_OK) return err;

    return err;
}

int proc_switch(struct proc_t *proc)
{
    mmu_pde_switch(proc->pagetb.pde->addr);

    return E_OK;
}
