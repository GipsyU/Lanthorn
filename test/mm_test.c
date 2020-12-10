#include <memory.h>
#include <log.h>
#include <error.h>

static void test1(void)
{
    int err = E_OK;

    addr_t addr;

    int test = 100;

    while(test --)
    {
        err = kmalloc(&addr, PAGE_SIZE);

        debug("%s %p\n", strerror(err), addr);

        int *x = addr;

        *x = 1;
        
        err = kmfree(addr);

        debug("%s\n", strerror(err));

        *x = 1;
    }

}

static void test2(void)
{
    int err = E_OK;

    addr_t addr;

    int test = 100;

    while(test --)
    {
        err = kmalloc(&addr, 64);

        debug("%s %p\n", strerror(err), addr);

        int *x = addr;

        *x = 1;
        
        err = kmfree(addr);

        debug("%s\n", strerror(err));

        *x = 1;
    }

}

int mm_test(void)
{
    int err = E_OK;

    // test1();
    
    test2();
    
    return err;
}