#include <arch/basic.h>
#include <arch/sysctrl.h>
#include <boot_arg.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <msg.h>
#include <proc.h>
#include <string.h>

extern void test(void);

extern int syscall_init(void);

void __attribute__((noreturn)) main(struct boot_arg_t boot_arg)
{
    int err = E_OK;

    info("Hello Lanthorn.\n");

    err = syscall_init();

    if (err != E_OK)
    {
        error("init syscall failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("init syscall success.\n");
    }

    err = memory_init(boot_arg.free_pmm_start, boot_arg.free_pmm_size, boot_arg.free_kvm_start, boot_arg.free_kvm_size,
                      boot_arg.pde);

    if (err != E_OK)
    {
        error("init memory failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("init memory success.\n");
    }

    err = proc_init();

    if (err != E_OK)
    {
        error("init proc failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("init proc success.\n");
    }

    err = msg_init();

    if (err != E_OK)
    {
        error("init msg failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("init msg success.\n");
    }

    test();

    info("Lanthorn kernel init finished.\n");

    sysctrl_shutdown();

    while (1)
        ;
}
