#include <proc.h>
#include <log.h>
#include <error.h>
#include <thread.h>
#include <memory.h>
#include <arch/mmu.h>
void T11(void)
{
    addr_t addr;


    int err = kmalloc(&addr, PAGE_SIZE * 1024 * 5);

    debug("%s %p\n", strerror(err), addr);

    debug("%p\n", mmu_get_pde())

    int *x = 0xc0800000;

    *x = 1;

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

    int *x = 0xc0800000;

    *x = 1;
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

    // struct proc_t *p1, *p2;

    // err = proc_new(&p1);

    // err = proc_new(&p2);
    
    // struct thread_t *t11, *t12, *t21, *t22;

    // err = thread_kern_new(&t11, p1, T11);
    
    // err = thread_kern_new(&t12, p1, T12);
    
    // err = thread_kern_new(&t21, p2, T21);
    
    // err = thread_kern_new(&t22, p2, T22);

    thread_schd();
}