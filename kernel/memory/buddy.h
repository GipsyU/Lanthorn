#ifndef _KERNEL_MEMORY_BUDDY_H_
#define _KERNEL_MEMORY_BUDDY_H_

#include <arch/basic.h>
#include <list.h>
#include <slot.h>
#include "page.h"

struct buddy_allocator_t
{
    char name[10];

    struct slot_alct_t slot_alct;

    struct list_node_t *head;
};

int buddy_insert(struct buddy_allocator_t *alct, struct page_t *page);

int buddy_alloc(struct buddy_allocator_t *alct, size_t page_num, struct page_t **page);

int buddy_free(struct buddy_allocator_t *alct, struct page_t *page);

int buddy_init(struct buddy_allocator_t *alct, addr_t addr, size_t size);

#endif