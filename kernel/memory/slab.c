#include <error.h>
#include <memory.h>
#include <spinlock.h>
#include <util.h>

static inline uint *get_cnt(addr_t addr)
{
    return (uint *)ROUND_DOWN(addr, PAGE_SIZE);
}

int slab_alloc(struct slab_alct_t *alct, addr_t *addr, size_t size)
{
    int err = E_OK;

    if (alct->freep == NULL || alct->freep + size >= ROUND_UP(alct->freep, PAGE_SIZE))
    {
        addr_t _addr = NULL;

        err = alct->alloc(&_addr, PAGE_SIZE);

        if (err != E_OK) return err;

        alct->freep = _addr + sizeof(uint);

        uint *cnt = (void *)_addr;

        *cnt = 0;
    }

    *addr = alct->freep;

    uint *cnt = get_cnt(alct->freep);

    (*cnt)++;

    alct->freep += size;

    return err;
}

int slab_free(struct slab_alct_t *alct, addr_t addr)
{
    int err = E_OK;

    uint *cnt = get_cnt(addr);

    (*cnt)--;

    if (*cnt == 0)
    {
        if (ROUND_DOWN(alct->freep, PAGE_SIZE) == (addr_t)cnt) alct->freep = NULL;
        
        err = (alct->free)((addr_t)cnt);
    }

    return err;
}

int slab_init(struct slab_alct_t *alct, int (*alloc)(addr_t *, size_t), int (*free)(addr_t))
{
    spin_init(&alct->lock);

    alct->alloc = alloc;

    alct->free = free;

    alct->freep = NULL;

    return E_OK;
}