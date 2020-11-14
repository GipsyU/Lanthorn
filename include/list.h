#ifndef _LIST_H_
#define _LIST_H_

#define list_node(data_t) \
    struct { \
        void *p; \
        void *n; \
        data_t data; \
    }

#define list_init(head) \
{\
    (head).p = &(head); \
    (head).n = &(head); \
}

#define list_empty(head) ( \
{ \
    ((head).n == &(head)); \
} \
)

#define list_front(head) ( \
{ \
    (typeof(head) *)((head).n); \
} \
)

#define list_back(head) ( \
{ \
    (typeof(head) *)((head).p); \
} \
)

/*
 * WARNING: Avoid insert one node twice, it's very dangerous.
 */

#define list_push_front(head, node) \
{ \
    (node).n = (head).n; \
    (node).p = &(head); \
    ((typeof(head)*)((head).n))->p = &(node); \
    (head).n = &(node); \
}

#define list_push_back(head, node) \
{ \
    (node).p = (head).p; \
    (node).n = &(head); \
    ((typeof(head)*)((head).p))->n = &(node); \
    (head).p = &(node); \
}

#define list_delete(node) \
{ \
    ((typeof(node))((node)->p))->n = (node)->n; \
    ((typeof(node))((node)->n))->p = (node)->p; \
}

#define list_pop_front(head) ( \
{ \
    typeof(head) *front = (typeof(head) *)((head).n); \
    (head).n = front->n; \
    ((typeof(head) *)(front->n))->p = &(head); \
    front; \
} \
)

#define list_pop_back(head) ( \
{ \
    typeof(head) *back = (typeof(head) *)((head).p); \
    (head).p = back->p; \
    ((typeof(head) *)(back->p))->n = &(head); \
    back; \
} \
)

#define list_rep(head, p) \
    for (typeof(head) *p = (head).n; p != &(head); p = p->n)

// #define list_rep_s(head, p)

#endif