#include <proc.h>
#include <error.h>
#include <memory.h>
#include <arch/mmu.h>
#include <string.h>
#include <log.h>
extern void init_start(void);
extern void init_end(void);
extern char PDE[];
static int proc_alloc(struct proc_t **proc)
{
    int err = E_OK;

    err = kalloc((addr_t *)*proc, sizeof (struct proc_t));

    /**
     * FIXME:ALLOC PID
     */

    return err;
}

static int p0_init(addr_t pde)
{
    int err = E_OK;

    p0.pde = pde;

    p0.pid = 0;

    memcpy(p0.name, "kern", 4);

    return err;
}

static int p1_init(void)
{
    int err = E_OK;

    struct proc_t *proc;

    err = proc_alloc(&proc);

    if (err != E_OK)
    {
        return err;
    }

    

    return err;
}

int proc_user_init(struct proc_t **proc)
{
    int err = E_OK;

    err = kalloc((addr_t *)*proc, sizeof (struct proc_t));

    if (err != E_OK)
    {
        return err;
    }

    err = page_alloc(&(*proc)->pde);

    if (err != E_OK)
    {
        // kfree
        // kfree
        return err;
    }
    

    err = mmu_pde_init((*proc)->pde);

    if (err != E_OK)
    {
        // kfree(proc);
        return err;
    }

    addr_t pte, init;

    err = page_alloc(&pte);

    if (err != E_OK)
    {
        return err;
    }

    err = kalloc(&init, PAGE_SIZE);

    if (err != E_OK)
    {
        return err;
    }
    
    addr_t initp;

    err = mmu_v2p((addr_t)PDE - KERN_BASE, init, &initp);

    if (err != E_OK)
    {
        return err;
    }

    err = mmu_user_map((*proc)->pde, initp, 0, pte);

    if (err != E_OK)
    {
        return err;
    }

    memcpy(init, init_start, PAGE_SIZE);

    return err;
}

int proc_init(addr_t pde)
{
    int err = E_OK;

    err = p0_init(pde);

    if (err != E_OK)
    {
        return err;
    }

    return err;
}

int proc_switch(struct proc_t *proc)
{

    mmu_pde_switch(proc->pde);

    return E_OK;
}

// int proc_fork()
// {

// }

int proc_new(struct proc_t **proc)
{
    int err = E_OK;

    err = kalloc((addr_t *)*proc, sizeof (struct proc_t));

    if (err != E_OK)
    {
        return err;
    }



    return err;
}