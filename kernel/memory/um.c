#include <error.h>
#include <memory.h>
#include <proc.h>
#include <log.h>
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

            struct vpage_t *vp;

            err = vm_alloc(&um->vp_alct, &vp, size);

            if (err != E_OK) return err;

            *addr = vp->addr;
        }
        else
            return err;
    }

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

    info("umalloc success: addr = %p, size = %p.\n", *addr, size);

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

int um_init(struct um_t *um)
{
    int err = vm_init(&um->vp_alct, kmalloc, kmfree);

    if (err != E_OK) return err;

    um->layout = init_uvmlo;

    slab_init(&um->slab_alct, umalloc_slab, umfree_slab);

    return err;
}