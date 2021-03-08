#include <proc.h>
#include <syscall.h>

int fork(long *pid)
{
    return syscall(SYS_fork, pid);
}

int thread_create(uint *tid, addr_t routine, struct thread_attr_t *attr, addr_t arg)
{
    return syscall(SYS_thread_create, tid, routine, attr, arg);
}

int thread_exit(void)
{
    return syscall(SYS_thread_exit);
}

int thread_tid(uint *tid)
{
    return syscall(SYS_thread_tid, tid);
}

int thread_block(uint tid)
{
    return syscall(SYS_thread_block, tid);
}

int thread_wake(uint tid)
{
    return syscall(SYS_thread_wake, tid);
}

int proc_create(char *path, struct proc_create_attr_t *attr)
{
    return syscall(SYS_proc_create, path, attr);
}