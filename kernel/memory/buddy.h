#ifndef _KERNEL_MEMORY_BUDDY_H_
#define _KERNEL_MEMORY_BUDDY_H_

#include <basic.h>
#include <list.h>

#include "page.h"

struct buddy_allocator_t
{
    uint max_slot_num;

    int free_slot_num;

    page_node_t *slot;

    page_node_t *head;

    // fixme

    page_node_t free_slot_head;
};

int buddy_insert(struct buddy_allocator_t *alct, struct page_t *page);

int buddy_alloc(struct buddy_allocator_t *alct, size_t page_num, struct page_t **page);

int buddy_free(struct buddy_allocator_t *alct, struct page_t *page);

int buddy_init(struct buddy_allocator_t *alct, addr_t addr, size_t size);

#endif