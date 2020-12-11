#include <error.h>
#include <log.h>
#include <proc.h>
#include <arch/cpu.h>
#include <log.h>
#include <thread.h>
#include <spinlock.h>

static struct spinlock_t spinlock;

void P1(void)
{
    for(uint i =0;;++i){
        if(i%100000000==0){
            debug("P1\n");
        }
        if(i%1000000000==0){
            spin_lock(&spinlock);
            debug("P1 get lock\n");
            int t = 1000000000;
            while(t--){

            }
            debug("P1 free lock\n");
            spin_unlock(&spinlock);
        }
    }
}
void P2(void)
{
    for(uint i =0;;++i){
        if(i%100000000==0){
            debug("P2\n");
        }
        if(i%1000000000==0){
            spin_lock(&spinlock);
            debug("P2 get lock\n");
            int t = 1000000000;
            while(t--){

            }
            debug("P2 free lock\n");
            spin_unlock(&spinlock);
        }
    }
}

void P3(void)
{
    for(uint i =0;;++i){
        if(i%100000000==0){
            debug("P3\n");
        }
    }
}
void P4(void)
{
    for(uint i =0;;++i){
        if(i%100000000==0){
            debug("P4\n");
        }
    }
}
void P5(void)
{
    for(uint i =0;;++i){
        if(i%100000000==0){
            debug("P5\n");
        }
    }
}

int thread_test(void)
{

    info("begin thread test.\n");

    thread_init();

    spin_init(&spinlock);

    struct thread_t *t1, *t2, *t3, *t4, *t5;

    thread_kern_new(&t1, P1);

    thread_kern_new(&t2, P2);

    // thread_kern_new(&t3, P3);
    
    // thread_kern_new(&t4, P4);
    
    // thread_kern_new(&t5, P5);

    thread_schd();


    debug("thread test over.\n");

    return E_OK;
}