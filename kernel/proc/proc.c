#include <arch/mmu.h>
#include <arch/phyops.h>
#include <elf.h>
#include <error.h>
#include <log.h>
#include <memory.h>
#include <proc.h>
#include <string.h>
#include <syscall.h>
#include <thread.h>
#include <util.h>

#include "proc.h"

struct proc_t proc_0;

static int p0_init(void)
{
    char name[] = "kernal process";

    assert(sizeof(name) <= PROC_NAME_MAX_LEN);

    memcpy(proc_0.name, name, sizeof(name));

    proc_0.pid = 0;

    list_init(&proc_0.thread_ls);

    int err = proc_rbt_insert(&proc_0, 0);

    return err;
}

static int proc_new(struct proc_t **proc, char *name)
{
    struct proc_t *_proc;

    int err = kmalloc((addr_t *)&_proc, sizeof(struct proc_t));

    if (err != E_OK) goto err1;

    spin_rwlock_init(&_proc->lock);

    memset(_proc->name, 0, sizeof(_proc->name));

    strcpy(_proc->name, name, strlen_s(name, PROC_NAME_MAX_LEN));

    err = ptb_setup(&_proc->ptb);

    if (err != E_OK) goto err2;

    list_init(&_proc->thread_ls);

    err = um_init(&_proc->um);

    if (err != E_OK) goto err2;

    proc_rbt_insert(_proc, 1);

    if (proc != NULL) *proc = _proc;

    info("setup process %s success.\n", _proc->name);

    return err;

err2:
    assert(kmfree(_proc) == E_OK);

err1:
    return err;
}

static int inner_setup_uthread_from_mm_thread(addr_t start, size_t size)
{
    size = ROUND_UP(size, PAGE_SIZE);

    addr_t exea = NULL;

    int err = umalloc(&proc_now()->um, &exea, size);

    assert(err == E_OK && exea == 0);

    err = elf_read((void *)start, NULL);

    assert(err == E_OK);

    elf_load((void *)start, exea);

    struct thread_t *thread = NULL;

    err = thread_user_new(&thread, proc_now(), 0, DFT_STK_SZ, NULL);

    assert(err == E_OK);

    return 0;
}

static int proc_create_from_mm(char *name, addr_t exea, size_t exes)
{
    if (strlen_s(name, PROC_NAME_MAX_LEN) == PROC_NAME_MAX_LEN) return E_TOOLONG;

    struct proc_t *proc;

    int err = proc_new(&proc, name);

    if (err != E_OK) return err;

    err = thread_kern_new(proc, NULL, inner_setup_uthread_from_mm_thread, 2, exea, exes);

    return err;
}

struct proc_t *proc_now(void)
{
    struct thread_t *thread = thread_now();

    return thread->proc;
}

// int proc_free(struct proc_t *proc)
// {
//     int err ;

//     err = kmfree(proc);
// }

static int do_fork(struct proc_t *po)
{
    struct proc_t *pn;

    int err = proc_new(&pn, "bug");

    assert(err == E_OK);

    err = ptb_fork(&po->ptb, &pn->ptb);

    assert(err == E_OK);

    list_rep(&po->thread_ls, p)
    {
        struct thread_t *threado = container_of(p, struct thread_t, proc_ln);

        struct thread_t *threadn = NULL;

        err = thread_fork(threado, pn, &threadn);

        assert(err == E_OK);
    }

    um_fork(&po->um, &pn->um, &pn->ptb);

    list_rep(&pn->thread_ls, p)
    {
        struct thread_t *thread = container_of(p, struct thread_t, proc_ln);

        schd_run(thread);
    }

    schd_wake(555);
}

int proc_fork(addr_t *res)
{
    struct thread_t *thread;

    thread_kern_new(&proc_0, &thread, do_fork, 1, proc_now());

    schd_sleep(thread_now(), 555);

    *res = (addr_t)proc_now();

    return E_OK;
}

extern char _binary_usr_init_elf_start[];
extern char _binary_usr_init_elf_size[];

extern char _binary_usr_devsrv_elf_start[];
extern char _binary_usr_devsrv_elf_size[];

extern char _binary_usr_filesrv_elf_start[];
extern char _binary_usr_filesrv_elf_size[];

extern char _binary_usr_test_elf_start[];
extern char _binary_usr_test_elf_size[];
int proc_init(void)
{
    int err = proc_rbt_init();

    if (err == E_OK)
        info("init process rbt success.\n");

    else
        panic("init process rbt failed.\n");

    err = p0_init();

    if (err == E_OK)
        info("init process 0 success.\n");

    else
        panic("init process 0 failed.\n");

    err = thread_init();

    if (err == E_OK)
        info("init thread success.\n");

    else
        panic("init thread failed.\n");

    // err = proc_create_from_mm("usr_init_proc", _binary_usr_init_elf_start, _binary_usr_init_elf_size);

    if (err == E_OK)
        info("init usr init process success.\n");

    else
        panic("init usr init process failed.\n");

    // err = proc_create_from_mm("dev_service", _binary_usr_devsrv_elf_start, _binary_usr_devsrv_elf_size);

    // if (err == E_OK)
    //     info("init device service process success.\n");

    // else
    //     panic("init device service process failed.\n");

    // err = proc_create_from_mm("file_service", _binary_usr_filesrv_elf_start, _binary_usr_filesrv_elf_size);

    // if (err == E_OK)
    //     info("init file service process success.\n");

    // else
    //     panic("init file service process failed.\n");

    err = proc_create_from_mm("test", _binary_usr_test_elf_start, _binary_usr_test_elf_size);

    assert(err == E_OK);

    syscall_register(SYS_fork, proc_fork, 1);

    return err;
}

int proc_switch(struct proc_t *proc)
{
    mmu_pde_switch(proc->ptb.pde);

    return E_OK;
}
