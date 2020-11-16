#include <error.h>
#include <list.h>
#include <log.h>
#include <util.h>
#include <memory.h>

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

#define BUDDY_TPX(TPX)                                       \
                                                             \
    static inline u32 GET_##TPX(buddy_node_t *node)          \
    {                                                        \
        return (node)->data.flag & BUDDY_##TPX##_MASK;       \
    }                                                        \
                                                             \
    static inline int SET_##TPX(buddy_node_t *node, u32 val) \
    {                                                        \
        (node)->data.flag &= (~BUDDY_##TPX##_MASK);          \
        (node)->data.flag |= val;                            \
        return E_OK;                                         \
    }

BUDDY_TPX(BUD)

BUDDY_TPX(ATTR)

BUDDY_TPX(STATE)


/*
 * FIXME: no free cache of list node.
 */

static int new_node(struct buddy_allocator_t *alct, buddy_node_t **node)
{
    if (list_empty(alct->free_slot_head))
    {
        // TODO: FIX it
        warn("NO CAche %d\n", alct->free_slot_num);
        return E_NOCACHE;
    }

    *node = (typeof(*node))list_pop_front(alct->free_slot_head);

    SET_BUD(*node, BUDDY_NO_BUD);

    SET_ATTR(*node, BUDDY_NO_USE);

    --(alct->free_slot_num);

    return E_OK;
}

static int free_node(struct buddy_allocator_t *alct, buddy_node_t *node)
{
    list_push_back(alct->free_slot_head, *node);

    ++(alct->free_slot_num);

    return E_OK;
}

static int merge_node(struct buddy_allocator_t *alct, buddy_node_t **f_node, buddy_node_t *node1, buddy_node_t *node2)
{
    int err = E_OK;

    if (f_node == NULL || node1 == NULL || node2 == NULL)
    {
        return E_INVAL;
    }

    if ((GET_BUD(node1) != BUDDY_LEFT || GET_BUD(node2) != BUDDY_RIGHT) && (GET_BUD(node1) != BUDDY_RIGHT || GET_BUD(node2) != BUDDY_LEFT))
    {
        return E_INVAL;
    }

    if (GET_STATE(node1) != BUDDY_ALLOCED || GET_STATE(node2) != BUDDY_ALLOCED)
    {
        return E_INVAL;
    }

    if (node1->data.b_node != node2 || node2->data.b_node != node1)
    {
        return E_INVAL;
    }

    if (node1->data.f_node != node2->data.f_node)
    {
        return E_INVAL;
    }

    *f_node = node1->data.f_node;

    if (GET_ATTR(*f_node) != BUDDY_VIRT || GET_ATTR(node1) != BUDDY_REAL || GET_ATTR(node2) != BUDDY_REAL)
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

    SET_ATTR(*f_node, BUDDY_REAL);

    SET_STATE(*f_node, BUDDY_ALLOCED);

    return err;
}

static int divide_node(buddy_node_t *f_node, buddy_node_t *l_node, buddy_node_t *r_node)
{
    int err = E_OK;

    if (f_node == NULL || l_node == NULL || r_node == NULL)
    {
        return E_INVAL;
    }

    if (GET_ATTR(f_node) != BUDDY_REAL || f_node->data.order == 0)
    {
        return E_INVAL;
    }

    l_node->data.addr = f_node->data.addr;

    l_node->data.order = f_node->data.order - 1;

    r_node->data.addr = l_node->data.addr + (1 << l_node->data.order) * PAGE_SIZE;

    r_node->data.order = l_node->data.order;

    l_node->data.f_node = r_node->data.f_node = f_node;

    l_node->data.b_node = r_node;

    r_node->data.b_node = l_node;

    SET_BUD(l_node, BUDDY_LEFT);

    SET_BUD(r_node, BUDDY_RIGHT);

    SET_ATTR(l_node, BUDDY_REAL);

    SET_ATTR(r_node, BUDDY_REAL);

    SET_STATE(l_node, BUDDY_ALLOCED);

    SET_STATE(r_node, BUDDY_ALLOCED);

    SET_ATTR(f_node, BUDDY_VIRT);

    return err;
}

static int get_buddy(struct buddy_allocator_t *alct, uint order, buddy_node_t **node)
{
    int err = E_OK;

    if (alct == NULL || order >= CONFIG_NR_BUDDY_ORDER || node == NULL)
    {
        return E_INVAL;
    }

    if (list_empty(alct->head[order]))
    {
        return E_NOMEM;
    }

    *node = list_pop_front(alct->head[order]);

    SET_STATE(*node, BUDDY_ALLOCED);

    return err;
}

static int get_ptc_buddy(buddy_node_t *node)
{
    int err = E_OK;

    if (node == NULL || GET_ATTR(node) != BUDDY_REAL || GET_STATE(node) != BUDDY_FREE)
    {
        return E_INVAL;
    }

    list_delete(node);

    SET_STATE(node, BUDDY_ALLOCED);

    return err;
}

static int put_buddy(struct buddy_allocator_t *alct, buddy_node_t *node)
{
    int err = E_OK;

    if (node->data.order < 0 || node->data.order >= CONFIG_NR_BUDDY_ORDER)
    {
        return E_INVAL;
    }

    if (GET_ATTR(node) != BUDDY_REAL || GET_STATE(node) != BUDDY_ALLOCED)
    {
        return E_INVAL;
    }

    info("put memory into buddy sys, addr:%p, order:%d.\n", node->data.addr, node->data.order);

    for (buddy_node_t *f_node; GET_BUD(node) != BUDDY_NO_BUD; node = f_node)
    {
        err = get_ptc_buddy(node->data.b_node);

        if (err != E_OK)
        {
            err = E_OK;

            break;
        }

        err = merge_node(alct, &f_node, node, node->data.b_node);

        if (err != E_OK)
        {
            err = E_OK;

            break;
        }
    }

    SET_STATE(node, BUDDY_FREE);

    list_push_back(alct->head[node->data.order], *node);

    return err;
}

static int buddy_node_alloc(struct buddy_allocator_t *alct, uint order, buddy_node_t **node)
{
    int err = E_OK;

    if (order >= CONFIG_NR_BUDDY_ORDER)
    {
        return E_NOMEM;
    }

    err = get_buddy(alct, order, node);

    if (err != E_OK)
    {
        buddy_node_t *f_node, *l_node, *r_node;

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
            buddy_node_t *node;

            err = new_node(alct, &node);

            if (err != E_OK)
            {
                /*
                 * FIXME: here need to release inserted before.
                 */
                return err;
            }

            node->data.addr = start;

            node->data.order = i;

            SET_BUD(node, BUDDY_NO_BUD);

            SET_ATTR(node, BUDDY_REAL);

            SET_STATE(node, BUDDY_ALLOCED);

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

int buddy_alloc(struct buddy_allocator_t *alct, size_t page_num, struct page_t *page)
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

    buddy_node_t *node;

    err = buddy_node_alloc(alct, order, &node);

    if (err != E_OK)
    {
        return err;
    }

    page->addr = node->data.addr;

    page->num = (1 << node->data.order);

    page->buddy = node;

    return err;
}

int buddy_free(struct buddy_allocator_t *alct, struct page_t *page)
{
    int err = E_OK;

    buddy_node_t *buddy = (buddy_node_t *)(page->buddy);

    err = put_buddy(alct, buddy);

    return err;
}

int buddy_init(struct buddy_allocator_t *alct, addr_t addr, size_t size)
{
    int err = E_OK;

    if (alct == NULL || addr == NULL || size == NULL || size % PAGE_SIZE != 0)
    {
        return E_INVAL;
    }

    alct->head = (buddy_node_t *)addr;

    alct->slot = (buddy_node_t *)(addr + CONFIG_NR_BUDDY_ORDER * sizeof(buddy_node_t));

    if ((addr_t)alct->slot >= addr + size)
    {
        return E_NOMEM;
    }

    alct->max_slot_num = (addr + size - (addr_t)alct->slot) / sizeof(buddy_node_t);

    alct->free_slot_num = alct->max_slot_num;

    list_init(alct->free_slot_head);

    for (int i = 0; i < alct->max_slot_num; ++i)
    {
        list_push_back(alct->free_slot_head, alct->slot[i]);
    }

    for (int i = 0; i < CONFIG_NR_BUDDY_ORDER; ++i)
    {
        list_init(alct->head[i]);
    }

    return err;
}