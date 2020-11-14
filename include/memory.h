#ifndef _MEMORY_H_
#define _MEMORY_H_
#include <basic.h>
#include <boot_arg.h>
struct page_t {
    addr_t addr;
    size_t num;
    void *buddy;
};

int memory_init(boot_mm_list_node_t *mm_list_node);

int pmm_alloc(addr_t *pmm_addr);

#endif