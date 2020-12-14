#include <arch/intr.h>
#include <arch/mmu.h>
#include <boot_arg.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <string.h>
#include <util.h>
#include <slot.h>

static struct page_alct_t pm_alct;

static u8 pm_slot[CONFIG_NR_PMM_BUDDY_SLOT_PG * PAGE_SIZE];

static struct vpage_alct_t kvm_alct;

static u8 kvm_slot[CONFIG_NR_KVMM_BUDDY_SLOT_PG * PAGE_SIZE];

static struct slot_alct_t kvm_slot_alct;

static struct slab_alct_t slab_alct;

extern int pm_insert(struct page_alct_t *alct, addr_t addr, size_t size);

extern int pm_alloc(struct page_alct_t *alct, size_t size, struct page_t **page);

extern int pm_free(struct page_alct_t *alct, struct page_t *page);

extern int pm_init(struct page_alct_t *alct, addr_t addr, size_t size);

static int kvm_slot_alloc(addr_t *addr, size_t size)
{
    assert(size = sizeof(struct vpage_t));

    int err = slot_alloc(&kvm_slot_alct, addr);

    if (err != E_OK)
    {
        /**
         * FIXME:NEW MM;
         */
    }

    return err;
}

static int kvm_slot_free(addr_t addr)
{
    return slot_free(&kvm_slot_alct, addr);
}

static int page_fault_hdl(uint errno)
{
    addr_t errv = mmu_err_addr();

    if ((errno & PF_P) == 0)
    {
        info("page fault: present, addr = %p.\n", errv);

        while(1);

        if (errv >= KERN_BASE)
        {
            mmu_sync_kern_space(mmu_get_pde(), errv);
        }
    }

    return E_OK;
}

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

    slot_init(&kvm_slot_alct, sizeof(struct vpage_t));

    slot_insert(&kvm_slot_alct, (addr_t)kvm_slot, CONFIG_NR_KVMM_BUDDY_SLOT_PG * PAGE_SIZE);

    vm_init(&kvm_alct, kvm_slot_alloc, kvm_slot_free);

    err = vm_insert(&kvm_alct, start, size);

    if (err != E_OK) return err;

    info("init kvm success.\n");

    err = slab_init(&slab_alct, kmalloc, kmfree);

    if (err != E_OK) return err;

    info("init slab success.\n");

    err = intr_register(INTR_PGFAULT, page_fault_hdl);

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

    for (size_t s = 0; s < size; s += PAGE_SIZE)
    {
        err = mmu_kern_map(pp->addr + s, vp->addr + s);
        
        if (err != E_OK) goto error2;
    }

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
    int err = pm_alloc(&pm_alct, 1, page);

    if (err != E_OK) return err;

    return page_get(*page);
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

int pagetb_init(struct pagetb_t *tb)
{
    spin_init(&tb->lock);

    tb->rbt.root = NULL;

    return E_OK;
}
