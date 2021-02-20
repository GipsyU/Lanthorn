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

struct proc_t proc_0;

static struct proc_t *proc_1;

static int p0_init(void)
{
    int err = E_OK;

    list_init(&proc_0.thread_ls);

    return err;
}

/**
 * FIXME: CLEAN UP
 */

static int p1_init(void)
{
    int err = proc_new(&proc_1);

    if (err != E_OK) return err;

    struct thread_t *thread = NULL;

    addr_t exea = NULL;

    err = umalloc(&proc_1->um, &exea, PAGE_SIZE);

    assert(exea == 0);

    if (err != E_OK) return err;

    err = thread_user_new(&thread, proc_1, 0, PAGE_SIZE * 1024, NULL);

    if (err != E_OK) return err;

    struct page_t *page;

    err = page_alloc(&page);

    if (err != E_OK) return err;

    err = ptb_map(&proc_1->ptb, exea, page->addr, 1, 1);

    struct vpage_t *vpage = NULL;

    err = vm_search_addr(&proc_1->um.vp_alct, 0, &vpage);

    assert(err == E_OK);

    vpage->map_page = page;

    vpage->type = UM_NORMAL;

    if (err != E_OK) return err;

    addr_t t;

    err = mmu_v2p(proc_1->ptb.pde, 0, &t);

    if (err != E_OK) return err;

    addr_t tmp;

    err = kmalloc(&tmp, PAGE_SIZE);

    assert(tmp % PAGE_SIZE == 0);

    if (err != E_OK) return err;

    extern char _binary_usr_init_elf_start[];

    extern char _binary_usr_init_elf_end[];

    err = elf_read((void *)_binary_usr_init_elf_start, NULL);

    if (err != E_OK) return err;

    elf_load((void *)_binary_usr_init_elf_start, tmp);

    phyops_memcpy_v2p(page->addr, tmp, PAGE_SIZE);

    return err;
}

struct proc_t *proc_now(void)
{
    struct thread_t *thread = thread_now();

    return thread->proc;
}

/**
 * FIXME: CLEAN UP RSC
 */
int proc_new(struct proc_t **proc)
{
    int err = kmalloc((addr_t *)proc, sizeof(struct proc_t));

    if (err != E_OK) return err;

    struct page_t *pde;

    err = page_alloc(&pde);

    if (err != E_OK) return err;

    err = ptb_init(&(*proc)->ptb, pde->addr);

    if (err != E_OK) return err;

    mmu_sync_kern_space(proc_0.ptb.pde, (*proc)->ptb.pde, KERN_BASE, 0 - KERN_BASE);

    list_init(&(*proc)->thread_ls);

    err = um_init(&(*proc)->um);

    if (err != E_OK) return err;

    return err;
}

// int proc_free(struct proc_t *proc)
// {
//     int err ;

//     err = kmfree(proc);

// }

static int do_fork(struct proc_t *po)
{
    struct proc_t *pn;

    int err = proc_new(&pn);

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

    thread_kern_new(&thread, do_fork, 1, proc_now());

    schd_sleep(thread_now(), 555);

    *res = (addr_t)proc_now();

    return E_OK;
}

int proc_init(void)
{
    int err = schd_init();

    if (err != E_OK) return err;

    err = p0_init();

    if (err != E_OK) return err;

    err = thread_init();

    if (err != E_OK) return err;

    err = p1_init();

    if (err != E_OK) return err;

    syscall_register(SYS_fork, proc_fork, 1);

    return err;
}

int proc_switch(struct proc_t *proc)
{
    mmu_pde_switch(proc->ptb.pde);

    return E_OK;
}
