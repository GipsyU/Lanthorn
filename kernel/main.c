#include <error.h>
#include <basic.h>
#include <log.h>
#include <memory.h>
#include <list.h>
#include <boot_arg.h>
#include <string.h>
#include <proc.h>
#include <arch/intr.h>
#include <arch/sysctrl.h>

static void test(void)
{
    debug("test over\n");
    return;
}

static int F(void)
{
    debug("F\n");

    return E_OK;
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

    // proc_init();

    info("Lanthorn kernel init finished.\n");


    asm volatile("sti");

    intr_register(128, F);

    asm volatile("int $128");

    intr_unregister(128);

    asm volatile("int $128");
    
    sysctrl_shutdown();
}
