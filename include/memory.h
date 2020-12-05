#ifndef _MEMORY_H_
#define _MEMORY_H_
#include <arch/basic.h>
#include <boot_arg.h>

int memory_init(addr_t free_pmm_start, size_t free_pmm_size, addr_t free_kvm_start, size_t free_kvm_size);

int kalloc(addr_t *addr, size_t size);

int kfree(addr_t addr);

int page_alloc(addr_t *addr);

#endif