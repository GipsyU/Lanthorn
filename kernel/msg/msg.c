#include <error.h>
#include <log.h>
#include <memory.h>
#include <msg.h>
#include <string.h>
#include <syscall.h>
#include <util.h>

int msg_newmsg(struct msg_t **msg, addr_t addr, size_t size)
{
    int err = kmalloc((addr_t *)msg, sizeof(struct msg_t));

    if (err != E_OK) return err;

    list_init(&(*msg)->list_node);

    if (addr < KERN_BASE)
    {
        err = kmalloc(&(*msg)->addr, size);

        if (err != E_OK)
        {
            kmfree(*msg);

            return err;
        }

        memcpy((*msg)->addr, addr, size);
    }
    else
    {
        (*msg)->addr = addr;
    }

    (*msg)->size = size;

    debug("ok\n");

    return E_OK;
}

int msg_newbox(struct msgbox_t **msgbox)
{
    int err = kmalloc((addr_t *)msgbox, sizeof(struct msgbox_t));

    if (err != E_OK) return err;

    spin_init(&(*msgbox)->list_lock);

    list_init(&(*msgbox)->list);

    atomic_set(&(*msgbox)->nmsg, 0);

    return err;
}

int msg_send(struct msgbox_t *msgbox, struct msg_t *msg)
{
    int err = E_OK;

    if (msg->addr < KERN_BASE)
    {
        addr_t addr;

        err = kmalloc(&addr, msg->size);

        if (err != E_OK) return err;

        memcpy(addr, msg->addr, msg->size);

        msg->addr = addr;
    }

    spin_lock(&msgbox->list_lock);

    list_push_back(&msgbox->list, &msg->list_node);

    atomic_add(&msgbox->nmsg, 1);

    spin_unlock(&msgbox->list_lock);

    return E_OK;
}

int msg_recieve(struct msgbox_t *msgbox, struct msg_t **msg)
{
    if (atomic_read(&msgbox->nmsg) == 0) return E_NOMSG;

    spin_lock(&msgbox->list_lock);

    *msg = container_of(list_pop_front(&msgbox->list), struct msg_t, list_node);

    atomic_sub(&msgbox->nmsg, 1);

    spin_unlock(&msgbox->list_lock);

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
    syscall_register(SYS_msg_newmsg, msg_newmsg, 3);

    syscall_register(SYS_msg_newbox, msg_newbox, 1);

    syscall_register(SYS_msg_send, msg_send, 2);

    syscall_register(SYS_msg_recieve, msg_recieve, 2);

    return E_OK;
}