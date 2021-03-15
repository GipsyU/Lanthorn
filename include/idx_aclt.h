#ifndef _IDX_ALCT_H_
#define _IDX_ACLT_H_
#include <arch/basic.h>
#include <mutex.h>
#include <rbt.h>

struct idx_alct_t
{
    struct rbt_t rbt;
    struct mutex_t mutex;
};

int idx_alct_new(struct idx_alct_t *alct, addr_t addr, int *id);

int idx_alct_find(struct idx_alct_t *alct, int id, addr_t *addr);

int idx_alct_delete(struct idx_alct_t *alct, int id);

void idx_aclt_init(struct idx_alct_t *alct);

#endif