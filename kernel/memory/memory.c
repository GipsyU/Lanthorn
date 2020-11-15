#include <memory.h>
#include <boot_arg.h>
#include <error.h>
#include <log.h>

extern int buddy_init(boot_mm_list_node_t *list_node);

extern int buddy_alloc(size_t page_num, struct page_t *page);

extern int buddy_free(struct page_t *page);

static int print_memory_info(boot_mm_list_node_t *mm_list_node)
{
    int err = E_OK;

    info("available memory:\n");

    list_rep (*mm_list_node, p) {
        info("free memory start addr: %p, end addr: %p.\n", p->data.addr, p->data.addr + p->data.size - 1);
    }

    return err;
}

struct page_t page[200];
int memory_init(boot_mm_list_node_t *mm_list_node)
{
    int err = E_OK;

    err = print_memory_info(mm_list_node);

    buddy_init(mm_list_node);


    for (int i=1;i<=100;++i)
    {
        err = buddy_alloc(11, &page[i]);
        info("%d %p %d %s\n",i, page[i].addr, page[i].num,strerror(err));
    }

    for(int i=1;i<=100;++i)
    {
        err = buddy_free(&page[i]);
        info("%s\n",strerror(err))
    }
    return err;
}

// int pmm_alloc(size_t size, addr_t *pmm_addr)
// {
//     int err = E_OK;
    
//     return err;
// }