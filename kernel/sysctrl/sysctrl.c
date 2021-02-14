#include <arch/basic.h>
#include <arch/sysctrl.h>
#include <error.h>
#include <syscall.h>

int sysctrl_init(void)
{
    int err = syscall_register(SYS_poweroff, sysctrl_shutdown, 0);

    return err;
}