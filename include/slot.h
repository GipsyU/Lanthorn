#ifndef _SOLT_H_
#define _SOLT_H_
#include <arch/basic.h>
#include <list.h>

struct slot_alct_t
{
    size_t slot_size;

    struct list_node_t free_slot_head;
};

void slot_init(struct slot_alct_t *alct, size_t slot_size);

int slot_alloc(struct slot_alct_t *alct, addr_t *addr);

int slot_free(struct slot_alct_t *alct, addr_t addr);

void slot_insert(struct slot_alct_t *alct, addr_t addr, size_t size);

#endif