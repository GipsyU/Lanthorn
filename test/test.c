#include <arch/atomic.h>
#include <arch/cpu.h>
#include <error.h>
#include <log.h>
#include <proc.h>
#include <string.h>
#include <thread.h>

extern int thread_test(void);
extern int mmu_test(void);
extern int proc_test(void);
extern int rbt_test(void);
extern int mm_test(void);
extern int msg_test(void);

int test(void)
{
    info("begin test.\n");

    // rbt_test();

    // mm_test();

    // mmu_test();


    proc_test();

    // msg_test();

    debug("test over.\n");

    return E_OK;
}