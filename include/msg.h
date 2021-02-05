#ifndef _MSG_H_
#define _MSG_H_
#include <id.h>
#include <list.h>
#include <proc.h>
#include <spinlock.h>

struct msg_t
{
    id_t id;
    enum MSG_STATE
    {
        MSG_UNUSED
    } state;
    id_t from;
    id_t to;
    addr_t addr;
    size_t size;
    struct list_node_t box_ln;
};

struct msgbox_t
{
    id_t id;
    uint nmsg;
    struct proc_t *proc;
    struct spinlock_t lock;
    struct list_node_t msg_ls;
};

int msg_newmsg(struct msg_t **msg, addr_t addr, size_t size);

int msg_newbox(struct msgbox_t **msgbox);

int msg_send(struct msgbox_t *msgbox, struct msg_t *msg);

int msg_recieve(struct msgbox_t *msgbox, struct msg_t **msg);

int msg_init(void);

#endif