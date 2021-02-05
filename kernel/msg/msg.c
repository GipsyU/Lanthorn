#include <error.h>
#include <log.h>
#include <memory.h>
#include <msg.h>
#include <string.h>
#include <syscall.h>
#include <util.h>

struct
{
    struct id_aclt_t msg_id_alct;

    struct id_aclt_t box_id_alct;
} MSG;

int msg_newmsg(struct msg_t **msg, addr_t addr, size_t size)
{
    int err = kmalloc((addr_t *)msg, sizeof(struct msg_t));

    if (err != E_OK) return err;

    err = id_alloc(&MSG.msg_id_alct, (addr_t)*msg, &(*msg)->id);

    if (err != E_OK) return err;

    list_init(&(*msg)->box_ln);

    if (addr < KERN_BASE)
    {
        err = kmalloc(&(*msg)->addr, size);

        if (err != E_OK) return err;

        memcpy((*msg)->addr, addr, size);
    }
    else
    {
        (*msg)->addr = addr;
    }

    (*msg)->size = size;

    return E_OK;
}

int msg_newbox(struct msgbox_t **msgbox)
{
    int err = kmalloc((addr_t *)msgbox, sizeof(struct msgbox_t));

    if (err != E_OK) return err;

    err = id_alloc(&MSG.box_id_alct, (addr_t)*msgbox, &(*msgbox)->id);

    if (err != E_OK) return err;

    spin_init(&(*msgbox)->lock);

    list_init(&(*msgbox)->msg_ls);

    (*msgbox)->nmsg = 0;

    return err;
}

int msg_send(struct msgbox_t *msgbox, struct msg_t *msg)
{
    int err = E_OK;

    assert(msg->addr >= KERN_BASE);

    spin_lock(&msgbox->lock);

    list_push_back(&msgbox->msg_ls, &msg->box_ln);

    msgbox->nmsg++;

    spin_unlock(&msgbox->lock);

    return E_OK;
}

int msg_recieve(struct msgbox_t *msgbox, struct msg_t **msg)
{
    spin_lock(&msgbox->lock);

    if (msgbox->nmsg == 0)
    {
        spin_unlock(&msgbox->lock);

        return E_NOMSG;
    }

    *msg = container_of(list_pop_front(&msgbox->msg_ls), struct msg_t, box_ln);

    msgbox->nmsg--;

    spin_unlock(&msgbox->lock);

    return E_OK;
}

// int msg_read(struct msg_t *msg, addr_t cache, size_t size)
// {
// }

// int msg_write(struct msg_t *msg)
// {
// }

int msg_init(void)
{
    addr_t addr = NULL;

    int err = kmalloc(&addr, PAGE_SIZE);

    if (err != E_OK) return err;

    id_init(&MSG.msg_id_alct, addr, PAGE_SIZE);

    err = kmalloc(&addr, PAGE_SIZE);

    if (err != E_OK) return err;

    id_init(&MSG.box_id_alct, addr, PAGE_SIZE);

    syscall_register(SYS_msg_newmsg, msg_newmsg, 3);

    syscall_register(SYS_msg_newbox, msg_newbox, 1);

    syscall_register(SYS_msg_send, msg_send, 2);

    syscall_register(SYS_msg_recieve, msg_recieve, 2);

    return E_OK;
}