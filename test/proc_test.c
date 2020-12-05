#include <proc.h>
#include <log.h>
#include <error.h>
#include <thread.h>

extern int thread_user_new(struct thread_t **thread, addr_t run);
int proc_test(void)
{
    int err = E_OK;

    thread_init();

    struct thread_t *t1;

    thread_user_new(&t1, NULL);

    struct proc_t *proc;

    err = proc_user_init(&proc);

    t1->proc = proc;

    thread_schd();

    return err;


}