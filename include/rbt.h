#ifndef _RBT_H_
#define _RBT_H_

#include <arch/basic.h>

struct rbt_t
{
    struct rbt_node_t *root;
};

enum COLOR
{
    RED,
    BLACK
};

struct rbt_node_t
{
    enum COLOR color;

    struct rbt_node_t *f, *l, *r;
};

typedef void (*rbt_update_func)(struct rbt_node_t *node);

static inline void rb_link_node(struct rbt_node_t *node, struct rbt_node_t *parent, struct rbt_node_t **link)
{
    node->f = parent;
    node->l = node->r = NULL;
    *link = node;
}

void rbt_insert_color(struct rbt_t *rbt, struct rbt_node_t *node);

void rbt_delete(struct rbt_t *rbt, struct rbt_node_t *node);

void rbt_update(struct rbt_node_t *node, rbt_update_func func);

void rbt_insert_update(struct rbt_node_t *node, rbt_update_func func);

struct rbt_node_t *rbt_delete_update_begin(struct rbt_node_t *node);

void rbt_delete_update_end(struct rbt_node_t *node, rbt_update_func func);

struct rbt_node_t *rbt_next(const struct rbt_node_t *node);

struct rbt_node_t *rbt_prev(const struct rbt_node_t *node);

struct rbt_node_t *rbt_first(struct rbt_t *rbt);

#define rbt_rep(rbt, p) for (struct rbt_node_t *p = rbt_first(rbt); p != NULL; p = rbt_next(p))

#endif