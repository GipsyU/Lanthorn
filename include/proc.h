#ifndef _PROC_H_
#define _PROC_H_

#include <basic.h>

struct proc_t
{
    uint flag;

    uint state;

    uint pid;

    // struct context 

};

int proc_init(void);

int proc_fork(void);


#endif