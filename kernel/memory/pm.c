#include <error.h>
#include <list.h>
#include <log.h>
#include <memory.h>
#include <rbt.h>
#include <spinlock.h>
#include <util.h>
/*
 * FIXME: no free cache of list node.
 * be careful: maybe cause deadlock.
 */
static int new_page(struct page_alct_t *alct, struct page_t **page)
{
    int err = E_OK;

    err = slot_alloc(&(alct->slot_alct), (addr_t *)page);

    if (err != E_OK)
    {
        error("buddy slot not enough\n");

        return err;
    }

    atomic_set(&(*page)->cnt, 0);

    spin_init(&(*page)->lock);

    return err;
}

static void free_page(struct page_alct_t *alct, struct page_t *page)
{
    slot_free(&(alct->slot_alct), (addr_t)page);
}

static void merge_page(struct page_alct_t *alct, struct page_t **f, struct page_t *p1, struct page_t *p2)
{
    assert(p1->buddy.state == BUD_ALLOCED && p2->buddy.state == BUD_ALLOCED);

    assert(p1->buddy.b == p2 && p2->buddy.b == p1);

    assert(p1->buddy.f == p2->buddy.f);

    *f = p1->buddy.f;

    assert((*f)->buddy.state == BUD_VIRT);

    (*f)->buddy.state = BUD_ALLOCED;
}

static int divide_page(struct page_t *f, struct page_t *l, struct page_t *r)
{
    if (f->buddy.order == 0) return E_NOMEM;

    l->addr = f->addr;

    l->buddy.order = f->buddy.order - 1;

    l->size = f->size / 2;

    r->addr = l->addr + l->size;

    r->buddy.order = l->buddy.order;

    r->size = l->size;

    l->buddy.f = r->buddy.f = f;

    l->buddy.b = r;

    r->buddy.b = l;

    l->buddy.pos = BUD_LEFT;

    r->buddy.pos = BUD_RIGHT;

    l->buddy.state = BUD_ALLOCED;

    r->buddy.state = BUD_ALLOCED;

    f->buddy.state = BUD_VIRT;

    return E_OK;
}

static int buddy_get(struct page_alct_t *alct, uint order, struct page_t **page)
{
    assert(order < CONFIG_NR_BUDDY_ORDER);

    if (list_isempty(&alct->head[order])) return E_NOMEM;

    struct list_node_t *node = list_pop_front(&alct->head[order]);

    *page = container_of(node, struct page_t, buddy.list_node);

    (*page)->buddy.state = BUD_ALLOCED;

    return E_OK;
}

static int buddy_get_ptc(struct page_t *page)
{
    if (page->buddy.state != BUD_FREE) return E_FAILE;

    list_delete(&page->buddy.list_node);

    page->buddy.state = BUD_ALLOCED;

    return E_OK;
}

static int buddy_put(struct page_alct_t *alct, struct page_t *page)
{
    assert(page->buddy.order < CONFIG_NR_BUDDY_ORDER);

    assert(page->buddy.state == BUD_ALLOCED);

    info("put memory into buddy sys, addr:%p, order:%d.\n", page->addr, page->buddy.order);

    for (struct page_t *f; page->buddy.pos != BUD_NOBUD; page = f)
    {
        if (buddy_get_ptc(page->buddy.b) != E_OK) break;

        merge_page(alct, &f, page, page->buddy.b);

        free_page(alct, page->buddy.b);

        free_page(alct, page);
    }

    page->buddy.state = BUD_FREE;

    list_push_back(&alct->head[page->buddy.order], &page->buddy.list_node);

    return E_OK;
}

static int buddy_alloc(struct page_alct_t *alct, uint order, struct page_t **page)
{
    if (order >= CONFIG_NR_BUDDY_ORDER) return E_NOMEM;

    int err = buddy_get(alct, order, page);

    if (err != E_OK)
    {
        struct page_t *f, *l, *r;

        err = buddy_alloc(alct, order + 1, &f);

        if (err != E_OK) return err;

        new_page(alct, &l);

        new_page(alct, &r);

        err = divide_page(f, l, r);

        if (err != E_OK)
        {
            free_page(alct, l);

            free_page(alct, r);

            buddy_put(alct, f);

            return err;
        }

        buddy_put(alct, r);

        *page = l;
    }

    return err;
}

