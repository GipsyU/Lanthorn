#include <arch/intr.h>
#include <arch/mmu.h>
#include <boot_arg.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <proc.h>
#include <slot.h>
#include <string.h>
#include <util.h>

static struct page_alct_t pm_alct;

static u8 pm_slot[CONFIG_NR_PMM_BUDDY_SLOT_PG * PAGE_SIZE];

static struct vpage_alct_t kvm_alct;

static u8 kvm_slot[CONFIG_NR_KVMM_BUDDY_SLOT_PG * PAGE_SIZE];

static struct slot_alct_t kvm_slot_alct;

static struct slab_alct_t slab_alct;

extern struct proc_t proc_0;

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
    slot_free(&kvm_slot_alct, addr);

    return 0;
}

static int page_fault_hdl(uint errno)
{
    addr_t errv = mmu_err_addr();

    info("page fault errno: %p.\n", errno);

    if ((errno & PF_P) == 0)
    {
        info("page fault: present, addr = %p.\n", errv);

        if (errv >= KERN_BASE)
        {
            mmu_sync_kern_space(proc_0.ptb.pde, mmu_get_pde(), errv, PAGE_SIZE);
        }
        else
        {
            um_page_fault_hdl(&(proc_now()->um), &(proc_now()->ptb), errv);
        }
    }
    else if (errno & PF_W)
    {
        info("page fault: write protect, addr = %p.\n", errv);

        int err = um_page_fault_hdl(&(proc_now()->um), &(proc_now()->ptb), errv);

        assert(err == E_OK);
    }
    else
    {
        error("page fault.\n");

        while (1)
            ;
    }

    return E_OK;
}

int memory_init(addr_t free_pmm_start, size_t free_pmm_size, addr_t free_kvm_start, size_t free_kvm_size,
                addr_t kern_pde)
{
    info("free pmm start addr: %p, end addr: %p.\n", free_pmm_start, free_pmm_start + free_pmm_size);

    int err = pm_init(&pm_alct, (addr_t)pm_slot, CONFIG_NR_PMM_BUDDY_SLOT_PG * PAGE_SIZE);

    if (err != E_OK) return err;

    addr_t start = ROUND_UP(free_pmm_start, PAGE_SIZE);

    size_t size = ROUND_DOWN(free_pmm_start + free_pmm_size, PAGE_SIZE) - start;

    err = pm_insert(&pm_alct, start, size);

    if (err != E_OK) return err;

    info("init pmm success.\n");

    ptb_init(&proc_0.ptb, kern_pde);

    info("init p0 ptb success.\n");

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

int kmalloc_page(struct vpage_t **vp, size_t size)
{
    struct page_t *pp;

    size = ROUND_UP(size, PAGE_SIZE);

    int err = pm_alloc(&pm_alct, size, &pp);

    if (err != E_OK) goto error0;

    err = vm_alloc(&kvm_alct, vp, size);

    if (err != E_OK) goto error1;

    (*vp)->type = KM_NORMAL;

    for (size_t s = 0; s < size; s += PAGE_SIZE)
    {
        err = ptb_map(&proc_0.ptb, (*vp)->addr + s, pp->addr + s, 0, 1);

        if (err != E_OK) goto error2;
    }

    (*vp)->map_page = pp;

    return err;

error2:
    vm_free(&kvm_alct, *vp);

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
        struct vpage_t *vp;

        err = kmalloc_page(&vp, ROUND_UP(size, PAGE_SIZE));

        *addr = vp->addr;
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

int page_get_ptr(addr_t pa, struct page_t **page)
{
    assert(pa % PAGE_SIZE == 0);

    return pm_get_page(&pm_alct, pa, page);
}

int page_alloc(struct page_t **page)
{
    int err = pm_alloc(&pm_alct, PAGE_SIZE, page);

    if (err != E_OK) return err;

    assert(atomic_read(&(*page)->cnt) == 0);

    atomic_add(&(*page)->cnt, 1);

    info("page alloc success, phy addr = %p.\n", (*page)->addr);

    return err;
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