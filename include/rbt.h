#ifndef _RBT_H_

#define _RBT_H_

#include <basic.h>

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

// int rbt_init()


// int rbt_insert(struct rbt_t *rbt, struct rbt_node_t *node);

// int rbt_find()


#endif