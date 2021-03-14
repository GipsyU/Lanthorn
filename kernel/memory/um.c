#include <arch/phyops.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <proc.h>
#include <syscall.h>
#include <util.h>

int um_stack_alloc(struct um_t *um, addr_t *addr, size_t size)
{
    assert(size % PAGE_SIZE == 0);

    if (um->layout.free_s + size > um->layout.free_e) return E_NOMEM;

    um->layout.stk_s -= size;

    um->layout.free_e -= size;

    *addr = um->layout.stk_s;

    return E_OK;
}

int um_heap_alloc(struct um_t *um, addr_t *addr, size_t size)
{
    assert(size % PAGE_SIZE == 0);

    if (um->layout.free_s + size > um->layout.free_e) return E_NOMEM;

    *addr = um->layout.heap_e;

    um->layout.heap_e += size;

    um->layout.free_s += size;

    return E_OK;
}

static int umalloc_page(struct um_t *um, addr_t *addr, size_t size)
{
    assert(size % PAGE_SIZE == 0);

    struct vpage_t *vp;

    int err = vm_alloc(&um->vp_alct, &vp, size);

    if (err != E_OK)
    {
        if (err == E_NOMEM)
        {
            addr_t nm = NULL;

            err = um_heap_alloc(um, &nm, size);

            if (err != E_OK) return err;

            err = vm_insert(&um->vp_alct, nm, size);

            if (err != E_OK) return err;

            err = vm_alloc(&um->vp_alct, &vp, size);

            if (err != E_OK) return err;

            *addr = vp->addr;
        }
        else
            return err;
    }

    vp->type = UM_NOPM;

    *addr = vp->addr;

    return err;
}

int umalloc(struct um_t *um, addr_t *addr, size_t size)
{
    int err = E_OK;

    if (size == 0) return E_INVAL;

    if (size <= CONFIG_SZ_SLAB_ALLOC_MAX)
    {
        err = slab_alloc(&um->slab_alct, addr, size);
    }
    else
    {
        err = umalloc_page(um, addr, ROUND_UP(size, PAGE_SIZE));
    }

    if (err == E_OK) info("umalloc success: addr = %p, size = %p.\n", *addr, size);

    return err;
}

int umfree(struct um_t *um, addr_t addr)
{
    return E_OK;
}

static int umalloc_slab(addr_t *addr, size_t size)
{
    struct proc_t *proc = proc_now();

    return umalloc(&proc->um, addr, size);
}

static int umfree_slab(addr_t addr)
{
    struct proc_t *proc = proc_now();

    return umfree(&proc->um, addr);
}

static int umalloc_hdl(addr_t *addr, size_t *size)
{
    struct proc_t *proc = proc_now();

    return umalloc(&proc->um, addr, size);
}

int um_init(struct um_t *um)
{
    int err = vm_init(&um->vp_alct, kmalloc, kmfree);

    if (err != E_OK) return err;

    um->layout = init_uvmlo;

    slab_init(&um->slab_alct, umalloc_slab, umfree_slab);

    err = syscall_register(SYS_malloc, umalloc_hdl, 2);

    return err;
}
/**
 * FIXME: CLEAN UP
 */

int um_fork(struct um_t *um_old, struct um_t *um_new, struct ptb_t *ptb)
{
    um_new->layout = um_old->layout;

    int err = vm_fork(&um_old->vp_alct, &um_new->vp_alct, ptb);

    if (err != E_OK) return err;

    err = slab_fork(&um_old->slab_alct, &um_new->slab_alct);

    return err;
}

int um_page_fault_hdl(struct um_t *um, struct ptb_t *ptb, addr_t errva)
{
    info("um page fault, addr = %p.\n", errva);
    
    int err = E_OK;

    if (errva >= um->layout.args_s && errva < um->layout.args_e)
    {
        struct page_t *page = NULL;

        err = page_alloc(&page);

        if (err != E_OK) return err;

        err = ptb_map(ptb, um->layout.args_s, page->addr, 1, 1);

        return err;
    }

    if (errva < um->layout.heap_s || errva >= um->layout.heap_e)
    {
        panic("um page fault error");
    }

    struct vpage_t *vp = NULL;

    err = vm_search_addr(&um->vp_alct, errva, &vp);

    if (err != E_OK) return err;

    if (vp->type == UM_NOPM)
    {
        struct vpage_t *tmp = NULL;

        err = vm_slice(&um->vp_alct, vp, ROUND_DOWN(errva, PAGE_SIZE), PAGE_SIZE, &tmp);

        if (err != E_OK) return err;

        vp = tmp;

        struct page_t *page = NULL;

        err = page_alloc(&page);

        if (err != E_OK) return err;

        err = ptb_map(ptb, vp->addr, page->addr, 1, 1);

        if (err != E_OK) return err;

        vp->map_page = page;

        vp->type = UM_NORMAL;
    }
    else if (vp->type == UM_NORMAL)
    {
        warn("um page fault in um_normal type in vpage.\n");
        while (1)
            ;
    }
    else if (vp->type == UM_RCU)
    {
        //     struct vpage_t *tmp = NULL;

        //     err = vm_slice(&um->vp_alct, vp, ROUND_DOWN(errva, PAGE_SIZE), PAGE_SIZE, &tmp);

        //     if (err != E_OK) return err;

        //     vp = tmp;

        struct page_t *page = NULL;

        err = page_alloc(&page);

        if (err != E_OK) return err;

        phyops_memcpy_v2p(page->addr, ROUND_DOWN(errva, PAGE_SIZE), PAGE_SIZE);

        err = ptb_unmap(ptb, vp->addr);

        assert(err == E_OK);

        err = ptb_map(ptb, vp->addr, page->addr, 1, 1);

        if (err != E_OK) return err;

        vp->map_page = page;

        vp->type = UM_NORMAL;
    }
    else
    {
        panic("um page fault bug.\n");
    }

    return err;
}
