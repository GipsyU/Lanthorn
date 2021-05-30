#include <error.h>
#include <log.h>
#include <mm.h>
#include <proc.h>
#include <stdio.h>
#include <syscall.h>

int fork(long *pid)
{
    return syscall(SYS_fork, pid);
}

int thread_create(uint *tid, addr_t routine, uint nparam, ...)
{
    struct thread_attr_t attr;

    attr.arga = ((addr_t)&nparam) + sizeof(nparam);

    addr_t *f = (addr_t *)attr.arga;

    attr.argsz = nparam * sizeof(addr_t);

    return syscall(SYS_thread_create, tid, routine, &attr);
}

int thread_exit(void)
{
    return syscall(SYS_thread_exit);
}

int proc_info(struct proc_info_t **info)
{
    return syscall(SYS_proc_info, info);
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

int proc_create(char *path, struct proc_create_attr_t *attr, struct proc_create_res_t *res)
{
    return syscall(SYS_proc_create, path, attr, res);
}

int proc_exit(int err)
{
    return syscall(SYS_proc_exit, err);
}