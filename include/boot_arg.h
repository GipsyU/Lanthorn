#ifndef _BOOT_ARG_H_
#define _BOOT_ARG_H_
#include <basic.h>
#include <list.h>

struct boot_mm_t {
    addr_t addr;
    size_t size;
};

typedef list_node(struct boot_mm_t) boot_mm_list_node_t;

struct boot_arg_t {
    boot_mm_list_node_t *mm_list;
    addr_t kern_start;
    addr_t kern_end;
    addr_t free_kvm_start;
    size_t free_kvm_size;
    int ncpu;

};


#endif