#ifndef _DEV_H_
#define _DEV_H_
#include <arch/basic.h>
#include <list.h>

struct serial_t
{
    const char *name;
    struct serial_ops
    {
        int (*init)(addr_t);
        int (*read)(addr_t, addr_t, uint);
        int (*write)(addr_t, addr_t, uint);
        int (*ioctl)(addr_t, uint, uint);
    } ops;
    struct list_node_t dev_ln;
};

#endif