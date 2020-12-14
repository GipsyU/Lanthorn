#include <syscall.h>
#include <error.h>
#include <arch/intr.h>
#include <log.h>

static int (*syscalls[NR_SYSCALL])(addr_t);

static int syscall_hdl(uint *args)
{
    assert(args[0] < NR_SYSCALL);

    info("syscall: sysno: %d.\n", args[0]);

    if (syscalls[args[0]] != NULL)
    {
        intr_get_arg();

        return syscalls[args[0]](args);
    }
}

int syscall_register(uint id, int (*func)(uint))
{
    if (id >= NR_SYSCALL) return E_INVAL;

    syscalls[id] = func;

    return E_OK;
}

int syscall_unregister(uint id)
{
    if (id >= NR_SYSCALL) return E_INVAL;

    syscalls[id] = NULL;

    return E_OK;
}

int syscall_init(void)
{
    intr_register(INTR_SYSCALL, syscall_hdl);

    return E_OK;
}