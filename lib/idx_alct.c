#include <error.h>
#include <idx_aclt.h>
#include <log.h>
#include <util.h>

struct idx_node_t
{
    int id;
    addr_t addr;
    int rbt_node_sum;
    struct rbt_node_t rbt_node;
};

static void update(struct rbt_node_t *node)
{
    struct idx_node_t *idx = container_of(node, struct idx_node_t, rbt_node);

    uint sum = 1;

    if (node->l)
    {
        sum += (container_of(node->l, struct idx_node_t, rbt_node))->rbt_node_sum;
    }
    if (node->r)
    {
        sum += (container_of(node->l, struct idx_node_t, rbt_node))->rbt_node_sum;
    }

    idx->rbt_node_sum = sum;
}

static void _rbt_insert(struct rbt_t *rbt, struct idx_node_t *idx)
{
    struct rbt_node_t **p = &rbt->root;

    struct rbt_node_t *parent = NULL;

    struct idx_node_t *_idx;

    while (*p)
    {
        parent = *p;

        _idx = container_of(parent, struct idx_node_t, rbt_node);

        if (idx->id < _idx->id)
            p = &(*p)->l;

        else if (idx->id > _idx->id)
            p = &(*p)->r;

        else
            panic("idx tree bug.\n");
    }

    rb_link_node(&idx->rbt_node, parent, p);
}

static void rbt_insert(struct rbt_t *rbt, struct idx_node_t *idx)
{
    _rbt_insert(rbt, idx);

    rbt_insert_color(rbt, &idx->rbt_node);

    rbt_insert_update(&idx->rbt_node, update);
}

static int rbt_find(struct rbt_t *rbt, int id, struct idx_node_t **res)
{
    struct rbt_node_t *n = rbt->root;

    while (n)
    {
        struct idx_node_t *idx = container_of(n, struct idx_node_t, rbt_node);

        if (id < idx->id)
            n = n->l;

        else if (id > idx->id)
            n = n->r;

        else
        {
            *res = idx;

            return E_OK;
        }
    }

    return E_NOTFOUND;
}

static int rbt_find_free_id(struct rbt_t *rbt)
{
    struct rbt_node_t *n = rbt->root;

    if (n == NULL) return 0;

    int base = 0;

    while (n)
    {
        struct idx_node_t *idx = container_of(n, struct idx_node_t, rbt_node);

        if (n->l)
        {
            assert(container_of(n->l, struct idx_node_t, rbt_node)->rbt_node_sum + base <= idx->id);

            if (container_of(n->l, struct idx_node_t, rbt_node)->rbt_node_sum + base < idx->id)
            {
                n = n->l;

                continue;
            }
        }
        else if (base < idx->id)
            return base;

        if (n->r)
        {
            base = idx->id + 1;

            n = n->r;
        }
        else
            return idx->id + 1;
    }

    panic("bug.\n");
}

int idx_alct_new(struct idx_alct_t *alct, addr_t addr, int *id)
{
    mutex_lock(&alct->mutex);

    int _id = rbt_find_free_id(&alct->rbt);

    struct idx_node_t *idx = NULL;

    int err = kmalloc((void *)&idx, sizeof(struct idx_node_t));

    if (err != E_OK) goto ret1;

    idx->id = _id;

    idx->addr = addr;

    rbt_insert(&alct->rbt, idx);

    if (id != NULL) *id = _id;

ret1:
    mutex_unlock(&alct->mutex);

    return err;
}

int idx_alct_find(struct idx_alct_t *alct, int id, addr_t *addr)
{
    mutex_lock(&alct->mutex);

    struct idx_node_t *idx;

    int err = rbt_find(&alct->rbt, id, &idx);

    if (err != E_OK) goto ret1;

    if (addr != NULL) *addr = idx->addr;

ret1:
    mutex_unlock(&alct->mutex);

    return err;
}

int idx_alct_delete(struct idx_alct_t *alct, int id)
{
    mutex_lock(&alct->mutex);

    struct idx_node_t *idx;

    int err = rbt_find(&alct->rbt, id, &idx);

    if (err != E_OK) goto ret1;

    struct rbt_node_t *deepset = rbt_delete_update_begin(&idx->rbt_node);

    rbt_delete(&alct->rbt, &idx->rbt_node);

    rbt_delete_update_end(deepset, update);

ret1:

    mutex_unlock(&alct->mutex);

    return err;
}

int idx_aclt_init(struct idx_alct_t *alct)
{
    rbt_init(&alct->rbt);

    mutex_init(&alct->mutex);

    return E_OK;
}