#include <error.h>
#include <log.h>
#include <proc.h>
#include <rbt.h>
#include <util.h>

#include "proc.h"

static void update(struct rbt_node_t *node)
{
    struct proc_t *proc = container_of(node, struct proc_t, pid_rbt);

    uint sum = 1;

    if (node->l)
    {
        sum += (container_of(node->l, struct proc_t, pid_rbt))->rbt_node_sum;
    }
    if (node->r)
    {
        sum += (container_of(node->r, struct proc_t, pid_rbt))->rbt_node_sum;
    }

    proc->rbt_node_sum = sum;
}

static int _rbt_insert(struct rbt_t *rbt, struct proc_t *proc)
{
    struct rbt_node_t **p = &rbt->root;

    struct rbt_node_t *parent = NULL;

    struct proc_t *_proc;

    while (*p)
    {
        parent = *p;

        _proc = container_of(parent, struct proc_t, pid_rbt);

        if (proc->pid < _proc->pid)
            p = &(*p)->l;

        else if (proc->pid > _proc->pid)
            p = &(*p)->r;

        else
            return E_EXIST;
    }

    rb_link_node(&proc->pid_rbt, parent, p);

    return E_OK;
}

static int rbt_insert(struct rbt_t *rbt, struct proc_t *proc)
{
    int err = _rbt_insert(rbt, proc);

    if (err != E_OK) return err;

    rbt_insert_color(rbt, &proc->pid_rbt);

    rbt_insert_update(&proc->pid_rbt, update);

    return err;
}

static int rbt_find(struct rbt_t *rbt, uint pid, struct proc_t **res)
{
    struct rbt_node_t *n = rbt->root;

    while (n)
    {
        struct proc_t *proc = container_of(n, struct proc_t, pid_rbt);

        if (pid < proc->pid)
            n = n->l;

        else if (pid > proc->pid)
            n = n->r;

        else
        {
            *res = proc;

            return E_OK;
        }
    }

    return E_NOTFOUND;
}

static uint rbt_find_free_pid(struct rbt_t *rbt)
{
    struct rbt_node_t *n = rbt->root;

    if (n == NULL) return 0;

    uint base = 0;

    while (n)
    {
        struct proc_t *proc = container_of(n, struct proc_t, pid_rbt);

        if (n->l)
        {
            assert(container_of(n->l, struct proc_t, pid_rbt)->rbt_node_sum + base <= proc->pid);

            if (container_of(n->l, struct proc_t, pid_rbt)->rbt_node_sum + base < proc->pid)
            {
                n = n->l;

                continue;
            }
        }
        else
        {
            if (base < proc->pid) return base;
        }

        if (n->r)
        {
            base = proc->pid + 1;

            n = n->r;
        }
        else
        {
            return proc->pid + 1;
        }
    }

    panic("bug.\n");
}

int proc_rbt_insert(struct proc_t *proc, uint is_alloc_pid)
{
    spin_write_lock(&PROC.rbt_rwlock);

    if (is_alloc_pid) proc->pid = rbt_find_free_pid(&PROC.proc_rbt);

    int err = rbt_insert(&PROC.proc_rbt, proc);

    spin_write_unlock(&PROC.rbt_rwlock);

    return err;
}

int proc_rbt_find(uint pid, struct proc_t **res)
{
    spin_read_lock(&PROC.rbt_rwlock);

    int err = rbt_find(&PROC.proc_rbt, pid, res);

    spin_read_unlock(&PROC.rbt_rwlock);

    return err;
}

int proc_rbt_delete(uint pid)
{
    spin_write_lock(&PROC.rbt_rwlock);

    struct proc_t *proc;

    int err = rbt_find(&PROC.proc_rbt, pid, &proc);

    if (err != E_OK) goto ret1;

    struct rbt_node_t *deepset = rbt_delete_update_begin(&proc->pid_rbt);

    rbt_delete(&PROC.proc_rbt, &proc->pid_rbt);

    rbt_delete_update_end(deepset, update);

ret1:

    spin_write_unlock(&PROC.rbt_rwlock);

    return err;
}

int proc_rbt_init(void)
{
    PROC.proc_rbt.root = NULL;

    spin_rwlock_init(&PROC.rbt_rwlock);

    return E_OK;
}