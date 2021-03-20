#include <arch/basic.h>
#include <arch/sysctrl.h>
#include <boot_arg.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <msg.h>
#include <proc.h>
#include <srv.h>
#include <string.h>
#include <syscall.h>
#include <arch/uart.h>

extern void test(void);

extern int syscall_init(void);

extern int sysctrl_init(void);

extern int tmp_init(void);

void main_ap(void)
{
    debug("OK\n");

    while(1);
}

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

    err = srv_init();

    if (err != E_OK)
    {
        error("init srv failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("init srv success.\n");
    }

    err = sysctrl_init();

    if (err != E_OK)
    {
        error("init sysctrl failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("init sysctrl success.\n");
    }

    err = tmp_init();

    addr_t stk;

    err = kmalloc(&stk, PAGE_SIZE);

    cpu_startap(1, main_ap, stk, PAGE_SIZE);

    while(1);

    test();

    info("Lanthorn kernel init finished.\n");

    sysctrl_shutdown();

    while (1)
        ;
}
