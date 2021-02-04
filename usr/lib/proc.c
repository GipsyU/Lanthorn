#include <proc.h>
#include <syscall.h>

int fork(long *pid)
{
    return syscall(SYS_fork, pid);
}