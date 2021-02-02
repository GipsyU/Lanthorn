#ifndef _MSG_H_
#define _MSG_H_
#include <arch/atomic.h>
#include <list.h>
#include <spinlock.h>

struct msg_t
{
    addr_t addr;
    size_t size;
    struct list_node_t list_node;
};

struct msgbox_t
{
    struct atomic_t nmsg;
    struct list_node_t list;
    struct spinlock_t list_lock;
};

int msg_newmsg(struct msg_t **msg, addr_t addr, size_t size);

int msg_newbox(struct msgbox_t **msgbox);

int msg_send(struct msgbox_t *msgbox, struct msg_t *msg);

int msg_recieve(struct msgbox_t *msgbox, struct msg_t **msg);

int msg_init(void);

#endif