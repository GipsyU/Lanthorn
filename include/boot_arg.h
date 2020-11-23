#ifndef _BOOT_ARG_H_
#define _BOOT_ARG_H_
#include <basic.h>
struct boot_arg_t {
    addr_t kern_start;
    addr_t kern_end;
    addr_t free_pmm_start;
    size_t free_pmm_size;
    addr_t free_kvm_start;
    size_t free_kvm_size;
    int ncpu;
};


#endif