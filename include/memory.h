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
    struct rbt_node_t rbt_node;
    struct spinlock_t lock;
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
    struct spinlock_t lock;
    enum TYPE {KM_NORMAL, UM_NORMAL, UM_NOPM, UM_RCU} type;
    struct page_t *map_page;
    struct rbt_node_t rbt_node;
};

struct vpage_alct_t
{
    struct spinlock_t lock;
    
    struct rbt_t free_rbt;
    struct rbt_t alloced_rbt;
    struct {
        int (*alloc)(addr_t *addr, size_t size);
        
        int (*free)(addr_t addr);
    }mm_ops;
};

struct slab_alct_t
{
    addr_t freep;

    struct spinlock_t lock;

    int (*alloc)(addr_t *, size_t);

    int (*free)(addr_t);
};

struct ptb_t
{
    addr_t pde;
    struct spinlock_t lock;
    struct rbt_t rbt;
};

struct uvm_layout_t
{
    addr_t exe_s;
    addr_t exe_e;
    addr_t heap_s;
    addr_t heap_e;
    addr_t free_s;
    addr_t free_e;
    addr_t stk_s;
    addr_t stk_e;
};

static struct uvm_layout_t init_uvmlo = {
    .exe_s = 0,
    .exe_e = 0,
    .heap_s = 0,
    .heap_e = 0,
    .free_s = 0,
    .free_e = KERN_BASE,
    .stk_s = KERN_BASE,
    .stk_e = KERN_BASE
};

struct um_t
{
    struct uvm_layout_t layout;
    struct vpage_alct_t vp_alct;
    struct slab_alct_t slab_alct;
};

int pm_insert(struct page_alct_t *alct, addr_t addr, size_t size);

int pm_alloc(struct page_alct_t *alct, size_t size, struct page_t **page);

int pm_free(struct page_alct_t *alct, struct page_t *page);

int pm_init(struct page_alct_t *alct, addr_t addr, size_t size);

int pm_get_page(struct page_alct_t *alct, addr_t addr, struct page_t **page);

int memory_init(addr_t free_pmm_start, size_t free_pmm_size, addr_t free_kvm_start, size_t free_kvm_size, addr_t pde);

int kmalloc_page(struct vpage_t **vp, size_t size);

int kmalloc(addr_t *addr, size_t size);

int kmfree(addr_t addr);

int page_alloc(struct page_t **page);

int page_free(struct page_t *page);

int page_get(struct page_t *page);

int page_put(struct page_t *page);

int page_get_ptr(addr_t pa, struct page_t **page);

int vm_init(struct vpage_alct_t *alct, int (*alloc)(addr_t *addr, size_t size), int (*free)(addr_t addr));

int vm_insert(struct vpage_alct_t *alct, addr_t addr, size_t size);

int vm_alloc(struct vpage_alct_t *alct, struct vpage_t **vp, size_t size);

int vm_free(struct vpage_alct_t *alct, struct vpage_t *vp);

int vm_search_addr(struct vpage_alct_t *alct, addr_t addr, struct vpage_t **res);

int vm_dump(struct vpage_alct_t *old_alct, struct vpage_alct_t *new_alct);

int vm_slice(struct vpage_alct_t *alct, struct vpage_t *vpage, addr_t addr, size_t size, struct vpage_t **res);

int umalloc(struct um_t *um, addr_t *addr, size_t size);

int um_stack_alloc(struct um_t *um, addr_t *addr, size_t size);

int um_heap_alloc(struct um_t *um, addr_t *addr, size_t size);

int um_init(struct um_t *um);

int um_dump(struct um_t *um_old, struct um_t *um_new);

int um_page_fault_hdl(struct um_t *um, struct ptb_t *ptb, addr_t errva);

int slab_alloc(struct slab_alct_t *alct, addr_t *addr, size_t size);

int slab_free(struct slab_alct_t *alct, addr_t addr);

int slab_init(struct slab_alct_t *alct, int (*alloc)(addr_t *, size_t), int (*free)(addr_t));

int slab_dump(struct slab_alct_t *slab_old, struct slab_alct_t *slab_new);

int ptb_map(struct ptb_t *ptb, addr_t va, addr_t pa, uint usr, uint wtb);

int ptb_init(struct ptb_t *tb, addr_t pde);

int ptb_dump(struct ptb_t *ptbo, struct ptb_t *ptbn);

#endif