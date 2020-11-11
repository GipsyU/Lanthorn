#ifndef _BOOT_ARG_H_
#define _BOOT_ARG_H_
#include <basic.h>

struct boot_mm_t {
    size_t length;
    struct boot_mm_t *next;
};

struct boot_arg_t {
    struct boot_mm_t *mm_free;
};

#endif