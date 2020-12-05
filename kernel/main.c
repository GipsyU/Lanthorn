#include <error.h>
#include <arch/basic.h>
#include <log.h>
#include <memory.h>
#include <boot_arg.h>
#include <string.h>
#include <proc.h>
#include <arch/sysctrl.h>

extern void test(void);

void __attribute__((noreturn)) main(struct boot_arg_t boot_arg)
{
    int err = E_OK;

    info("Hello Lanthorn.\n");

    err = memory_init(boot_arg.free_pmm_start, boot_arg.free_pmm_size, boot_arg.free_kvm_start, boot_arg.free_kvm_size);

    if (err != E_OK)
    {
        error("init memory failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("kernal memory init success.\n");
    }
    
    err = proc_init(boot_arg.pde);
    
    if (err != E_OK)
    {
        error("init proc failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("init proc success.\n");
    }

    test();

    info("Lanthorn kernel init finished.\n");

    sysctrl_shutdown();
}
