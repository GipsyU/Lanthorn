#ifndef _FILE_H_
#define _FILE_H_
#include <arch/basic.h>

struct file_t
{
    enum {F_INODE, F_DEV} type;
    addr_t info;
    struct {
        int (*read)(addr_t, addr_t, size_t);
        int (*write)(addr_t, addr_t, size_t);
    }ops;
};

int file_read(uint fd, addr_t addr, size_t size);

int file_write(uint fd, addr_t addr, size_t size);

#endif