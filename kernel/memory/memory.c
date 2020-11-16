#include <memory.h>
#include <boot_arg.h>
#include <error.h>
#include <log.h>
#include <util.h>

#include "buddy.h"

static struct buddy_allocator_t pmm_alct;

// static struct buddy_allocator_t kvmm_alct;

static u8 pmm_slot[CONFIG_NR_PMM_BUDDY_SLOT_PG * PAGE_SIZE];

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

struct page_t page[200];
int memory_init(boot_mm_list_node_t *mm_list_node)
{
    int err = E_OK;

    err = print_memory_info(mm_list_node);

    buddy_init(&pmm_alct, (addr_t)pmm_slot, CONFIG_NR_PMM_BUDDY_SLOT_PG * PAGE_SIZE);

    list_rep(*mm_list_node, p)
    {
        struct page_t page;

        addr_t start = ROUND_UP(p->data.addr, PAGE_SIZE);

        addr_t end = ROUND_DOWN(p->data.addr + p->data.size, PAGE_SIZE);

        page.addr = start;

        page.num = (end - start) / PAGE_SIZE;

        buddy_insert(&pmm_alct, &page);
    }

    for (int i = 1; i <= 100; ++i)
    {
        err = buddy_alloc(&pmm_alct, 11, &page[i]);
        info("%d %p %d %s\n", i, page[i].addr, page[i].num, strerror(err));
    }

    for (int i = 1; i <= 100; ++i)
    {
        err = buddy_free(&pmm_alct, &page[i]);
        info("%s\n", strerror(err))
    }
    return err;
}

// int pmm_alloc(size_t size, addr_t *pmm_addr)
// {
//     int err = E_OK;

//     return err;
// }