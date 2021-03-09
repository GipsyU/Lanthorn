#include <buf.h>
#include <error.h>
#include <memory.h>

int buf_read(struct buf_t *buf, char *c)
{
    mutex_lock(&buf->readlock);

    if (buf->freesz == buf->size)
    {
        buf->rblkt = thread_now();

        schd_block(thread_now());
    }

    assert(buf->freesz < buf->size);

    *c = buf->cache[buf->b];

    buf->b = (buf->b + 1) % buf->size;

    buf->freesz++;

    if (buf->wblkt != NULL)
    {
        struct thread_t *t = buf->wblkt;

        buf->wblkt = NULL;

        schd_run(t);
    }

    mutex_unlock(&buf->readlock);

    return E_OK;
}

int buf_write(struct buf_t *buf, char c)
{
    mutex_lock(&buf->writelock);

    if (buf->freesz == 0)
    {
        buf->wblkt = thread_now();

        schd_block(thread_now());
    }

    assert(buf->freesz > 0);

    buf->cache[buf->e] = c;

    buf->e = (buf->e + 1) % buf->size;

    buf->freesz--;

    if (buf->rblkt != NULL)
    {
        struct thread_t *t = buf->rblkt;

        buf->rblkt = NULL;

        schd_run(t);
    }

    mutex_unlock(&buf->writelock);

    return E_OK;
}

int buf_init(struct buf_t *buf, size_t size)
{
    int err = kmalloc((void *)&buf->cache, size);

    if (err != E_OK) return err;

    buf->b = buf->e = 0;

    buf->size = size;

    buf->freesz = size;

    mutex_init(&buf->readlock);

    mutex_init(&buf->writelock);

    buf->rblkt = buf->wblkt = NULL;

    return err;
}