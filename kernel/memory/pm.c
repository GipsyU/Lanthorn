#include <error.h>
#include <list.h>
#include <log.h>
#include <util.h>
#include <memory.h>
/*
 * FIXME: no free cache of list node.
 */
static int new_page(struct page_alct_t *alct, struct page_t **page)
{
    int err = E_OK;

    err = slot_new(&(alct->slot_alct), (addr_t *)page);

    atomic_set(&(*page)->cnt, 0);

    if (err != E_OK)
    {
        /**
         * FIXME
         */
        error("buddy slot not enough\n");

        return err;
    }

    return err;
}

static int free_page(struct page_alct_t *alct, struct page_t *page)
{
    return slot_free(&(alct->slot_alct), (addr_t)page);
}

static int merge_page(struct page_alct_t *alct, struct page_t **f, struct page_t *p1, struct page_t *p2)
{
    assert (p1->buddy.state == BUD_ALLOCED && p2->buddy.state == BUD_ALLOCED, "bud bug.\n");
    
    assert(p1->buddy.b == p2 && p2->buddy.b == p1, "bud bug.\n");

    assert(p1->buddy.f == p2->buddy.f, "bud bug\n");

    *f = p1->buddy.f;

    assert((*f)->buddy.state == BUD_VIRT, "bud bug.\n");

    /**
     * FIXME: should free
     */

    free_page(alct, p1);

    free_page(alct, p2);

    (*f)->buddy.state = BUD_ALLOCED;

    return E_OK;
}

static int divide_page(struct page_t *f, struct page_t *l, struct page_t *r)
{
    int err = E_OK;

    if (f->buddy.state != BUD_ALLOCED || f->buddy.order == 0) return E_INVAL;

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
    
    return err;
}

static int buddy_get(struct page_alct_t *alct, uint order, struct page_t **page)
{
    int err = E_OK;

    assert(order < CONFIG_NR_BUDDY_ORDER, "bud bug.\n");

    if (list_isempty(&alct->head[order])) return E_NOMEM;

    struct list_node_t *node = list_pop_front(&alct->head[order]);

    *page = container_of(node, struct page_t, buddy.list_node);

    (*page)->buddy.state = BUD_ALLOCED;

    return err;
}

static int buddy_get_ptc(struct page_t *page)
{
    if (page->buddy.state != BUD_FREE) return E_INVAL;

    list_delete(&page->buddy.list_node);

    page->buddy.state = BUD_ALLOCED;

    return E_OK;
}

static int buddy_put(struct page_alct_t *alct, struct page_t *page)
{
    if (page->buddy.order >= CONFIG_NR_BUDDY_ORDER) return E_INVAL;

    assert(page->buddy.state == BUD_ALLOCED, "bud bug\n.");

    info("put memory into buddy sys, addr:%p, order:%d.\n", page->addr, page->buddy.order);

    for (struct page_t *f; page->buddy.pos != BUD_NOBUD; page = f)
    {
        if (buddy_get_ptc(page->buddy.b) != E_OK) break;

        merge_page(alct, &f, page, page->buddy.b);
    }

    page->buddy.state = BUD_FREE;

    list_push_back(&alct->head[page->buddy.order], &page->buddy.list_node);

    return E_OK;
}

static int buddy_alloc(struct page_alct_t *alct, uint order, struct page_t **page)
{
    int err = E_OK;

    if (order >= CONFIG_NR_BUDDY_ORDER) return E_NOMEM;

    err = buddy_get(alct, order, page);

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

        err = buddy_put(alct, r);

        *page = l;
    }

    return err;
}

int pm_insert(struct page_alct_t *alct, addr_t addr, size_t size)
{
    int err = E_OK;

    assert(size % PAGE_SIZE == 0 && size > 0 && addr %PAGE_SIZE == 0, "bud bug.\n");

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

            int err = buddy_put(alct, page);

            if (err != E_OK) return err;

            addr += (1 << i) * PAGE_SIZE;
        }
    }

    return err;
}

int pm_alloc(struct page_alct_t *alct, size_t size, struct page_t **page)
{

    int err = E_OK;

    if (size == 0) return E_INVAL;

    size = ROUND_UP(size, PAGE_SIZE);

    if (size / PAGE_SIZE > (1 << (CONFIG_NR_BUDDY_ORDER - 1))) return E_NOMEM;

    int order;

    for (int i = CONFIG_NR_BUDDY_ORDER - 1; i >= 0; --i)
    {
        if (size / PAGE_SIZE <= (1 << i)) order = i;
        
        else break;
    }

    err = buddy_alloc(alct, order, page);

    return err;
}

int pm_free(struct page_alct_t *alct, struct page_t *page)
{
    int err = E_OK;

    err = buddy_put(alct, page);

    return err;
}

int pm_init(struct page_alct_t *alct, addr_t addr, size_t size)
{
    int err = E_OK;

    alct->head = (struct list_node_t *)addr;

    addr_t slot_addr = addr + CONFIG_NR_BUDDY_ORDER * sizeof(struct list_node_t);

    if (slot_addr > addr + size) return E_NOMEM;

    size_t slot_size = addr + size - slot_addr;

    err = slot_init(&(alct->slot_alct), sizeof(struct page_t));

    err = slot_insert(&(alct->slot_alct), slot_addr, slot_size);

    for (int i = 0; i < CONFIG_NR_BUDDY_ORDER; ++i)
    {
        list_init(&alct->head[i]);
    }

    return err;
}