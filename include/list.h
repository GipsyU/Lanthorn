#ifndef _list_H_

#define _list_H_

#include <arch/basic.h>

struct list_node_t
{
    struct list_node_t *p, *n;
};

void list_init(struct list_node_t *head);

addr_t list_data(struct list_node_t *node);

int list_isempty(struct list_node_t *head);

struct list_node_t *list_front(struct list_node_t *head);

struct list_node_t *list_back(struct list_node_t *head);

void list_push_front(struct list_node_t *head, struct list_node_t *node);

void list_push_back(struct list_node_t *head, struct list_node_t *node);

void list_delete(struct list_node_t *node);

struct list_node_t *list_pop_front(struct list_node_t *head);

struct list_node_t *list_pop_back(struct list_node_t *head);

#define list_node_size(type) (                    \
    {                                              \
        sizeof(type) + sizeof(struct list_node_t); \
    })

/**
 * WARNING: head should be a ptr
 */

#define list_rep(head, p) \
    for (struct list_node_t * (p) = (head)->n; (p) != head; (p) = (p)->n)

#define list_rep_s(head, p) \
    for (struct list_node_t * (p) = (head)->n, *pp = (p)->n; (p) != head; (p) = pp, pp = pp->n)

#endif