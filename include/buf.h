#ifndef _BUF_H_
#define _BUF_H_
#include <arch/basic.h>
#include <mutex.h>
#include <thread.h>

struct buf_t
{
    char *cache;
    uint size;
    uint freesz;
    struct mutex_t readlock;
    struct mutex_t writelock;
    uint b, e;
    struct thread_t *wblkt;
    struct thread_t *rblkt;
};

int buf_read(struct buf_t *buf, char *c);

int buf_write(struct buf_t *buf, char c);

int buf_init(struct buf_t *buf, size_t size);

#endif