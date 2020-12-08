#ifndef _RBT_H_

#define _RBT_H_

#include <arch/basic.h>

struct rbt_t
{
    struct rbt_node_t *root;
};

enum COLOR {RED, BLACK};

struct rbt_node_t
{
    enum COLOR color;

    struct rbt_node_t *f, *l, *r;
};

static inline void rb_link_node(struct rbt_node_t *node, struct rbt_node_t *parent,
				struct rbt_node_t **link)
{
	node->f = parent;
	node->l = node->r = NULL;
	*link = node;
}

int rbt_insert_color(struct rbt_t *rbt, struct rbt_node_t *node);

// int rbt_init()

// int rbt_insert(struct rbt_t *rbt, struct rbt_node_t *node);

// int rbt_find()


#endif