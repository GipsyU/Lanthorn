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
    addr_t addr;
    size_t size;
    struct atomic_t cnt;
    struct buddy_t
    {
        size_t order;
        struct page_t *f, *b;
        struct list_node_t list_node;
        enum BUD_POS {BUD_NOBUD, BUD_LEFT, BUD_RIGHT} pos;
        enum BUD_STATE {BUD_FREE, BUD_ALLOCED, BUD_VIRT} state;
    } buddy;
};

struct page_alct_t
{
    struct slot_alct_t slot_alct;
    struct spinlock_t lock;
    struct list_node_t *head;
    struct rbt_t alloced_rbt;
};

struct vpage_t
{
    addr_t addr;
    size_t size;
    size_t mx_size;
    struct page_t *map_page;
    struct rbt_node_t rbt_node;
};

struct vpage_alct_t
{
    struct spinlock_t lock;
    struct slot_alct_t slot_alct;
    struct rbt_t free_rbt;
    struct rbt_t alloced_rbt;
};

struct slab_alct_t
{
    addr_t freep;

    struct spinlock_t lock;

    int (*alloc)(addr_t *, size_t);

    int (*free)(addr_t);
};

int memory_init(addr_t free_pmm_start, size_t free_pmm_size, addr_t free_kvm_start, size_t free_kvm_size);

int kmalloc(addr_t *addr, size_t size);

int kmfree(addr_t addr);

// int kvmalloc()

int page_alloc(struct page_t **page);

int page_free(struct page_t *page);

int page_get(struct page_t *page);

int page_put(struct page_t *page);

#endif