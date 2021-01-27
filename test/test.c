#include <arch/atomic.h>
#include <arch/cpu.h>
#include <error.h>
#include <log.h>
#include <proc.h>
#include <thread.h>
#include <string.h>


extern int thread_test(void);
extern int mmu_test(void);
extern int proc_test(void);
extern int rbt_test(void);
extern int mm_test(void);

char A[10] = "asdfasdf";
char B[10] = "asdf";

int test(void)
{
    debug("%d %d\n", strlen(A), strlen(B));

    info("begin test.\n");

    // rbt_test();

    // mm_test();

    // mmu_test();

    proc_test();

    debug("test over.\n");

    return E_OK;
}