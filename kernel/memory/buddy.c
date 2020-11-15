#include <error.h>
#include <boot_arg.h>
#include <list.h>
#include <log.h>
#include <util.h>
#include <memory.h>

struct buddy_t
{
    addr_t addr;
    size_t order;
    void *f_node, *b_node;
    u32 flag;
};

typedef list_node(struct buddy_t) buddy_node_t;

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

static list_node(buddy_node_t) free_cache_head;

#define BUDDY_CACHE_NUM (PAGE_SIZE * CONFIG_NR_BUDDY_CACHE_PG / sizeof(buddy_node_t))

static buddy_node_t buddy_cache[BUDDY_CACHE_NUM];

static buddy_node_t buddy_head[CONFIG_NR_BUDDY_ORDER];

static size_t free_cache_num = BUDDY_CACHE_NUM;

/*
 * FIXME: no free cache of list node.
 */

static int new_node(buddy_node_t **node)
{
    if (list_empty(free_cache_head))
    {
        // TODO: FIX it
        warn("NO CAche %d\n", free_cache_num);
        return E_NOCACHE;
    }

    *node = (typeof(*node))list_pop_front(free_cache_head);

    SET_BUD(*node, BUDDY_NO_BUD);

    SET_ATTR(*node, BUDDY_NO_USE);

    --free_cache_num;

    return E_OK;
}

static int free_node(buddy_node_t *node)
{
    list_push_back(free_cache_head, *node);

    ++free_cache_num;

    return E_OK;
}

static int merge_node(buddy_node_t **f_node, buddy_node_t *node1, buddy_node_t *node2)
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

    err = free_node(node1);

    if (err != E_OK)
    {
        return err;
    }

    err = free_node(node2);

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

static int get_buddy(int order, buddy_node_t **node)
{
    int err = E_OK;

    if (order < 0 || order >= CONFIG_NR_BUDDY_ORDER)
    {
        return E_INVAL;
    }

    if (list_empty(buddy_head[order]))
    {
        return E_NOMEM;
    }

    *node = list_pop_front(buddy_head[order]);

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

static int put_buddy(buddy_node_t *node)
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

        err = merge_node(&f_node, node, node->data.b_node);

        if (err != E_OK)
        {
            err = E_OK;

            break;
        }
    }

    SET_STATE(node, BUDDY_FREE);

    list_push_back(buddy_head[node->data.order], *node);

    return err;
}

static int buddy_node_alloc(int order, buddy_node_t **node)
{
    int err = E_OK;

    if (order >= CONFIG_NR_BUDDY_ORDER)
    {
        return E_NOMEM;
    }

    err = get_buddy(order, node);

    if (err != E_OK)
    {
        buddy_node_t *f_node, *l_node, *r_node;

        err = buddy_node_alloc(order + 1, &f_node);

        if (err != E_OK)
        {
            return err;
        }

        new_node(&l_node);

        new_node(&r_node);

        err = divide_node(f_node, l_node, r_node);

        if (err != E_OK)
        {
            free_node(l_node);

            free_node(r_node);

            put_buddy(f_node);

            return err;
        }

        err = put_buddy(r_node);

        *node = l_node;
    }

    return err;
}

int buddy_alloc(size_t page_num, struct page_t *page)
{

    int err = E_OK;

    if (page_num == 0 || page == NULL)
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

    err = buddy_node_alloc(order, &node);

    if (err != E_OK)
    {
        return err;
    }

    page->addr = node->data.addr;

    page->num = (1 << node->data.order);

    page->buddy = node;

    return err;
}

int buddy_free(struct page_t *page)
{
    int err = E_OK;

    buddy_node_t *buddy = (buddy_node_t *)(page->buddy);

    err = put_buddy(buddy);

    return err;
}

static int init_add_mm(addr_t addr, size_t buddy_num)
{
    buddy_node_t *buddy_node;

    new_node(&buddy_node);

    buddy_node->data.addr = addr;

    buddy_node->data.order = buddy_num;

    SET_BUD(buddy_node, BUDDY_NO_BUD);

    SET_ATTR(buddy_node, BUDDY_REAL);

    SET_STATE(buddy_node, BUDDY_ALLOCED);

    put_buddy(buddy_node);

    return E_OK;
}

static int init_insert_mm(addr_t addr, size_t size)
{
    int err = E_OK;

    addr_t _start = ROUND_UP(addr, PAGE_SIZE);

    addr_t _end = ROUND_DOWN(addr + size, PAGE_SIZE);

    size_t _size = _end - _start;

    assert(_size >= 0 && _size % PAGE_SIZE == 0, "insert _size error.\n");

    if (_size == 0)
    {
        warn("No aligned 4K memory.\n");

        return E_NOMEM;
    }

    size_t page_num = _size / PAGE_SIZE;

    for (int i = 0; i < CONFIG_NR_BUDDY_ORDER; ++i)
    {
        if (page_num & (1 << i))
        {
            err = init_add_mm(_start, i);

            _start += (1 << i) * PAGE_SIZE;
        }
    }
    assert(_start == _end, "buddy mm insert bug.\n");

    return err;
}

int buddy_init(boot_mm_list_node_t *mm_list)
{
    int err = E_OK;

    list_init(free_cache_head);

    for (int i = 0; i < BUDDY_CACHE_NUM; ++i)
    {
        list_push_back(free_cache_head, buddy_cache[i]);
    }

    for (int i = 0; i < CONFIG_NR_BUDDY_ORDER; ++i)
    {
        list_init(buddy_head[i]);
    }

    /*
     * FIXME: ERR
     */

    list_rep(*mm_list, p)
    {
        init_insert_mm(p->data.addr, p->data.size);
    }

    return err;
}
