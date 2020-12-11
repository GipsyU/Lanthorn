#include <arch/mmu.h>
#include <boot_arg.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <string.h>
#include <util.h>

static struct page_alct_t pm_alct;

static u8 pm_slot[CONFIG_NR_PMM_BUDDY_SLOT_PG * PAGE_SIZE];

static struct vpage_alct_t kvm_alct;

static u8 kvm_slot[CONFIG_NR_KVMM_BUDDY_SLOT_PG * PAGE_SIZE];

static struct slab_alct_t slab_alct;

extern int vm_init(struct vpage_alct_t *alct);

extern int vm_insert(struct vpage_alct_t *alct, addr_t addr, size_t size);

extern int vm_alloc(struct vpage_alct_t *alct, struct vpage_t **vp, size_t size);

extern int vm_free(struct vpage_alct_t *alct, struct vpage_t *vp);

extern int vm_search_addr(struct vpage_alct_t *alct, addr_t addr, struct vpage_t **res);

extern int pm_insert(struct page_alct_t *alct, addr_t addr, size_t size);

extern int pm_alloc(struct page_alct_t *alct, size_t size, struct page_t **page);

extern int pm_free(struct page_alct_t *alct, struct page_t *page);

extern int pm_init(struct page_alct_t *alct, addr_t addr, size_t size);

extern int slab_alloc(struct slab_alct_t *alct, addr_t *addr, size_t size);

extern int slab_free(struct slab_alct_t *alct, addr_t addr);

extern int slab_init(struct slab_alct_t *alct, int (*alloc)(addr_t *, size_t), int (*free)(addr_t));

int memory_init(addr_t free_pmm_start, size_t free_pmm_size, addr_t free_kvm_start, size_t free_kvm_size)
{
    int err = E_OK;

    info("free pmm start addr: %p, end addr: %p.\n", free_pmm_start, free_pmm_start + free_pmm_size);

    err = pm_init(&pm_alct, (addr_t)pm_slot, CONFIG_NR_PMM_BUDDY_SLOT_PG * PAGE_SIZE);

    if (err != E_OK) return err;

    addr_t start = ROUND_UP(free_pmm_start, PAGE_SIZE);

    size_t size = ROUND_DOWN(free_pmm_start + free_pmm_size, PAGE_SIZE) - start;

    err = pm_insert(&pm_alct, start, size);

    if (err != E_OK) return err;
    
    info("init pmm success.\n")

    info("free kvm start addr: %p, end addr: %p.\n", free_kvm_start, free_kvm_start + free_kvm_size);

    start = ROUND_UP(free_kvm_start, PAGE_SIZE);

    size = ROUND_DOWN(free_pmm_start + free_kvm_size, PAGE_SIZE) - start;

    err = vm_init(&kvm_alct);

    if (err != E_OK) return err;

    slot_insert(&kvm_alct.slot_alct, (addr_t)kvm_slot, CONFIG_NR_KVMM_BUDDY_SLOT_PG * PAGE_SIZE);

    err = vm_insert(&kvm_alct, start, size);

    if (err != E_OK) return err;
    
    info("init kvm success.\n");

    err = slab_init(&slab_alct, kmalloc, kmfree);

    if (err != E_OK) return err;

    info("init slab success.\n");

    return err;
}

static int kmalloc_page(addr_t *addr, size_t size)
{
    int err = E_OK;

    struct page_t *pp;

    struct vpage_t *vp;

    size = ROUND_UP(size, PAGE_SIZE);

    err = pm_alloc(&pm_alct, size / PAGE_SIZE, &pp);

    if (err != E_OK) goto error0;

    err = vm_alloc(&kvm_alct, &vp, size);

    if (err != E_OK) goto error1;

    err = mmu_kern_map(pp->addr, vp->addr);

    if (err != E_OK) goto error2;

    *addr = vp->addr;

    vp->map_page = pp;

    return err;

error2:
    vm_free(&kvm_alct, vp);

error1:
    pm_free(&pm_alct, pp);

error0:
    return err;
}

int kmalloc(addr_t *addr, size_t size)
{
    int err = E_OK;

    if (size == 0) return E_INVAL;

    if (size <= CONFIG_SZ_SLAB_ALLOC_MAX)
    {
        err = slab_alloc(&slab_alct, addr, size);
    }
    else
    {
        err = kmalloc_page(addr, ROUND_UP(size, PAGE_SIZE));
    }

    info("kmalloc success: addr = %p, size = %p.\n", *addr, size);

    return err;
}

int kmfree(addr_t addr)
{
    int err = E_OK;

    if (ROUND_DOWN(addr, PAGE_SIZE) == addr)
    {
        struct vpage_t *vp;

        err = vm_search_addr(&kvm_alct, addr, &vp);

        if (err != E_OK) return err;

        err = pm_free(&pm_alct, vp->map_page);

        if (err != E_OK) return err;

        err = vm_free(&kvm_alct, vp);
    }
    else
    {
        err = slab_free(&slab_alct, addr);
    }

    return err;
}

int page_alloc(struct page_t **page)
{
    return pm_alloc(&pm_alct, 1, page);
}

int page_free(struct page_t *page)
{
    return pm_free(&pm_alct, page);
}

int page_get(struct page_t *page)
{
    atomic_add(&page->cnt, 1);

    return E_OK;
}

int page_put(struct page_t *page)
{
    int err = E_OK;

    if (atomic_sub_and_test(&page->cnt, 1)) err = page_free(page);

    return err;
}
