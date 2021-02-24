#include <error.h>
#include <log.h>
#include <memory.h>
#include <msg.h>
#include <string.h>
#include <syscall.h>
#include <util.h>

#define MSG_MAX 256
static struct
{
    struct msg_t msg[MSG_MAX];
    struct msgbox_t box[MSG_MAX];
} MSG;

int msg_newmsg(uint *id, addr_t addr, size_t size)
{
    int err = E_OK;

    for (uint i = 0; i < MSG_MAX; ++i)
    {
        if (spin_trylock(&MSG.msg[i].lock))
        {
            if (MSG.msg[i].state == MSG_UNUSED)
            {
                list_init(&MSG.msg[i].box_ln);

                err = kmalloc(&MSG.msg[i].addr, size);

                assert(err == E_OK);

                memcpy(MSG.msg[i].addr, addr, size);
                
                MSG.msg[i].size = size;

                MSG.msg[i].state = MSG_UNSEND;

                *id = i;

                spin_unlock(&MSG.msg[i].lock);

                return E_OK;
            }

            spin_unlock(&MSG.msg[i].lock);
        }
    }

    return E_NOSLOT;
}
int msg_newbox(uint *id)
{
    int err = E_OK;

    for (uint i = 0; i < MSG_MAX; ++i)
    {
        if (spin_trylock(&MSG.box[i].lock))
        {
            if (MSG.box[i].state == BOX_UNUSED)
            {
                list_init(&MSG.box[i].msg_ls);

                MSG.box[i].nmsg = 0;

                MSG.box[i].state = BOX_USED;

                *id = i;

                spin_unlock(&MSG.box[i].lock);

                return E_OK;
            }

            spin_unlock(&MSG.box[i].lock);
        }
    }

    return E_NOSLOT;
}

int msg_send(uint box_id, uint msg_id)
{
    struct msgbox_t *msgbox = NULL;

    spin_lock(&MSG.msg[msg_id].lock);

    if (MSG.msg[msg_id].state != MSG_UNSEND)
    {
        spin_unlock(&MSG.msg[msg_id].lock);

        return E_INVAL;
    }
    
    assert(MSG.msg[msg_id].addr >= KERN_BASE);

    spin_lock(&MSG.box[box_id].lock);

    if (MSG.box[box_id].state != BOX_USED)
    {
        spin_unlock(&MSG.box[box_id].lock);

        spin_unlock(&MSG.msg[msg_id].lock);

        return E_INVAL;
    }

    list_push_back(&MSG.box[box_id].msg_ls, &MSG.msg[msg_id].box_ln);

    MSG.box[box_id].nmsg++;

    MSG.msg[msg_id].state = MSG_SENDED;

    spin_unlock(&MSG.box[box_id].lock);

    spin_unlock(&MSG.msg[msg_id].lock);

    return E_OK;
}

int msg_recieve(uint box_id, uint *msg_id)
{
    spin_lock(&MSG.box[box_id].lock);

    if (MSG.box[box_id].state != BOX_USED)
    {
        spin_unlock(&MSG.box[box_id].lock);

        return E_INVAL;
    }

    if (MSG.box[box_id].nmsg == 0)
    {
        spin_unlock(&MSG.box[box_id].lock);

        return E_NOMSG;
    }

    struct msg_t *msg = container_of(list_pop_front(&MSG.box[box_id].msg_ls), struct msg_t, box_ln);

    assert(msg->state == MSG_SENDED);

    msg->state = MSG_RECIEVED;

    MSG.box[box_id].nmsg--;

    *msg_id = msg->id;

    spin_unlock(&MSG.box[box_id].lock);

    return E_OK;
}

int msg_read(uint msg_id, addr_t cache, size_t size)
{
    spin_lock(&MSG.msg[msg_id].lock);

    if (MSG.msg[msg_id].state != MSG_RECIEVED)
    {
        spin_unlock(&MSG.msg[msg_id].lock);

        return E_INVAL;
    }

    if (MSG.msg[msg_id].size > size)
    {
        spin_unlock(&MSG.msg[msg_id].lock);

        return E_NOCACHE;
    }

    uint *x = MSG.msg[msg_id].addr;

    memcpy(cache, MSG.msg[msg_id].addr, MSG.msg[msg_id].size);

    spin_unlock(&MSG.msg[msg_id].lock);

    return E_OK;
}

int msg_size(uint msg_id, size_t *size)
{
    spin_lock(&MSG.msg[msg_id].lock);
    
    if (MSG.msg[msg_id].state != MSG_RECIEVED)
    {
        spin_unlock(&MSG.msg[msg_id].lock);

        return E_INVAL;
    }

    *size = MSG.msg[msg_id].size;

    spin_unlock(&MSG.msg[msg_id].lock);

    return E_OK;
}

int msg_init(void)
{
    for (uint i = 0; i < MSG_MAX; ++i)
    {
        spin_init(&MSG.msg[i].lock);

        MSG.msg[i].id = i;

        MSG.msg[i].state = MSG_UNUSED;
    }

    for (uint i = 0; i < MSG_MAX; ++i)
    {
        spin_init(&MSG.box[i].lock);

        MSG.box[i].id = i;
        
        MSG.box[i].state = BOX_UNUSED;
    }

    syscall_register(SYS_msg_newmsg, msg_newmsg, 3);

    syscall_register(SYS_msg_newbox, msg_newbox, 1);

    syscall_register(SYS_msg_send, msg_send, 2);

    syscall_register(SYS_msg_recieve, msg_recieve, 2);

    syscall_register(SYS_msg_read, msg_read, 3);

    return E_OK;
}