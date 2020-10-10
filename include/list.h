#ifndef _LIST_H_
#define _LIST_H_

#define list_node(data_t) \
            struct { \
                void *p; \
                void *n; \
                data_t data; \
            }

#define list_init(head) \
            head.p = &head; \
            head.n = &head;

#define list_append(head, node) \
            node.p = head.p; \
            node.n = &head; \
            ((typeof(head)*)head.p)->n = &node; \
            head.p = &node;

#define list_rep(head, p) \
            for (typeof(head) *p = head.n; p != &head; p = p->n)

// #define list_rep_s(head, p)

// #define list_insert()

// #define list_delete()


#endif