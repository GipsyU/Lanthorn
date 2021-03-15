#ifndef _MSG_H_
#define _MSG_H_
#include <list.h>
#include <mutex.h>
#include <proc.h>
#include <spinlock.h>
struct msg_t
{
    int id;
    enum MSG_STATE
    {
        MSG_UNUSED,
        MSG_UNSEND,
        MSG_SENDED,
        MSG_RECIEVED
    } state;
    uint from;
    uint to;
    addr_t addr;
    size_t size;
    struct proc_t *owner;
    struct mutex_t lock;
    struct list_node_t box_ln;
};

struct msgbox_t
{
    uint id;
    enum BOX_STATE
    {
        BOX_UNUSED,
        BOX_RCVABLE,
        BOX_RCV_BLK
    } state;
    uint nmsg;
    struct proc_t *owner;
    struct thread_t *blk_thread;
    struct mutex_t lock;
    struct list_node_t msg_ls;
};

int msg_newmsg(uint *id, addr_t addr, size_t size);

int msg_newbox(uint *id);

int msg_send(uint box_id, uint msg_id);

int msg_recieve(uint box_id, uint *msg_id, uint is_block);

int msg_read(uint msg_id, addr_t cache, addr_t offset, size_t size);

int msg_size(uint msg_id, size_t *size);

int msg_init(void);

#endif