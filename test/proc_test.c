#include <proc.h>
#include <log.h>
#include <error.h>
#include <thread.h>
#include <memory.h>
#include <arch/mmu.h>
void T11(void)
{
    for(uint i =0;;++i){
        if(i%100000000==0){
            debug("t11\n");
        }
    }

    while(1);
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

    // struct thread_t *t11, *t12, *t21, *t22;

    // err = thread_kern_new(&t11, T11);
    
    // err = thread_kern_new(&t12, T12);
    
    // err = thread_kern_new(&t21, T21);
    
    // err = thread_kern_new(&t22, T22);

    schd_schdule();
}