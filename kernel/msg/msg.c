#include <error.h>
#include <idx_aclt.h>
#include <log.h>
#include <memory.h>
#include <msg.h>
#include <string.h>
#include <syscall.h>
#include <thread.h>
#include <util.h>

static struct
{
    struct idx_alct_t msg_alct;
    struct idx_alct_t box_alct;
} MSG;

int msg_newmsg(uint *id, addr_t addr, size_t size)
{
    struct msg_t *msg;

    int err = kmalloc((void *)&msg, sizeof(struct msg_t));

    if (err != E_OK) goto err1;

    mutex_init(&msg->lock);

    mutex_lock(&msg->lock);

    err = idx_alct_new(&MSG.msg_alct, msg, &msg->id);

    if (err != E_OK) goto err2;

    list_init(&msg->box_ln);

    err = kmalloc(&msg->addr, size);

    if (err != E_OK) goto err3;

    memcpy(msg->addr, addr, size);

    msg->size = size;

    msg->owner = proc_now();

    msg->state = MSG_UNSEND;

    if (id != NULL) *id = msg->id;

    info("sign up a new message, id: %d.\n", msg->id);

    mutex_unlock(&msg->lock);

    return err;

err3:
    idx_alct_delete(&MSG.msg_alct, msg->id);

err2:
    kmfree(msg);

err1:
    return err;
}

int msg_newbox(uint *id)
{
    struct msgbox_t *box;

    int err = kmalloc((void *)&box, sizeof(struct msgbox_t));

    if (err != E_OK) goto err1;

    mutex_init(&box->lock);

    mutex_lock(&box->lock);

    err = idx_alct_new(&MSG.box_alct, box, &box->id);

    if (err != E_OK) goto err2;

    box->nmsg = 0;

    box->blk_thread = NULL;

    box->state = BOX_RCVABLE;

    box->owner = proc_now();

    list_init(&box->msg_ls);

    if (id != NULL) *id = box->id;

    info("sign up a new box, id: %d.\n", box->id);

    mutex_unlock(&box->lock);

    return err;

err2:
    kmfree(box);

err1:
    return err;
}

int msg_send(uint box_id, uint msg_id)
{
    struct msg_t *msg = NULL;

    int err = idx_alct_find(&MSG.msg_alct, msg_id, (void *)&msg);

    if (err != E_OK) goto ret1;

    mutex_lock(&msg->lock);

    struct msgbox_t *box = NULL;

    err = idx_alct_find(&MSG.box_alct, box_id, (void *)&box);

    if (err != E_OK) goto ret2;

    mutex_lock(&box->lock);

    if (err = (msg->owner != proc_now() ? E_INVAL : E_OK)) goto ret3;

    if (err = (msg->state != MSG_UNSEND ? E_INVAL : E_OK)) goto ret3;

    list_push_back(&box->msg_ls, &msg->box_ln);

    box->nmsg++;

    if (box->state == BOX_RCV_BLK)
    {
        schd_run(box->blk_thread);

        box->blk_thread = NULL;
    }

    msg->state = MSG_SENDED;

    info("send message %d to box %d.\n", msg->id, box->id);

ret3:
    mutex_unlock(&box->lock);

ret2:
    mutex_unlock(&msg->lock);

ret1:
    return err;
}

int msg_read(uint msg_id, addr_t cache, addr_t offset, size_t size)
{
    struct msg_t *msg = NULL;

    int err = idx_alct_find(&MSG.msg_alct, msg_id, (void *)&msg);

    if (err != E_OK) goto ret1;

    mutex_lock(&msg->lock);

    if (err = (msg->owner != proc_now() ? E_INVAL : E_OK)) goto ret2;

    if (err = (msg->state != MSG_RECIEVED ? E_INVAL : E_OK)) goto ret2;

    if (err = (offset + size > msg->size ? E_INVAL : E_OK)) goto ret2;

    memcpy(cache, msg->addr + offset, size);

ret2:
    mutex_unlock(&msg->lock);

ret1:
    return err;
}

