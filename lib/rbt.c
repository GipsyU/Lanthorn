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
        rbt->root->f = r;
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
        rbt->root->f = l;
    }
}

int rbt_insert_color(struct rbt_t *rbt, struct rbt_node_t *node)
{
    struct rbt_node_t *f, *g;

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

            rotate_right(g, rbt);
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

            rotate_left(g, rbt);
        }
    }

    rbt->root->color = BLACK;
}
