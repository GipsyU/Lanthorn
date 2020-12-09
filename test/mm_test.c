#include <memory.h>
#include <log.h>
#include <error.h>

int mm_test(void)
{
    int err = E_OK;

    addr_t addr;

    int test = 100;

    while(test --)
    {
        err = kmalloc(&addr, PAGE_SIZE);

        debug("%s %p\n", strerror(err), addr);
    }

    return err;
}