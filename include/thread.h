#ifndef _THREAD_H_

#define _THREAD_H_

#include <arch/cpu.h>

#include <proc.h>

#include <list.h>

#include <arch/basic.h>

#include <arch/task.h>

enum T_STATE {RUNNABEL, RUNNING, SLEEPING};

struct thread_t
{
    uint id;

    uint cpuid;

    enum T_STATE state;

    struct task_t task;

    struct proc_t *proc;

    struct list_node_t schd_ln;
};

int thread_new(struct thread_t **thread, addr_t run);

int thread_schd(void);

int thread_init(void);

int thread_dd(void);

#endif