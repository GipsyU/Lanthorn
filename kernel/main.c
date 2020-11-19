#include <error.h>
#include <basic.h>
#include <log.h>
#include <memory.h>
#include <list.h>
#include <boot_arg.h>
#include <string.h>

static void test(void){
    addr_t addr;
    for(int i=1;i<=100;++i)
    {
        int err = kalloc(&addr, 0x1000);
        info("%p %s\n", addr, strerror(err));
    }
}

void __attribute__((noreturn)) main(struct boot_arg_t boot_arg)
{
    int err = E_OK;

    info("Hello Lanthorn.\n");

    int x;
    memset(x, 0, sizeof(x));

    err = memory_init(boot_arg.mm_list, boot_arg.free_kvm_start, boot_arg.free_kvm_size);

    if (err != E_OK) {
        panic("init memory failed, err = %s.\n",strerror(err));
    } else {
        info("kernal memory init success.\n");
    }

    test();

    // process_init();

    while(1);
}