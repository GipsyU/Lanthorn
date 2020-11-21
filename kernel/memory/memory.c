#include <arch/mmu.h>
#include <memory.h>
#include <boot_arg.h>
#include <error.h>
#include <log.h>
#include <util.h>

#include "buddy.h"
#include "page.h"

static struct buddy_allocator_t pmm_alct;

static struct buddy_allocator_t kvmm_alct;

static u8 pmm_slot[CONFIG_NR_PMM_BUDDY_SLOT_PG * PAGE_SIZE];

static u8 kvmm_slot[CONFIG_NR_KVMM_BUDDY_SLOT_PG * PAGE_SIZE];

static int print_memory_info(boot_mm_list_node_t *mm_list_node)
{
    int err = E_OK;

    info("available memory:\n");

    list_rep(*mm_list_node, p)
    {
        info("free memory start addr: %p, end addr: %p.\n", p->data.addr, p->data.addr + p->data.size - 1);
    }

    return err;
}

struct page_t Page[200];

int memory_init(boot_mm_list_node_t *mm_list_node, addr_t free_kvm_start, size_t free_kvm_size)
{
    int err = E_OK;

    if (free_kvm_start % PAGE_SIZE != 0)
    {
        return E_INVAL;
    }

    err = print_memory_info(mm_list_node);

    buddy_init(&pmm_alct, (addr_t)pmm_slot, CONFIG_NR_PMM_BUDDY_SLOT_PG * PAGE_SIZE);

    list_rep(*mm_list_node, p)
    {
        struct page_t page;

        addr_t start = ROUND_UP(p->data.addr, PAGE_SIZE);

        addr_t end = ROUND_DOWN(p->data.addr + p->data.size, PAGE_SIZE);

        page.addr = start;

        page.num = (end - start) / PAGE_SIZE;

        err = buddy_insert(&pmm_alct, &page);

        if (err != E_OK)
        {
            /*
             * FIXME
             */
            panic("bug\n");
        }
    }

    info("pmm init finished.\n");

    buddy_init(&kvmm_alct, (addr_t)kvmm_slot, CONFIG_NR_KVMM_BUDDY_SLOT_PG * PAGE_SIZE);

    struct page_t page;

    page.addr = free_kvm_start;

    page.num = free_kvm_size / PAGE_SIZE;

    err = buddy_insert(&kvmm_alct, &page);

    // if (err != E_OK)
    // {
    //     panic("bug\n");
    // }
    // else
    // {
    //     info("kvmm init finished.\n");
    // }

    // err = buddy_alloc(&pmm_alct, 1, &Page[0]);
    // info("%d %p %d %s\n", 0, Page[0].addr, Page[0].num, strerror(err));

    // err = buddy_alloc(&kvmm_alct, 11, &Page[1]);
    // info("%d %p %d %s\n", 1, Page[1].addr, Page[1].num, strerror(err));

    // err = buddy_alloc(&pmm_alct, 1, &Page[2]);
    // info("%d %p %d %s\n", 2, Page[2].addr, Page[2].num, strerror(err));

    // err = mmu_map(Page[0].addr, Page[1].addr, Page[2].addr);

    // debug("%s\n",strerror(err));

    // debug("OK\n");

    // int *x = Page[1].addr;

    // debug("OK %p\n",x);

    // *x = 1;

    // debug("OK %p %d\n", x, *x);

    // for (int i = 1; i <= 100; ++i)
    // {
    //     struct page_t *p;
    //     err = buddy_alloc(&kvmm_alct, 11, &p);
    //     info("%d %p %d %s\n", i, p->addr, p->num, strerror(err));
    // }

    // for (int i = 1; i <= 100; ++i)
    // {
    //     err = buddy_free(&kvmm_alct, &Page[i]);
    //     info("%s\n", strerror(err))
    // }

    return err;
}

// int get_page()
// {

// }

// int put_page()
// {

// }

static int kalloc_buddy(addr_t *addr, size_t size)
{
    int err = E_OK;

    struct page_t *pp, *vp, *pte;

    err = buddy_alloc(&pmm_alct, ROUND_UP(size, PAGE_SIZE) / PAGE_SIZE, &pp);

    if (err != E_OK)
    {
        goto error0;
    }

    err = buddy_alloc(&kvmm_alct, ROUND_UP(size, PAGE_SIZE) / PAGE_SIZE, &vp);

    if (err != E_OK)
    {
        goto error1;
    }

    err = mmu_map(pp->addr, vp->addr, NULL);

    if (err != E_OK)
    {
        if (err == E_AGAIN)
        {
            err = buddy_alloc(&kvmm_alct, ROUND_UP(size, PAGE_SIZE) / PAGE_SIZE, &pte);

            if (err != E_OK)
            {
                goto error2;
            }

            err = mmu_map(pp->addr, vp->addr, pte->addr);

            if (err != E_OK)
            {
                goto error3;
            }
        }
        else
        {
            goto error2;
        }
    }

    *addr = vp->addr;

    return err;

error3:
    buddy_free(&pmm_alct, pte);

error2:
    buddy_free(&kvmm_alct, vp);

error1:
    buddy_free(&pmm_alct, pp);

error0:
    return err;
}

int kalloc(addr_t *addr, size_t size)
{
    int err = E_OK;

    if (addr == NULL || size == 0)
    {
        return E_INVAL;
    }

    if (size <= 128)
    {
    }
    else
    {
        err = kalloc_buddy(addr, ROUND_UP(size, PAGE_SIZE));
    }

    return err;
}

int kfree(addr_t addr)
{
    int err = E_OK;

    return err;
}