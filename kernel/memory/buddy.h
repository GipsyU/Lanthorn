#ifndef _KERNEL_MEMORY_BUDDY_H_
#define _KERNEL_MEMORY_BUDDY_H_

#include <basic.h>
#include <list.h>

struct buddy_t
{
    addr_t addr;
    size_t order;
    void *f_node, *b_node;
    u32 flag;
};

typedef list_node(struct buddy_t) buddy_node_t;

struct buddy_allocator_t
{    
    uint max_slot_num;
    
    int free_slot_num;

    buddy_node_t *slot;

    buddy_node_t *head;

    list_node(buddy_node_t) free_slot_head;
};

int buddy_insert(struct buddy_allocator_t *alct, struct page_t *page);

int buddy_alloc(struct buddy_allocator_t *alct, size_t page_num, struct page_t *page);

int buddy_free(struct buddy_allocator_t *alct, struct page_t *page);

int buddy_init(struct buddy_allocator_t *alct, addr_t addr, size_t size);


#endif