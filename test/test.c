#include <error.h>
#include <log.h>
#include <proc.h>
#include <arch/cpu.h>
#include <log.h>
#include <thread.h>
#include <arch/atomic.h>

extern int thread_test(void);
extern int mmu_test(void);
extern int proc_test(void);

int test(void)
{

    info("begin test.\n");

    // thread_test();

    // mmu_test();

    // proc_test();





    debug("test over.\n");

    return E_OK;
}