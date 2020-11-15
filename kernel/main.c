#include <error.h>
#include <basic.h>
#include <log.h>
#include <drivers/console.h>
#include <memory.h>
#include <list.h>
#include <boot_arg.h>
#include <string.h>

static void test(void){
}

void __attribute__((noreturn)) main(struct boot_arg_t boot_arg)
{
    int err = E_OK;

    test();

    info("Hello Lanthorn.\n");
    err = memory_init(boot_arg.mm_list);

    if (err != E_OK) {
        info("init memory failed, err = %s.\n",strerror(err));
    } else {
        info("kernal memory init success.\n");
    }
    // process_init();
    while(1);
}