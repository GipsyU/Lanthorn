#include <proc.h>
#include <error.h>
#include <memory.h>
#include <arch/mmu.h>
#include <string.h>
#include <log.h>
#include <thread.h>

static struct proc_t proc_0;

static int p0_init(addr_t pde)
{
    int err = E_OK;

    // pagetb_init(&proc_0.pagetb, pde);


    // list_init(&proc_0.thraed_ls);

    return err;
}

// int proc_user_init(struct proc_t **proc)
// {
//     int err = E_OK;

//     err = kmalloc((addr_t *)*proc, sizeof (struct proc_t));

//     if (err != E_OK)
//     {
//         return err;
//     }

//     // err = page_alloc(&(*proc)->pde);

//     if (err != E_OK)
//     {
//         // kfree
//         // kfree
//         return err;
//     }
    

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

int proc_init(addr_t pde)
{
    int err = p0_init(pde);

    if (err != E_OK) return err;

    err = thread_init();

    if (err != E_OK) return err;
    
    return err;
}

int proc_switch(struct proc_t *proc)
{
    mmu_pde_switch(proc->pagetb.pde->addr);

    return E_OK;
}

int proc_new(struct proc_t **proc)
{
    int err = kmalloc((addr_t *)proc, sizeof (struct proc_t));

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

    return err;
}

// int proc_free(struct proc_t *proc)
// {
//     int err ;

//     err = kmfree(proc);


// }

