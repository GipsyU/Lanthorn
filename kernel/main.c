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
    
    list_rep (*(boot_arg.mm_list), p) {
        info("%p %p\n", p->data.addr, p->data.size);
    }

    err = memory_init(boot_arg.mm_list);
    
    if (err != E_OK) {
        info("init memory success %s\n",strerror(err));
    }
    // process_init();
    while(1);
}