int msg_size(uint msg_id, size_t *size)
{
    struct msg_t *msg = NULL;

    int err = idx_alct_find(&MSG.msg_alct, msg_id, (void *)&msg);

    if (err != E_OK) goto ret1;

    mutex_lock(&msg->lock);

    if (err = (msg->owner != proc_now() ? E_INVAL : E_OK)) goto ret2;

    if (err = (msg->state != MSG_RECIEVED ? E_INVAL : E_OK)) goto ret2;

    *size = msg->size;

ret2:
    mutex_unlock(&msg->lock);

ret1:
    return err;
}

int msg_recieve(uint boxid, uint *msgid, uint is_block)
{
    struct msgbox_t *box = NULL;

    int err = idx_alct_find(&MSG.box_alct, boxid, (void *)&box);

    if (err != E_OK) goto ret1;

    mutex_lock(&box->lock);

    if ((err = (box->owner != proc_now() ? E_INVAL : err)) != E_OK) goto ret1;

    if ((err = (box->state == BOX_RCV_BLK ? E_BLOCK : err)) != E_OK) goto ret1;

    assert(box->state == BOX_RCVABLE);

    if (box->nmsg == 0)
    {
        if (is_block)
        {
            box->state = BOX_RCV_BLK;

            box->blk_thread = thread_now();

            intr_irq_save();

            mutex_unlock(&box->lock);

            schd_block(thread_now());

            mutex_lock(&box->lock);

            intr_irq_restore();

            box->state = BOX_RCVABLE;
        }
        else
        {
            err = E_NOMSG;

            goto ret1;
        }
    }

    assert(box->nmsg > 0);

    struct msg_t *msg = container_of(list_pop_front(&box->msg_ls), struct msg_t, box_ln);

    assert(msg->state == MSG_SENDED);

    msg->state = MSG_RECIEVED;

    info("recieve message %d from box %d.\n", msg->id, box->id);

    box->nmsg--;

    msg->owner = proc_now();

    if (msg != NULL) *msgid = msg->id;

ret1:
    mutex_unlock(&box->lock);

    return err;
}

int msg_delmsg(uint msg_id)
{
    struct msg_t *msg = NULL;

    int err = idx_alct_find(&MSG.msg_alct, msg_id, (void *)&msg);

    if (err != E_OK) goto err1;

    mutex_lock(&msg->lock);

    if (err = (msg->owner != proc_now() ? E_INVAL : E_OK)) goto err2;

    if (err = (msg->state == MSG_SENDED ? E_INVAL : E_OK)) goto err2;

    err = idx_alct_delete(&MSG.msg_alct, msg->id);

    assert(err == E_OK);

    kmfree(msg);

    info("delete message %d.\n", msg->id);

    return err;

err2:
    mutex_unlock(&msg->lock);

err1:
    return err;
}

int msg_delbox(uint box_id)
{
    struct msgbox_t *box = NULL;

    int err = idx_alct_find(&MSG.box_alct, box_id, (void *)&box);

    if (err != E_OK) goto err1;

    mutex_lock(&box->lock);

    if (err = (box->owner != proc_now() ? E_INVAL : E_OK)) goto err2;

    if (err = (box->state == BOX_RCV_BLK ? E_INVAL : E_OK)) goto err2;

    if (err = (box->nmsg > 0 ? E_INVAL : E_OK)) goto err2;

    err = idx_alct_delete(&MSG.box_alct, box->id);

    assert(err == E_OK);

    kmfree(box);

    info("delete box %d.\n", box->id);

    return err;

err2:
    mutex_unlock(&box->lock);

err1:
    return err;
}

int msg_init(void)
{
    idx_aclt_init(&MSG.msg_alct);

    idx_aclt_init(&MSG.box_alct);

    syscall_register(SYS_msg_newmsg, msg_newmsg, 3);

    syscall_register(SYS_msg_newbox, msg_newbox, 1);

    syscall_register(SYS_msg_send, msg_send, 2);

    syscall_register(SYS_msg_recieve, msg_recieve, 3);

    syscall_register(SYS_msg_read, msg_read, 4);

    syscall_register(SYS_msg_delmsg, msg_delmsg, 1);

    syscall_register(SYS_msg_delbox, msg_delbox, 1);

    return E_OK;
}