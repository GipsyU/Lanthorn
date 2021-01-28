#include <arch/basic.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <rbt.h>
#include <spinlock.h>
#include <util.h>

static void update(struct rbt_node_t *node)
{
    struct vpage_t *vpage = container_of(node, struct vpage_t, rbt_node);

    size_t mx_size = 0;

    if (node->l)
    {
        mx_size = max(mx_size, (container_of(node->l, struct vpage_t, rbt_node))->mx_size);
    }

    if (node->r)
    {
        mx_size = max(mx_size, (container_of(node->r, struct vpage_t, rbt_node))->mx_size);
    }

    mx_size = max(mx_size, vpage->size);

    vpage->mx_size = mx_size;
}

static int _rbt_insert(struct rbt_t *rbt, struct vpage_t *vpage)
{
    struct rbt_node_t **p = &rbt->root;

    struct rbt_node_t *parent = NULL;

    struct vpage_t *vp;

    while (*p)
    {
        parent = *p;

        vp = container_of(parent, struct vpage_t, rbt_node);

        if (vpage->addr < vp->addr)
        {
            p = &(*p)->l;
        }
        else if (vpage->addr > vp->addr)
        {
            p = &(*p)->r;
        }
        else
        {
            return E_EXIST;
        }
    }

    rb_link_node(&vpage->rbt_node, parent, p);

    return E_OK;
}

static int rbt_insert(struct rbt_t *rbt, struct vpage_t *vpage)
{
    int err = _rbt_insert(rbt, vpage);

    if (err != E_OK) return err;

    rbt_insert_color(rbt, &vpage->rbt_node);

    return err;
}

/**
 * WARNING: delete from free rbt and free vp
 */
static int vm_delete_free(struct vpage_alct_t *alct, struct vpage_t *vp)
{
    int err = E_OK;

    struct rbt_node_t *deepset = rbt_delete_update_begin(&vp->rbt_node);

    rbt_delete(&alct->free_rbt, &vp->rbt_node);

    rbt_delete_update_end(deepset, update);

    err = alct->mm_ops.free(vp);

    return err;
}

static int vm_merge(struct vpage_alct_t *alct, struct vpage_t *vp)
{
    struct rbt_node_t *now = &vp->rbt_node;

    struct rbt_node_t *tmp = NULL;

    while ((tmp = rbt_next(now)) != NULL)
    {
        struct vpage_t *nowvp = container_of(now, struct vpage_t, rbt_node);

        struct vpage_t *tmpvp = container_of(tmp, struct vpage_t, rbt_node);

        if (nowvp->addr + nowvp->size != tmpvp->addr) return E_OK;

        nowvp->size += tmpvp->size;

        vm_delete_free(alct, tmpvp);

        rbt_update(now, update);
    }

    while ((tmp = rbt_prev(now)) != NULL)
    {
        struct vpage_t *nowvp = container_of(now, struct vpage_t, rbt_node);

        struct vpage_t *tmpvp = container_of(tmp, struct vpage_t, rbt_node);

        if (tmpvp->addr + tmpvp->size != nowvp->addr) return E_OK;

        nowvp->size += tmpvp->size;

        nowvp->addr = tmpvp->addr;

        vm_delete_free(alct, tmpvp);

        rbt_update(now, update);
    }

    return E_OK;
}

static int vm_insert_free(struct vpage_alct_t *alct, struct vpage_t *vp)
{
    int err = rbt_insert(&alct->free_rbt, vp);

    if (err != E_OK) return err;

    rbt_insert_update(&vp->rbt_node, update);

    err = vm_merge(alct, vp);

    return err;
}

static int vm_insert_alloced(struct vpage_alct_t *alct, struct vpage_t *vp)
{
    int err = rbt_insert(&alct->alloced_rbt, vp);

    if (err != E_OK) return err;

    return err;
}

