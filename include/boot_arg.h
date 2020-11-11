#ifndef _BOOT_ARG_H_
#define _BOOT_ARG_H_
#include <basic.h>
#include <list.h>

struct boot_mm_t {
    addr_t addr;
    size_t size;
};

struct boot_arg_t {
    list_node(struct boot_mm_t) *mm_list;
};

#endif