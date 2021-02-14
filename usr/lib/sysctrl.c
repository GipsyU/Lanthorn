#include <syscall.h>
#include <sysctrl.h>

void sysctrl_poweroff(void)
{
    syscall(SYS_poweroff);
}