int vm_search_addr(struct vpage_alct_t *alct, addr_t addr, struct vpage_t **res)
{
    int err = E_OK;

    struct rbt_node_t *n = alct->alloced_rbt.root;

    struct vpage_t *vpage;

    while (n)
    {
        vpage = container_of(n, struct vpage_t, rbt_node);

        if (addr < vpage->addr)
        {
            n = n->l;
        }
        else if (addr > vpage->addr)
        {
            n = n->r;
        }
        else
        {
            *res = vpage;
            
            return E_OK;
        }
    }
    
    return E_NOTFOUND;
}

static int rbt_search_size(struct rbt_t *rbt, size_t size, struct vpage_t **res)
{
    int err = E_OK;

    struct rbt_node_t *n = rbt->root;

    while (n)
    {
        struct vpage_t *vpage;

        size_t mx = -1;

        struct rbt_node_t *_n = NULL;

        if (n->l)
        {
            vpage = container_of(n->l, struct vpage_t, rbt_node);

            if (vpage->mx_size >= size && vpage->mx_size < mx)
            {
                mx = vpage->mx_size;

                _n = n->l;
            }
        }

        if (n->r)
        {
            vpage = container_of(n->r, struct vpage_t, rbt_node);

            if (vpage->mx_size >= size && vpage->mx_size < mx)
            {
                mx = vpage->mx_size;

                _n = n->r;
            }
        }

        vpage = container_of(n, struct vpage_t, rbt_node);

        if (vpage->size >= size && vpage->size < mx)
        {
            *res = vpage;

            return E_OK;
        }

        n = _n;
    }

    return E_NOMEM;
}

int vm_insert(struct vpage_alct_t *alct, addr_t addr, size_t size)
{
    int err = E_OK;

    if (addr % PAGE_SIZE != 0 || size % PAGE_SIZE != 0 || size == 0)
    {
        return E_INVAL;
    }

    struct vpage_t *vp;

    err = alct->mm_ops.alloc((addr_t *)&vp, sizeof(struct vpage_t));

    if (err != E_OK) return err;

    /**
     * TODO: vpage_init
     */

    vp->addr = addr;

    vp->size = size;

    err = vm_insert_free(alct, vp);

    return err;
}

int vm_alloc(struct vpage_alct_t *alct, struct vpage_t **vp, size_t size)
{
    int err = E_OK;

    size = ROUND_UP(size, PAGE_SIZE);

    struct vpage_t *res = NULL;

    err = rbt_search_size(&alct->free_rbt, size, &res);

    if (err != E_OK)
    {
        return err;
    }

    err = alct->mm_ops.alloc((addr_t *)vp, sizeof(struct vpage_t));

    if (err != E_OK)
    {
        return err;
    }

    (*vp)->addr = res->addr;

    (*vp)->size = size;

    if (res->size == size)
    {
        vm_delete_free(alct, res);
    }
    else
    {
        res->addr += size;

        res->size -= size;

        rbt_update(&res->rbt_node, update);
    }

    vm_insert_alloced(alct, *vp);

    return err;
}

int vm_free(struct vpage_alct_t *alct, struct vpage_t *vp)
{
    int err = E_OK;

    rbt_delete(&alct->alloced_rbt, &vp->rbt_node);

    err = vm_insert_free(alct, vp);

    return err;
}

// int vm_dump(struct vpage_alct_t *old_alct, struct vpage_alct_t **new_alct)
// {
//     int err = kmalloc(new_alct, sizeof(struct vpage_alct_t));

//     if (err != E_OK) return err;

//     spin_lock(&old_alct->lock);

//     spin_init(&(*new_alct)->lock);

//     (*new_alct)->mm_ops = old_alct->mm_ops;

//     struct rbt_node_t *node = NULL;

//     err = rbt_first(&old_alct->free_rbt, &node);

    

//     spin_unlock(&old_alct->lock);

//     return err;
// }

/**
 * FIXME: func p
 */
int vm_init(struct vpage_alct_t *alct, int (*alloc)(addr_t *addr, size_t size), int (*free)(addr_t addr))
{
    spin_init(&alct->lock);

    alct->free_rbt.root = NULL;

    alct->alloced_rbt.root = NULL;

    alct->mm_ops.alloc = alloc;

    alct->mm_ops.free = free;

    return E_OK;
}