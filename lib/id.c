#include <error.h>
#include <id.h>

int id_alloc(struct id_aclt_t *alct, addr_t ptr, id_t *res)
{
    if (ptr == 0) return E_INVAL;

    spin_lock(&alct->lock);

    for (id_t i = 0; i < alct->max_id; ++i)
    {
        if (alct->table[i] == 0)
        {
            alct->table[i] = ptr;

            spin_unlock(&alct->lock);

            *res = i;

            return E_OK;
        }
    }

    spin_unlock(&alct->lock);

    return E_NOSLOT;
}

int id_free(struct id_aclt_t *alct, id_t id)
{
    spin_lock(&alct->lock);

    if (id >= alct->max_id || alct->table[id] == 0)
    {
        spin_unlock(&alct->lock);

        return E_NOTFOUND;
    }

    alct->table[id] = 0;

    spin_unlock(&alct->lock);

    return E_OK;
}

int id_get_ptr(struct id_aclt_t *alct, id_t id, addr_t *res)
{
    spin_lock(&alct->lock);

    if (id >= alct->max_id || alct->table[id] == 0)
    {
        spin_unlock(&alct->lock);

        return E_NOTFOUND;
    }

    *res = alct->table[id];

    spin_unlock(&alct->lock);

    return E_OK;
}

int id_init(struct id_aclt_t *alct, addr_t addr, size_t size)
{
    spin_init(&alct->lock);

    alct->table = (void *)addr;

    alct->max_id = size / sizeof(id_t);

    for (id_t i = 0; i < alct->max_id; ++i) alct->table[i] = 0;

    return E_OK;
}