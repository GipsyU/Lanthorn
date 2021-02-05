#ifndef _IDALCT_H_
#define _IDALCT_H_
#include <arch/basic.h>
#include <spinlock.h>

typedef unsigned long id_t;

struct id_aclt_t
{
    addr_t *table;
    id_t max_id;
    struct spinlock_t lock;
};

int id_alloc(struct id_aclt_t *alct, addr_t ptr, id_t *res);

int id_free(struct id_aclt_t *alct, id_t id);

int id_get_ptr(struct id_aclt_t *alct, id_t id, addr_t *res);

int id_init(struct id_aclt_t *alct, addr_t addr, size_t size);

#endif