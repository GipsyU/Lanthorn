#ifndef __SOLT_H_

#define __SLOT_H_

#include <basic.h>

#include <list.h>

struct slot_alct_t
{
    size_t slot_size;

    struct list_node_t free_slot_head;

    int (*alloc)(addr_t *, size_t);
};

int slot_init(struct slot_alct_t *alct, size_t slot_size);

int slot_new(struct slot_alct_t *alct, addr_t *addr);

int slot_free(struct slot_alct_t *alct, addr_t addr);

int slot_insert(struct slot_alct_t *alct, addr_t addr, size_t size);

#endif