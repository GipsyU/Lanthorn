#ifndef _MEMORY_H_
#define _MEMORY_H_
#include <arch/basic.h>
#include <boot_arg.h>
#include <arch/atomic.h>
#include <rbt.h>
#include <spinlock.h>
#include <slot.h>

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

struct buddy_allocator_t
{
    char name[10];

    struct slot_alct_t slot_alct;

    struct spinlock_t lock;

    struct list_node_t *head;
};

struct vpage_t
{
    addr_t addr;
    size_t size;
    size_t mx_size;
    struct rbt_node_t rbt_node;
};

struct vpage_alct_t
{
    struct spinlock_t lock;
    struct slot_alct_t slot_alct;
    struct rbt_t free_rbt;
    struct rbt_t alloced_rbt;
};

int memory_init(addr_t free_pmm_start, size_t free_pmm_size, addr_t free_kvm_start, size_t free_kvm_size);

int kmalloc(addr_t *addr, size_t size);

int kmfree(addr_t addr);

// int kvmalloc()

int page_alloc(addr_t *addr);

int page_free(struct page_t *page);


// int page_get()

// int page_put()

#endif