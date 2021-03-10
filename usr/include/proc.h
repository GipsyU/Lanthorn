#ifndef _USR_PROC_H_
#define _USR_PROC_H_
#include <type.h>
struct thread_attr_t
{
    uint tid;
    size_t stk_sz;
    addr_t arga;
    size_t argsz;
};

#define PROC_NAME_MAX_LEN 32
struct proc_create_attr_t
{
    char name[PROC_NAME_MAX_LEN];
    char **argv;
    char **envp;
};

int fork(long *pid);

int proc_create(char *path, struct proc_create_attr_t *attr);

int thread_create(uint *tid, addr_t routine, struct thread_attr_t *attr);

int proc_exit(void);

int thread_exit(void);

int thread_tid(uint *tid);

int thread_block(uint tid);

int thread_wake(uint tid);

#endif