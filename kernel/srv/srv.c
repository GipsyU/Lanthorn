#include <arch/basic.h>
#include <error.h>
#include <memory.h>
#include <proc.h>
#include <rbt.h>
#include <spinlock.h>
#include <srv.h>
#include <string.h>
#include <syscall.h>
#include <util.h>

static struct
{
    struct rbt_t srv_rbt;

    struct spin_rwlock_t rbt_rwlock;
} SRV;

static int _rbt_insert(struct rbt_t *rbt, struct srv_t *server)
{
    struct rbt_node_t **p = &rbt->root;

    struct rbt_node_t *parent = NULL;

    struct srv_t *sv;

    while (*p)
    {
        parent = *p;

        sv = container_of(parent, struct srv_t, rbt_node);

        int ans = strcmp(server->name, sv->name);

        if (ans == -1)
            p = &(*p)->l;

        else if (ans == 1)
            p = &(*p)->r;

        else
            return E_EXIST;
    }

    rb_link_node(&server->rbt_node, parent, p);

    return E_OK;
}

static int rbt_insert(struct rbt_t *rbt, struct srv_t *server)
{
    int err = _rbt_insert(rbt, server);

    if (err != E_OK) return err;

    rbt_insert_color(rbt, &server->rbt_node);

    return err;
}

static int rbt_find(struct rbt_t *rbt, char *name, struct srv_t **res)
{
    struct rbt_node_t *n = rbt->root;

    struct srv_t *server;

    while (n)
    {
        server = container_of(n, struct srv_t, rbt_node);

        int ans = strcmp(name, server->name);

        if (ans == -1)
            n = n->l;

        else if (ans == 1)
            n = n->r;

        else
        {
            *res = server;

            return E_OK;
        }
    }

    return E_NOTFOUND;
}

static int srv_sys_register(char *name, uint nparam)
{
    if (strlen_s(name, MAX_SRV_NAME_LEN) == MAX_SRV_NAME_LEN) return E_INVAL;

    if (nparam > MAX_CALL_PARAM) return E_INVAL;

    spin_write_lock(&SRV.rbt_rwlock);

    struct srv_t *srv = NULL;

    if (rbt_find(&SRV.srv_rbt, name, &srv) != E_NOTFOUND) return E_EXIST;

    int err = kmalloc((addr_t *)&srv, sizeof(struct srv_t));

    if (err != E_OK) return err;

    memset(srv->name, 0, MAX_SRV_NAME_LEN);

    strcpy(srv->name, name, strlen(name));

    err = msg_newbox(&srv->boxid);

    if (err != E_OK) return err;

    srv->owner = proc_now();

    srv->nparam = nparam;

    err = rbt_insert(&SRV.srv_rbt, srv);

    spin_write_unlock(&SRV.rbt_rwlock);

    assert(err == E_OK);

    return E_OK;
}

static int srv_call(char *name, addr_t *param, struct srv_replyee_t *replyee, uint iskern)
{
    struct srv_t *srv;

    int err = rbt_find(&SRV.srv_rbt, name, &srv);

    if (err != E_OK) return err;

    addr_t msgcache = NULL;

    size_t msgsz = sizeof(struct srv_call_t);

    for (uint i = 0; i < srv->nparam; ++i) msgsz += param[i * 2 + 1];

    err = kmalloc(&msgcache, msgsz);

    if (err != E_OK) return err;

    addr_t offset = sizeof(struct srv_call_t);

    struct srv_call_t *srv_call = msgcache;

    srv_call->arg_sz = 0;

    for (uint i = 0; i < srv->nparam; ++i)
    {
        srv_call->sz[i] = param[i * 2 + 1];

        srv_call->arg_sz += param[i * 2 + 1];

        memcpy(msgcache + offset, param[i * 2], param[i * 2 + 1]);

        offset += param[i * 2 + 1];
    }

    assert(offset == msgsz);

    uint msgid, retboxid, retmsgid;

    err = msg_newbox(&retboxid);

    if (err != E_OK) return err;

    srv_call->retboxid = retboxid;

    err = msg_newmsg(&msgid, msgcache, msgsz);

    if (err != E_OK) return err;

    err = msg_send(srv->boxid, msgid);

    if (err != E_OK) return err;

    err = msg_recieve(retboxid, &retmsgid, 1);

    assert(err == E_OK);

    if (err != E_OK) return err;

    struct srv_reply_t reply;

    if (err != E_OK) return err;

    err = msg_read(retmsgid, &reply, 0, sizeof(reply));

    if (err != E_OK) return err;

    size_t sz = 0;

    for (uint i = 0; i < reply.narg; ++i) sz += reply.sz[i];

    if (sz > 0)
    {
        if (iskern)
            err = kmalloc(&replyee->cache, sz);

        else
            err = umalloc(&proc_now()->um, &replyee->cache, sz);

        if (err != E_OK) return err;

        err = msg_read(retmsgid, replyee->cache, sizeof(reply), sz);

        if (err != E_OK) return err;
    }

    replyee->err = reply.err;

    replyee->narg = reply.narg;

    memcpy(replyee->sz, reply.sz, sizeof(reply.sz));

    return err;
}

