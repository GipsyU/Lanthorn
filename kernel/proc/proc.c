#include <arch/mmu.h>
#include <arch/phyops.h>
#include <elf.h>
#include <error.h>
#include <idx_aclt.h>
#include <log.h>
#include <memory.h>
#include <proc.h>
#include <srv.h>
#include <string.h>
#include <syscall.h>
#include <thread.h>
#include <util.h>
#include "proc.h"

struct PROC_T
{
    struct idx_alct_t idx_alct;
    struct list_node_t proc_ls;
} PROC;

struct proc_t proc_0;

static int p0_init(void)
{
    char name[] = "kernal process";

    assert(sizeof(name) <= PROC_NAME_MAX_LEN);

    memcpy(proc_0.name, name, sizeof(name));

    proc_0.pid = 0;

    list_init(&proc_0.thread_ls);

    int err = idx_alct_new(&PROC.idx_alct, &proc_0, &proc_0.pid);

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

    err = idx_alct_new(&PROC.idx_alct, _proc, &_proc->pid);

    if (err != E_OK) goto err2;

    _proc->wait_t = NULL;

    list_push_back(&PROC.proc_ls, &_proc->proc_ln);

    if (proc != NULL) *proc = _proc;

    info("setup process %s success.\n", _proc->name);

    return err;

err2:
    assert(kmfree(_proc) == E_OK);

err1:
    return err;
}

static int inner_setup_uthread_from_mm_thread(addr_t start, size_t size, addr_t args)
{
    size = ROUND_UP(size, PAGE_SIZE);

    struct proc_t *proc = proc_now();

    addr_t exea = NULL;

    int err = umalloc(&proc->um, &exea, size);

    assert(err == E_OK && exea == 0);

    err = elf_read((void *)start, NULL);

    assert(err == E_OK);

    elf_load((void *)start, exea);

    memcpy(proc->um.layout.args_s, args, proc->um.layout.args_e - proc->um.layout.args_s);

    int *argc = (int *)proc->um.layout.args_s;

    char **argv = (char **)(proc->um.layout.args_s + sizeof(addr_t));

    char **envp = (char **)(proc->um.layout.args_s + sizeof(addr_t) * (1 + *argc));

    for (int i = 0; i < *argc; ++i)
    {
        char *s = NULL;

        err = umalloc(&proc->um, (void *)&s, strlen(argv[i]) + 1);

        if (err != E_OK) return err;

        memcpy(s, argv[i], strlen(argv[i]) + 1);

        argv[i] = s;
    }

    for (; *envp; ++envp)
    {
        char *s = NULL;

        err = umalloc(&proc->um, (void *)&s, strlen(*envp) + 1);

        if (err != E_OK) return err;

        memcpy(s, *envp, strlen(*envp) + 1);

        *envp = s;
    }

    struct thread_t *thread = NULL;

    err = thread_user_new(&thread, proc_now(), 0, DFT_STK_SZ, NULL, NULL);

    assert(err == E_OK);

    return 0;
}

static int proc_create_from_mm(char *name, addr_t exea, size_t exes, char **argv, char **envp, struct thread_t *wait_t,
                               int *res)
{
    if (strlen_s(name, PROC_NAME_MAX_LEN) == PROC_NAME_MAX_LEN) return E_TOOLONG;

    struct proc_t *proc;

    int err = proc_new(&proc, name);

    if (err != E_OK) return err;

    addr_t args = NULL;

    err = kmalloc(&args, proc->um.layout.args_e - proc->um.layout.args_s);

    if (err != E_OK) return err;

    int *_argc = (int *)args;

    *_argc = 0;

    char **_argv = (char **)(args + sizeof(_argc));

    _argv[0] = NULL;

    if (argv != NULL)
    {
        for (; *argv; (*_argc) += 1, argv++)
        {
            err = kmalloc((void *)&_argv[*_argc], strlen(*argv) + 1);

            if (err != E_OK) return err;

            strcpy(_argv[*_argc], *argv, strlen(*argv) + 1);
        }
    }

    char **_envp = (char **)(args + sizeof(addr_t) * (1 + *_argc));

    _envp[0] = NULL;

    if (envp != NULL)
    {
        for (int i = 0; *envp; ++i, envp++)
        {
            err = kmalloc((void *)&_envp[i], strlen(*envp) + 1);

            _envp[i + 1] = NULL;

            if (err != E_OK) return err;

            strcpy(_envp[i], *envp, strlen(*envp) + 1);
        }
    }

    if (wait_t != NULL)
    {
        proc->exit_state = res;

        proc->wait_t = wait_t;

        intr_irq_save();
    }

    err = thread_kern_new(proc, NULL, inner_setup_uthread_from_mm_thread, 3, exea, exes, args);

    if (wait_t != NULL)
    {
        schd_block(thread_now(), NULL);

        intr_irq_restore();
    }

    return err;
}

struct proc_t *proc_now(void)
{
    struct thread_t *thread = thread_now();

    return thread->proc;
}

