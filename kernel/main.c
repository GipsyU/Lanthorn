#include <error.h>
#include <basic.h>
#include <log.h>
#include <memory.h>
#include <list.h>
#include <boot_arg.h>
#include <string.h>

static void test(void)
{
    int *x = 0xFEE00000;
    debug("%d\n",*x)
    debug("test over\n");
}

void __attribute__((noreturn)) main(struct boot_arg_t boot_arg)
{
    int err = E_OK;

    info("Hello Lanthorn.\n");

    err = memory_init(boot_arg.mm_list, boot_arg.free_kvm_start, boot_arg.free_kvm_size);

    if (err != E_OK)
    {
        panic("init memory failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("kernal memory init success.\n");
    }

    test();

    // process_init();

}