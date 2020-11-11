#include <error.h>
#include <basic.h>
#include <log.h>
#include <drivers/console.h>
#include <memory.h>
#include <list.h>
#include <boot_arg.h>

void __attribute__((noreturn)) main(struct boot_arg_t boot_arg)
{
    int err = E_OK;
    
    err = memory_init();
    if (err != E_OK) {
        info("init memory success %s\n",strerror(err));
    }
    // process_init();
    while(1);
}