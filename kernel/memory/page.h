#ifndef _KERNEL_MEMORY_PAGE_H_
#define _KERNEL_MEMORY_PAGE_H_
#include <arch/basic.h>

#define PAGE_PMM 1
#define PAGE_VMM 2
#define PAGE_TYPE_MASK 3

struct page_t
{
    uint flag;
    addr_t addr;
    size_t num;
    addr_t map_addr;
    struct buddy_t
    {
        size_t order;
        void *f_node, *b_node;
        u32 flag;
    } buddy;
};

#endif