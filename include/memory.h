#ifndef _MEMORY_H_
#define _MEMORY_H_
#include <basic.h>
#include <boot_arg.h>
#define PAGE_PMM 1
#define PAGE_VMM 2
#define PAGE_TYPE_MASK 3
struct page_t {
    addr_t addr;
    size_t num;
    addr_t xaddr;
    uint flag;
    void *buddy;
    struct page_t *pde;
};

int memory_init(boot_mm_list_node_t *mm_list_node, addr_t free_kvm_start, size_t free_kvm_size);

int pmm_alloc(addr_t *pmm_addr);

#endif