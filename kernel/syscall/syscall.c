#include <arch/intr.h>
#include <error.h>
#include <log.h>
#include <syscall.h>
#include <spinlock.h>

static addr_t syscalls[NR_SYSCALL];

static uint syscall_nparam[NR_SYSCALL];

static uint syscall_nolog[NR_SYSCALL];

static int syscall_hdl(uint *args)
{
    assert(args[0] < NR_SYSCALL);

    if (syscall_nolog[args[0]] == 0) info("syscall: sysno: %d.\n", args[0]);

    if (syscalls[args[0]] != NULL)
    {
        if (syscall_nparam[args[0]] == 0)
        {
            int (*handler)(void) = (void *)syscalls[args[0]];

            return handler();
        }
        if (syscall_nparam[args[0]] == 1)
        {
            int (*handler)(uint) = (void *)syscalls[args[0]];

            return handler(args[1]);
        }
        if (syscall_nparam[args[0]] == 2)
        {
            int (*handler)(uint, uint) = (void *)syscalls[args[0]];

            return handler(args[1], args[2]);
        }
        if (syscall_nparam[args[0]] == 3)
        {
            int (*handler)(uint, uint, uint) = (void *)syscalls[args[0]];

            return handler(args[1], args[2], args[3]);
        }
        if (syscall_nparam[args[0]] == 4)
        {
            int (*handler)(uint, uint, uint, uint) = (void *)syscalls[args[0]];

            return handler(args[1], args[2], args[3], args[4]);
        }
        if (syscall_nparam[args[0]] == 5)
        {
            int (*handler)(uint, uint, uint, uint, uint) = (void *)syscalls[args[0]];

            return handler(args[1], args[2], args[3], args[4], args[5]);
        }

        error("nparam overflow.\n");
    }
}

int syscall_register(uint id, addr_t func, uint nparam)
{
    if (id >= NR_SYSCALL) return E_INVAL;

    syscalls[id] = func;

    syscall_nparam[id] = nparam;

    return E_OK;
}

int syscall_unregister(uint id)
{
    if (id >= NR_SYSCALL) return E_INVAL;

    syscalls[id] = NULL;

    syscall_nparam[id] = 0;

    return E_OK;
}

int syscall_init(void)
{
    intr_register(INTR_SYSCALL, (void *)syscall_hdl);

    syscall_nolog[SYS_read] = 1;

    syscall_nolog[SYS_write] = 1;

    return E_OK;
}