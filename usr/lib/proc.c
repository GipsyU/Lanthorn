#include <proc.h>
#include <syscall.h>

int fork(long *pid)
{
    return syscall(SYS_fork, pid);
}

int thread_new(uint *tid, addr_t routine, struct thread_attr_t *attr, addr_t arg)
{
    return syscall(SYS_thread_new, tid, routine, attr, arg);
}