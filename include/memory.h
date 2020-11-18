#ifndef _MEMORY_H_
#define _MEMORY_H_
#include <basic.h>
#include <boot_arg.h>

int memory_init(boot_mm_list_node_t *mm_list_node, addr_t free_kvm_start, size_t free_kvm_size);

int kalloc(addr_t *addr, size_t size);

int kfree(addr_t addr);

#endif