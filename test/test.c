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

    struct atomic_t atomic = ATOMIC_INIT(1);

    debug("%d\n",atomic_read(&atomic));
    atomic_cmpxchg(&atomic, 2, 2);
    debug("%d\n",atomic_read(&atomic));
    atomic_cmpxchg(&atomic, 1, 2);
    debug("%d\n",atomic_read(&atomic));
    atomic_xchg(&atomic, 3);
    debug("%d\n",atomic_read(&atomic));



    debug("test over.\n");

    return E_OK;
}