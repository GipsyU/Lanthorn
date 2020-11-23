#include <slot.h>

#include <error.h>

int slot_init(struct slot_alct_t *alct, size_t slot_size)
{   
    if (slot_size < sizeof(struct list_node_t))
    {
        slot_size = sizeof(struct list_node_t);
    }
    
    alct->slot_size = slot_size;

    list_init(&(alct->free_slot_head));

    return E_OK;
}

int slot_new(struct slot_alct_t *alct, addr_t *addr)
{
    if (list_isempty(&(alct->free_slot_head)))
    {
        return E_NOSLOT;
    }

    *addr = (addr_t)list_pop_front(&(alct->free_slot_head));

    return E_OK;
}

int slot_free(struct slot_alct_t *alct, addr_t addr)
{   
    list_push_back(&(alct->free_slot_head), (struct list_node_t *)addr);

    return E_OK;
}

int slot_insert(struct slot_alct_t *alct, addr_t addr, size_t size)
{
    for (addr_t _addr= addr; _addr + alct->slot_size <= addr + size; _addr += alct->slot_size)
    {
        slot_free(alct, _addr);
    }

    return E_OK;
}