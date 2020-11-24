#include <error.h>
#include <list.h>
#include <log.h>
#include <util.h>
#include <memory.h>

#include "page.h"
#include "buddy.h"

#define BUDDY_NO_BUD 0
#define BUDDY_LEFT 1
#define BUDDY_RIGHT 2
#define BUDDY_BUD_MASK 3
#define BUDDY_NO_USE 0
#define BUDDY_REAL 4
#define BUDDY_VIRT 8
#define BUDDY_ATTR_MASK 12
#define BUDDY_FREE 16
#define BUDDY_ALLOCED 32
#define BUDDY_STATE_MASK 48

/*
 * FIXME
 */

#define BUDDY_TPX(TPX)                                        \
                                                              \
    static inline u32 GET_##TPX(struct page_t *page)          \
    {                                                         \
        return page->flag & BUDDY_##TPX##_MASK;               \
    }                                                         \
                                                              \
    static inline int SET_##TPX(struct page_t *page, u32 val) \
    {                                                         \
        page->flag &= (~BUDDY_##TPX##_MASK);                  \
        page->flag |= val;                                    \
        return E_OK;                                          \
    }

BUDDY_TPX(BUD)

BUDDY_TPX(ATTR)

BUDDY_TPX(STATE)

/*
 * FIXME: no free cache of list node.
 */

static int new_node(struct buddy_allocator_t *alct, struct list_node_t **node)
{


    int err = E_OK;

    err = slot_new(&(alct->slot_alct), (addr_t *)node);

    if (err != E_OK)
    {
        /**
         * FIXME
         */
        error("buddy slot not enough\n");

        return err;
    }

    SET_BUD(list_data(*node), BUDDY_NO_BUD);

    SET_ATTR(list_data(*node), BUDDY_NO_USE);
    return err;
}

static int free_node(struct buddy_allocator_t *alct, struct list_node_t *node)
{
    return slot_free(&(alct->slot_alct), (addr_t)node);
}

static int merge_node(struct buddy_allocator_t *alct, struct list_node_t **f_node, struct list_node_t *node1, struct list_node_t *node2)
{
    int err = E_OK;

    if (f_node == NULL || node1 == NULL || node2 == NULL)
    {
        return E_INVAL;
    }

    struct page_t *page1 = (struct page_t *)list_data(node1);

    struct page_t *page2 = (struct page_t *)list_data(node2);


    if ((GET_BUD(page1) != BUDDY_LEFT || GET_BUD(page2) != BUDDY_RIGHT) && (GET_BUD(page1) != BUDDY_RIGHT || GET_BUD(page2) != BUDDY_LEFT))
    {
        return E_INVAL;
    }

    if (GET_STATE(page1) != BUDDY_ALLOCED || GET_STATE(page2) != BUDDY_ALLOCED)
    {
        return E_INVAL;
    }

    if (page1->buddy.b_node != node2 || page2->buddy.b_node != node1)
    {
        return E_INVAL;
    }

    if (page1->buddy.f_node != page2->buddy.f_node)
    {
        return E_INVAL;
    }

    *f_node = page1->buddy.f_node;

    struct page_t *f_page = (struct page_t *)list_data(*f_node);

    if (GET_ATTR(f_page) != BUDDY_VIRT || GET_ATTR(page1) != BUDDY_REAL || GET_ATTR(page2) != BUDDY_REAL)
    {
        return E_INVAL;
    }

    /*
     * FIXME: here shouldn't free, just change it to unused.
     */

    err = free_node(alct, node1);

    if (err != E_OK)
    {
        return err;
    }

    err = free_node(alct, node2);

    if (err != E_OK)
    {
        return err;
    }

    SET_ATTR(f_page, BUDDY_REAL);

    SET_STATE(f_page, BUDDY_ALLOCED);

    return err;
}

static int divide_node(struct list_node_t *f_node, struct list_node_t *l_node, struct list_node_t *r_node)
{
    int err = E_OK;

    if (f_node == NULL || l_node == NULL || r_node == NULL)
    {
        return E_INVAL;
    }

    struct page_t *f_page = list_data(f_node);

    struct page_t *l_page = list_data(l_node); 
    
    struct page_t *r_page = list_data(r_node); 

    if (GET_ATTR(f_page) != BUDDY_REAL || f_page->buddy.order == 0)
    {
        return E_INVAL;
    }

    l_page->addr = f_page->addr;

    l_page->buddy.order = f_page->buddy.order - 1;

    l_page->num = f_page->num / 2;

    r_page->addr = l_page->addr + (1 << l_page->buddy.order) * PAGE_SIZE;

    r_page->buddy.order = l_page->buddy.order;

    r_page->num = f_page->num / 2;

    l_page->buddy.f_node = r_page->buddy.f_node = f_node;

    l_page->buddy.b_node = r_node;

    r_page->buddy.b_node = l_node;

    SET_BUD(l_page, BUDDY_LEFT);

    SET_BUD(r_page, BUDDY_RIGHT);

    SET_ATTR(l_page, BUDDY_REAL);

    SET_ATTR(r_page, BUDDY_REAL);

    SET_STATE(l_page, BUDDY_ALLOCED);

    SET_STATE(r_page, BUDDY_ALLOCED);

    SET_ATTR(f_page, BUDDY_VIRT);

    return err;
}

static int get_buddy(struct buddy_allocator_t *alct, uint order, struct list_node_t **node)
{
    int err = E_OK;

    if (alct == NULL || order >= CONFIG_NR_BUDDY_ORDER || node == NULL)
    {
        return E_INVAL;
    }

    if (list_isempty(&alct->head[order]))
    {
        return E_NOMEM;
    }

    *node = list_pop_front(&alct->head[order]);

    SET_STATE(list_data(*node), BUDDY_ALLOCED);

    return err;
}

static int get_ptc_buddy(struct list_node_t *node)
{
    int err = E_OK;

    struct page_t *page = list_data(node);

    if (node == NULL || GET_ATTR(page) != BUDDY_REAL || GET_STATE(page) != BUDDY_FREE)
    {
        return E_INVAL;
    }

    list_delete(node);

    SET_STATE(page, BUDDY_ALLOCED);

    return err;
}

static int put_buddy(struct buddy_allocator_t *alct, struct list_node_t *node)
{
    int err = E_OK;

    struct page_t * page = list_data(node);

    if (page->buddy.order >= CONFIG_NR_BUDDY_ORDER)
    {
        return E_INVAL;
    }

    if (GET_ATTR(page) != BUDDY_REAL || GET_STATE(page) != BUDDY_ALLOCED)
    {
        return E_INVAL;
    }

    info("%s: put memory into buddy sys, addr:%p, order:%d.\n", alct->name, page->addr, page->buddy.order);

    for (struct list_node_t *f_node; GET_BUD(page) != BUDDY_NO_BUD; node = f_node)
    {
        page = list_data(node);

        err = get_ptc_buddy(page->buddy.b_node);

        if (err != E_OK)
        {
            err = E_OK;

            break;
        }

        err = merge_node(alct, &f_node, node, page->buddy.b_node);

        if (err != E_OK)
        {
            err = E_OK;

            break;
        }
    }

    SET_STATE(page, BUDDY_FREE);

    list_push_back(&alct->head[page->buddy.order], node);

    return err;
}

static int buddy_node_alloc(struct buddy_allocator_t *alct, uint order, struct list_node_t **node)
{
    int err = E_OK;

    if (order >= CONFIG_NR_BUDDY_ORDER)
    {
        return E_NOMEM;
    }

    err = get_buddy(alct, order, node);

    if (err != E_OK)
    {
        struct list_node_t *f_node, *l_node, *r_node;

        err = buddy_node_alloc(alct, order + 1, &f_node);

        if (err != E_OK)
        {
            return err;
        }

        new_node(alct, &l_node);

        new_node(alct, &r_node);

        err = divide_node(f_node, l_node, r_node);

        if (err != E_OK)
        {
            free_node(alct, l_node);

            free_node(alct, r_node);

            put_buddy(alct, f_node);

            return err;
        }

        err = put_buddy(alct, r_node);

        *node = l_node;
    }

    return err;
}

int buddy_insert(struct buddy_allocator_t *alct, struct page_t *page)
{
    int err = E_OK;

    if (alct == NULL || page == NULL || page->num == 0)
    {
        return E_INVAL;
    }

    addr_t start = page->addr;

    for (int i = 0; i < CONFIG_NR_BUDDY_ORDER; ++i)
    {
        if (page->num & (1 << i))
        {
            struct list_node_t *node;

            err = new_node(alct, &node);

            if (err != E_OK)
            {
                /*
                 * FIXME: here need to release inserted before.
                 */
                return err;
            }

            struct page_t *page = list_data(node);

            page->addr = start;

            page->num = (1 << i);

            page->buddy.order = i;

            SET_BUD(page, BUDDY_NO_BUD);

            SET_ATTR(page, BUDDY_REAL);

            SET_STATE(page, BUDDY_ALLOCED);

            int err = put_buddy(alct, node);

            if (err != E_OK)
            {
                /*
                 * FIXME: here need to release inserted before.
                 */
                return err;
            }

            start += (1 << i) * PAGE_SIZE;
        }
    }

    assert(start == page->addr + page->num * PAGE_SIZE, "buddy insert bug.\n");

    return err;
}

int buddy_alloc(struct buddy_allocator_t *alct, size_t page_num, struct page_t **page)
{

    int err = E_OK;

    if (alct == NULL || page_num == 0 || page == NULL)
    {
        return E_INVAL;
    }

    if (page_num > (1 << (CONFIG_NR_BUDDY_ORDER - 1)))
    {
        return E_NOMEM;
    }

    int order;

    for (int i = CONFIG_NR_BUDDY_ORDER - 1; i >= 0; --i)
    {

        if (page_num <= (1 << i))
        {
            order = i;
        }
        else
        {
            break;
        }
    }

    struct list_node_t *node;

    err = buddy_node_alloc(alct, order, &node);

    if (err != E_OK)
    {
        return err;
    }

    *page = list_data(node);

    return err;
}

int buddy_free(struct buddy_allocator_t *alct, struct page_t *page)
{
    int err = E_OK;

    struct list_node_t *node = ((addr_t)page - sizeof(struct list_node_t));

    err = put_buddy(alct, node);

    return err;
}

int buddy_init(struct buddy_allocator_t *alct, addr_t addr, size_t size)
{
    int err = E_OK;

    if (alct == NULL || addr == NULL || size == NULL)
    {
        return E_INVAL;
    }

    alct->head = (struct list_node_t *)addr;

    addr_t slot_addr = addr + CONFIG_NR_BUDDY_ORDER * sizeof(struct page_t);
    ;

    if (slot_addr > addr + size)
    {
        return E_NOMEM;
    }

    size_t slot_size = addr + size - slot_addr;

    slot_init(&(alct->slot_alct), sizeof(struct page_t) + sizeof(struct list_node_t));

    slot_insert(&(alct->slot_alct), slot_addr, slot_size);

    for (int i = 0; i < CONFIG_NR_BUDDY_ORDER; ++i)
    {
        list_init(&alct->head[i]);
    }

    return err;
}