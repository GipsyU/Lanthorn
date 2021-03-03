#include <list.h>
#include <log.h>

void list_init(struct list_node_t *head)
{
    head->n = head;

    head->p = head;
}

int list_isempty(struct list_node_t *head)
{
    /**
     * test
     */
    if (head->n == head)
    {
        assert(head->p == head);

        return 1;
    }

    return 0;
}

struct list_node_t *list_front(struct list_node_t *head)
{
    return head->n;
}

struct list_node_t *list_back(struct list_node_t *head)
{
    return head->p;
}

/**
 * WARNING: Avoid insert one node twice, it's very dangerous.
 */
void list_push_front(struct list_node_t *head, struct list_node_t *node)
{
    node->n = head->n;

    node->p = head;

    head->n->p = node;

    head->n = node;
}

void list_push_back(struct list_node_t *head, struct list_node_t *node)
{
    node->p = head->p;

    node->n = head;

    head->p->n = node;

    head->p = node;
}

void list_delete(struct list_node_t *node)
{
    node->p->n = node->n;

    node->n->p = node->p;
}

struct list_node_t *list_pop_front(struct list_node_t *head)
{
    struct list_node_t *front = head->n;

    head->n = front->n;

    front->n->p = head;

    return front;
}

struct list_node_t *list_pop_back(struct list_node_t *head)
{
    struct list_node_t *back = head->p;

    head->p = back->p;

    back->p->n = head;

    return back;
}