#include <proc.h>
#include <log.h>
#include <error.h>
#include <thread.h>

void T11(void)
{
    for(uint i =0;;++i){
        if(i%100000000==0){
            debug("t11\n");
        }
    }
}
void T12(void)
{
    for(uint i =0;;++i){
        if(i%100000000==0){
            debug("t12\n");
        }
    }
}

void T21(void)
{
    for(uint i =0;;++i){
        if(i%100000000==0){
            debug("t21\n");
        }
    }
}

void T22(void)
{
    for(uint i =0;;++i){
        if(i%100000000==0){
            debug("t22\n");
        }
    }
}

int proc_test(void)
{
    int err = E_OK;

    struct proc_t *p1, *p2;

    err = proc_new(&p1);

    err = proc_new(&p2);

    struct thread_t *t11, *t12, *t21, *t22;

    err = thread_kern_new(&t11, p1, T11);

    debug("%s\n", strerror(err));

    err = thread_kern_new(&t12, p1, T12);
    
    debug("%s\n", strerror(err));

    err = thread_kern_new(&t21, p2, T21);
    
    debug("%s\n", strerror(err));
    
    err = thread_kern_new(&t22, p2, T22);

    debug("%s\n", strerror(err));
    
    thread_schd();
}