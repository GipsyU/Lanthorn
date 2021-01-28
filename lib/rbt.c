/**
 * REFER FROM LINUX
 */
#include <log.h>
#include <rbt.h>

static void rotate_left(struct rbt_t *rbt, struct rbt_node_t *node)
{
    struct rbt_node_t *r = node->r;

    struct rbt_node_t *f = node->f;

    if ((node->r = r->l))
    {
        r->l->f = node;
    }

    node->f = r;

    r->f = f;

    r->l = node;

    if (f != NULL)
    {
        if (node == f->l)
        {
            f->l = r;
        }
        else
        {
            f->r = r;
        }
    }
    else
    {
        rbt->root = r;
    }
}

static void rotate_right(struct rbt_t *rbt, struct rbt_node_t *node)
{
    struct rbt_node_t *l = node->l;

    struct rbt_node_t *f = node->f;

    if ((node->l = l->r))
    {
        l->r->f = node;
    }

    node->f = l;

    l->f = f;

    l->r = node;

    if (f != NULL)
    {
        if (node == f->l)
        {
            f->l = l;
        }
        else
        {
            f->r = l;
        }
    }
    else
    {
        rbt->root = l;
    }
}

void rbt_insert_color(struct rbt_t *rbt, struct rbt_node_t *node)
{
    struct rbt_node_t *f, *g;

    node->color = RED;

    while ((f = node->f) && f->color == RED)
    {
        g = f->f;

        if (f == g->l)
        {
            struct rbt_node_t *u = g->r;

            if (u && u->color == RED)
            {
                u->color = BLACK;

                f->color = BLACK;

                g->color = RED;

                node = g;

                continue;
            }

            if (f->r == node)
            {
                struct rbt_node_t *tmp;

                rotate_left(rbt, f);

                tmp = f;

                f = node;

                node = tmp;
            }

            f->color = BLACK;

            g->color = RED;

            rotate_right(rbt, g);
        }
        else
        {
            struct rbt_node_t *u = g->l;

            if (u && u->color == RED)
            {
                u->color = BLACK;

                f->color = BLACK;

                g->color = RED;

                node = g;

                continue;
            }

            if (f->l == node)
            {
                struct rbt_node_t *tmp;

                rotate_right(rbt, f);

                tmp = f;

                f = node;

                node = tmp;
            }

            f->color = BLACK;

            g->color = RED;

            rotate_left(rbt, g);
        }
    }

    rbt->root->color = BLACK;
}

static void rbt_delete_color(struct rbt_t *rbt, struct rbt_node_t *node, struct rbt_node_t *parent)
{
    struct rbt_node_t *other;

    while ((!node || node->color == BLACK) && node != rbt->root)
    {
        if (parent->l == node)
        {
            other = parent->r;

            if (other->color == RED)
            {
                other->color = BLACK;
                parent->color = RED;
                rotate_left(rbt, parent);
                other = parent->r;
            }
            if ((!other->l || other->l->color == BLACK) && (!other->r || other->r->color == BLACK))
            {
                other->color = RED;
                node = parent;
                parent = node->f;
            }
            else
            {
                if (!other->r || other->r->color == BLACK)
                {
                    other->l->color = BLACK;
                    other->color = RED;
                    rotate_right(rbt, other);
                    other = parent->r;
                }
                other->color = parent->color;
                parent->color = BLACK;
                other->r->color = BLACK;
                rotate_left(rbt, parent);
                node = rbt->root;
                break;
            }
        }
        else
        {
            other = parent->l;
            if (other->color == RED)
            {
                other->color = BLACK;
                parent->color = RED;
                rotate_right(rbt, parent);
                other = parent->l;
            }
            if ((!other->l || other->l->color == BLACK) && (!other->r || other->r->color == BLACK))
            {
                other->color = RED;
                node = parent;
                parent = node->f;
            }
            else
            {
                if (!other->l || other->l->color == BLACK)
                {
                    other->r->color = BLACK;
                    other->color = RED;
                    rotate_left(rbt, other);
                    other = parent->l;
                }
                other->color = parent->color;
                parent->color = BLACK;
                other->l->color = BLACK;
                rotate_right(rbt, parent);
                node = rbt->root;
                break;
            }
        }
    }

    if (node) node->color = BLACK;
}

