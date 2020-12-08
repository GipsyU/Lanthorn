#ifndef _MEMORY_H_
#define _MEMORY_H_
#include <arch/basic.h>
#include <boot_arg.h>
#include <arch/atomic.h>
#include <rbt.h>

struct vpage_t
{
    addr_t addr;
    size_t num;
    struct rbt_node_t rbt_node;
};

struct page_t
{
    uint flag;
    addr_t addr;
    size_t num;
    addr_t map_addr;

    struct atomic_t cnt;
    struct buddy_t
    {
        size_t order;
        void *f_node, *b_node;
        u32 flag;
    } buddy;
};

int memory_init(addr_t free_pmm_start, size_t free_pmm_size, addr_t free_kvm_start, size_t free_kvm_size);

int kalloc(addr_t *addr, size_t size);

int kfree(addr_t addr);

int page_alloc(addr_t *addr);

int page_free(struct page_t *page);

// int page_get()

// int page_put()

#endif