static int srv_called(char *name, struct srv_callee_t *srvcalled)
{
    struct srv_t *srv;

    spin_read_lock(&SRV.rbt_rwlock);

    int err = rbt_find(&SRV.srv_rbt, name, &srv);

    spin_read_unlock(&SRV.rbt_rwlock);

    if (err != E_OK) goto ret1;

    if ((err = (srv->owner != proc_now() ? E_INVAL : err)) != E_OK) goto ret1;

    uint msg_id;

    size_t msg_sz;

    struct srv_call_t srvcall;

    assert(msg_recieve(srv->boxid, &msg_id, 1) == E_OK);

    assert(msg_size(msg_id, &msg_sz) == E_OK);

    assert(msg_sz >= sizeof(srvcall));

    err = msg_read(msg_id, &srvcall, 0, sizeof(srvcall));

    assert(err == E_OK);

    if (srvcall.arg_sz > 0)
    {
        err = umalloc(&proc_now()->um, &srvcalled->cache, srvcall.arg_sz);

        if (err != E_OK) goto ret1;

        err = msg_read(msg_id, srvcalled->cache, sizeof(struct srv_call_t), srvcall.arg_sz);

        assert(err == E_OK);
    }

    memcpy(&srvcalled->sz, &srvcall.sz, sizeof(srvcall.sz));

    srvcalled->sid = srvcall.retboxid;

    srvcalled->arg_sz = srvcall.arg_sz;

ret1:
    return err;
}

static int srv_reply(struct srv_replyer_t *replyer)
{
    uint sz = sizeof(struct srv_reply_t);

    for (uint i = 0; i < replyer->narg; ++i) sz += replyer->sz[i];

    struct srv_reply_t *reply;

    int err = kmalloc((void *)&reply, sz);

    if (err != E_OK) return err;

    uint offset = sizeof(struct srv_reply_t);

    for (uint i = 0; i < replyer->narg; ++i)
    {
        memcpy((addr_t)reply + offset, replyer->ad[i], replyer->sz[i]);

        offset += replyer->sz[i];
    }

    reply->err = replyer->err;

    reply->narg = replyer->narg;

    memcpy(&reply->sz, &replyer->sz, sizeof(replyer->sz));

    uint msg;

    err = msg_newmsg(&msg, reply, sz);

    if (err != E_OK) return err;

    err = msg_send(replyer->sid, msg);

    return err;
}

static int srv_sys_reply(struct srv_replyer_t *replyer)
{
    return srv_reply(replyer);
}

int srv_kern_call(char *name, struct srv_replyee_t *replyee, ...)
{
    addr_t param = (addr_t)&replyee + sizeof(replyee);

    return srv_call(name, param, replyee, 1);
}

static int srv_sys_call(char *name, addr_t *param, struct srv_replyee_t *replyee)
{
    return srv_call(name, param, replyee, 0);
}

static int srv_sys_called(char *name, struct srv_callee_t *srvcalled)
{
    return srv_called(name, srvcalled);
}

struct srv_info_t
{
    char name[MAX_SRV_NAME_LEN];
    struct proc_info_t proc_info;
};

static int srv_sys_info(struct srv_info_t **info)
{
    spin_read_lock(&SRV.rbt_rwlock);

    int tot = 1;

    rbt_rep(&SRV.srv_rbt, p)
    {
        ++tot;
    }

    struct srv_info_t *_info;

    int err = umalloc(&proc_now()->um, (void *)&_info, tot * sizeof(struct srv_info_t));

    if (err != E_OK) return err;

    int now = 0;

    rbt_rep(&SRV.srv_rbt, p)
    {
        struct srv_t *srv = container_of(p, struct srv_t, rbt_node);

        memcpy(_info[now].name, srv->name, sizeof(srv->name));

        memcpy(_info[now].proc_info.name, srv->owner->name, sizeof(srv->owner->name));

        _info[now].proc_info.pid = srv->owner->pid;

        ++now;
    }

    _info[now].proc_info.pid = 0;

    spin_read_unlock(&SRV.rbt_rwlock);

    *info = _info;

    return E_OK;
}

int srv_init(void)
{
    spin_rwlock_init(&SRV.rbt_rwlock);

    syscall_register(SYS_srv_call, srv_sys_call, 3);

    syscall_register(SYS_srv_called, srv_sys_called, 2);

    syscall_register(SYS_srv_reply, srv_sys_reply, 1);

    syscall_register(SYS_srv_register, srv_sys_register, 2);

    syscall_register(SYS_srv_info, srv_sys_info, 1);

    return E_OK;
}