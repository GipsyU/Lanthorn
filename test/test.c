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
extern int rbt_test(void);
extern int mm_test(void);

int test(void)
{

    extern char _binary_usr_test_elf_start[];

    debug("%p\n", _binary_usr_test_elf_start);

    info("begin test.\n");

    // rbt_test();

    // mm_test();

    // mmu_test();

    // proc_test();





    debug("test over.\n");

    return E_OK;
}