int proc_id(void)
{
    return proc_now()->pid;
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

struct proc_create_attr_t
{
    char name[PROC_NAME_MAX_LEN];
    char **argv;
    char **envp;
    int iswait;
};

struct proc_create_res_t
{
    int err;
};

struct proc_create_attr_t proc_create_dft_attr = {.name = "anonymous proc", .argv = NULL, .envp = NULL, .iswait = 0};

static int proc_sys_create(char *path, struct proc_create_attr_t *attr, struct proc_create_res_t *res)
{
    struct srv_replyee_t replyee;

    int err = srv_kern_call("fssrv/read", &replyee, path, strlen(path) + 1);

    if (err != E_OK) return err;

    err = replyee.err;

    if (err != E_OK) return err;

    struct proc_create_attr_t *_attr = attr;

    if (_attr == NULL) _attr = &proc_create_dft_attr;

    if (_attr->iswait != 0)
    {
        int res_err = E_OK;

        err = proc_create_from_mm(_attr->name, replyee.cache, replyee.sz[0], _attr->argv, _attr->envp, thread_now(),
                                  &res_err);

        res->err = res_err;
    }
    else
    {
        err = proc_create_from_mm(_attr->name, replyee.cache, replyee.sz[0], _attr->argv, _attr->envp, NULL, NULL);
    }

    return err;
}

static int proc_sys_exit(int err)
{
    struct proc_t *proc = proc_now();

    info("process %p exit, err = %s.\n", proc, strerror(err));

    list_rep_s(&proc->thread_ls, p)
    {
        struct thread_t *t = container_of(p, struct thread_t, proc_ln);

        if (t != thread_now() && t->state != KILLED) schd_kill(t);
    }

    if (proc->wait_t != NULL)
    {
        *(proc->exit_state) = err;

        schd_run(proc->wait_t);
    }

    list_delete(&proc->proc_ln);

    schd_kill(thread_now());

    panic("bug.\n");
}

static int proc_sys_info(struct proc_info_t **info)
{

    int tot = 1;

    list_rep(&PROC.proc_ls, p)
    {
        ++tot;
    }

    int now = 0;

    struct proc_info_t *_info;

    int err = umalloc(&proc_now()->um, (void *)&_info, sizeof(struct proc_info_t) * tot);

    if (err != E_OK) return err;

    list_rep(&PROC.proc_ls, p)
    {
        struct proc_t *proc = container_of(p, struct proc_t, proc_ln);

        _info[now].pid = proc->pid;

        memcpy(_info[now].name, proc->name, sizeof(proc->name));

        ++now;
    }

    _info[now].pid = 0;

    *info = _info;

    return E_OK;
}

static int proc_sys_kill(int pid)
{
    if (proc_now()->pid == pid) return E_INVAL;

    struct proc_t *proc;

    int err = idx_alct_find(&PROC.idx_alct, pid, (void *)&proc);

    if (err != E_OK) return err;

    list_rep_s(&proc->thread_ls, p)
    {
        struct thread_t *t = container_of(p, struct thread_t, proc_ln);

        if (t->state != KILLED) schd_kill(t);
    }

    list_delete(&proc->proc_ln);

    return err;
}

extern char _binary_usr_init_elf_start[];
extern char _binary_usr_init_elf_size[];

extern char _binary_usr_devsrv_elf_start[];
extern char _binary_usr_devsrv_elf_size[];

extern char _binary_usr_fssrv_elf_start[];
extern char _binary_usr_fssrv_elf_size[];

extern char _binary_usr_test_elf_start[];
extern char _binary_usr_test_elf_size[];

int proc_init(void)
{
    idx_aclt_init(&PROC.idx_alct);

    list_init(&PROC.proc_ls);

    int err = p0_init();

    if (err == E_OK)
        info("init process 0 success.\n");

    else
        panic("init process 0 failed.\n");

    err = thread_init();

    if (err == E_OK)
        info("init thread success.\n");

    else
        panic("init thread failed.\n");

    // err = proc_create_from_mm("dev_service", _binary_usr_devsrv_elf_start, _binary_usr_devsrv_elf_size);

    // if (err == E_OK)
    //     info("init device service process success.\n");

    // else
    //     panic("init device service process failed.\n");

    // err = proc_create_from_mm("test", _binary_usr_test_elf_start, _binary_usr_test_elf_size);

    err = proc_create_from_mm("usr_init_proc", _binary_usr_init_elf_start, _binary_usr_init_elf_size, NULL, NULL, NULL,
                              NULL);

    if (err == E_OK)
        info("init usr init process success.\n");

    else
        panic("init usr init process failed.\n");

    err = proc_create_from_mm("file_service", _binary_usr_fssrv_elf_start, _binary_usr_fssrv_elf_size, NULL, NULL, NULL,
                              NULL);

    if (err == E_OK)
        info("init file service process success.\n");

    else
        panic("init file service process failed.\n");

    syscall_register(SYS_fork, proc_fork, 1);

    syscall_register(SYS_proc_create, proc_sys_create, 3);

    syscall_register(SYS_proc_exit, proc_sys_exit, 1);

    syscall_register(SYS_proc_info, proc_sys_info, 1);

    syscall_register(SYS_proc_kill, proc_sys_kill, 1);

    return err;
}

int proc_switch(struct proc_t *proc)
{
    mmu_pde_switch(proc->ptb.pde);

    return E_OK;
}