struct rbt_node_t *rbt_next(const struct rbt_node_t *node)
{
    struct rbt_node_t *parent, *res;

    if (node->r)
    {
        node = node->r;

        while (node->l) node = node->l;

        return node;
    }

    while ((parent = node->f) && node == parent->r) node = parent;

    return parent;
}
struct rbt_node_t *rbt_prev(const struct rbt_node_t *node)
{
    struct rbt_node_t *parent;

    if (node->l)
    {
        node = node->l;

        while (node->r) node = node->r;

        return node;
    }

    while ((parent = node->f) && node == parent->l) node = parent;

    return parent;
}

void rbt_delete(struct rbt_t *rbt, struct rbt_node_t *node)
{
    struct rbt_node_t *s, *f;

    enum COLOR color;

    if (node->l == NULL)
    {
        s = node->r;
    }
    else if (node->r == NULL)
    {
        s = node->l;
    }
    else
    {
        struct rbt_node_t *old = node;

        node = node->r;

        while (node->l)
        {
            node = node->l;
        }

        if (old->f)
        {
            if (old->f->l == old)
            {
                old->f->l = node;
            }
            else
            {
                old->f->r = node;
            }
        }
        else
        {
            rbt->root = node;
        }

        s = node->r;

        f = node->f;

        color = node->color;

        if (f == old)
        {
            f = node;
        }
        else
        {
            if (s)
            {
                s->f = f;
            }

            f->l = s;

            node->r = old->r;

            old->r->f = node;
        }

        node->f = old->f;

        node->color = old->color;

        node->l = old->l;

        old->l->f = node;

        goto color;
    }

    f = node->f;

    color = node->color;

    if (s)
    {
        s->f = f;
    }

    if (f)
    {
        if (f->l == node)
        {
            f->l = s;
        }
        else
        {
            f->r = s;
        }
    }
    else
    {
        rbt->root = s;
    }

color:
    if (color == BLACK)
    {
        rbt_delete_color(rbt, s, f);
    }
}

void rbt_update(struct rbt_node_t *node, rbt_update_func func)
{
    struct rbt_node_t *f;

    while (1)
    {
        func(node);

        if ((f = node->f) == NULL)
        {
            return;
        }

        if (node == f->l && f->r)
        {
            func(f->r);
        }
        else if (f->l)
        {
            func(f->l);
        }

        node = f;
    }
}

void rbt_insert_update(struct rbt_node_t *node, rbt_update_func func)
{
    if (node->l)
    {
        node = node->l;
    }
    else if (node->r)
    {
        node = node->r;
    }

    rbt_update(node, func);
}

struct rbt_node_t *rbt_delete_update_begin(struct rbt_node_t *node)
{
    struct rbt_node_t *deepest;

    if (node->r == NULL && node->l == NULL)

        deepest = node->f;

    else if (node->r == NULL)

        deepest = node->l;

    else if (node->l == NULL)

        deepest = node->r;

    else
    {
        deepest = rbt_next(node);

        if (deepest->r)

            deepest = deepest->r;

        else if (deepest->f != node)

            deepest = deepest->f;
    }

    return deepest;
}

void rbt_delete_update_end(struct rbt_node_t *node, rbt_update_func func)
{
    if (node) rbt_update(node, func);
}

struct rbt_node_t *rbt_first(struct rbt_t *rbt)
{
    struct rbt_node_t *tmp = NULL, *res;

    res = rbt->root;

    if (res == NULL) return res;

    while ((tmp = rbt_prev(res)) != NULL) res = tmp;

    return res;
}