static int _rbt_insert(struct rbt_t *rbt, struct page_t *page)
{
    struct rbt_node_t **p = &rbt->root;

    struct rbt_node_t *parent = NULL;

    struct page_t *pg;

    while (*p)
    {
        parent = *p;

        pg = container_of(parent, struct page_t, rbt_node);

        if (page->addr < pg->addr)
        {
            p = &(*p)->l;
        }
        else if (page->addr > pg->addr)
        {
            p = &(*p)->r;
        }
        else
        {
            return E_EXIST;
        }
    }

    rb_link_node(&page->rbt_node, parent, p);

    return E_OK;
}

static int rbt_insert(struct rbt_t *rbt, struct page_t *page)
{
    int err = _rbt_insert(rbt, page);

    if (err != E_OK) return err;

    rbt_insert_color(rbt, &page->rbt_node);

    return err;
}

static int rbt_find(struct rbt_t *rbt, addr_t pa, struct page_t **res)
{
    assert(pa % PAGE_SIZE == 0);

    struct rbt_node_t *n = rbt->root;

    struct page_t *page;

    while (n)
    {
        page = container_of(n, struct page_t, rbt_node);

        if (pa < page->addr)
        {
            n = n->l;
        }
        else if (pa > page->addr)
        {
            n = n->r;
        }
        else
        {
            *res = page;

            return E_OK;
        }
    }

    return E_NOTFOUND;
}

int pm_insert(struct page_alct_t *alct, addr_t addr, size_t size)
{
    assert(size % PAGE_SIZE == 0 && size > 0 && addr % PAGE_SIZE == 0);

    int err = E_OK;

    for (int i = 0; i < CONFIG_NR_BUDDY_ORDER; ++i)
    {
        if ((size / PAGE_SIZE) & (1 << i))
        {
            struct page_t *page;

            err = new_page(alct, &page);

            if (err != E_OK) return err;

            page->addr = addr;

            page->size = (1 << i) * PAGE_SIZE;

            page->buddy.order = i;

            page->buddy.pos = BUD_NOBUD;

            page->buddy.state = BUD_ALLOCED;

            buddy_put(alct, page);

            addr += (1 << i) * PAGE_SIZE;
        }
    }

    return err;
}

int pm_alloc(struct page_alct_t *alct, size_t size, struct page_t **page)
{

    if (size == 0) return E_INVAL;

    size = ROUND_UP(size, PAGE_SIZE);

    int order;

    for (int i = CONFIG_NR_BUDDY_ORDER - 1; i >= 0; --i)
    {
        if (size / PAGE_SIZE <= ((uint)1 << i))
            order = i;

        else
            break;
    }

    spin_lock(&alct->lock);

    int err = buddy_alloc(alct, order, page);

    if (err != E_OK)
    {
        spin_unlock(&alct->lock);

        return err;
    }

    err = rbt_insert(&alct->alloced_rbt, *page);

    assert(err == E_OK);

    spin_unlock(&alct->lock);

    return err;
}

int pm_free(struct page_alct_t *alct, struct page_t *page)
{
    int err = E_OK;

    spin_lock(&alct->lock);

    err = buddy_put(alct, page);

    if (err != E_OK)
    {
        spin_unlock(&alct->lock);

        return err;
    }

    rbt_delete(&alct->alloced_rbt, &page->rbt_node);

    spin_unlock(&alct->lock);

    return err;
}

int pm_init(struct page_alct_t *alct, addr_t addr, size_t size)
{
    int err = E_OK;

    alct->head = (struct list_node_t *)addr;

    addr_t slot_addr = addr + CONFIG_NR_BUDDY_ORDER * sizeof(struct list_node_t);

    if (slot_addr > addr + size) return E_NOMEM;

    size_t slot_size = addr + size - slot_addr;

    slot_init(&(alct->slot_alct), sizeof(struct page_t));

    slot_insert(&(alct->slot_alct), slot_addr, slot_size);

    for (int i = 0; i < CONFIG_NR_BUDDY_ORDER; ++i)
    {
        list_init(&alct->head[i]);
    }

    spin_init(&alct->lock);

    rbt_init(&alct->alloced_rbt);

    return err;
}

int pm_get_page(struct page_alct_t *alct, addr_t addr, struct page_t **page)
{
    assert(addr % PAGE_SIZE == 0);

    spin_lock(&alct->lock);

    int err = rbt_find(&alct->alloced_rbt, addr, page);

    spin_unlock(&alct->lock);

    return err;
}