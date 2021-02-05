#include <error.h>
#include <log.h>
#include <msg.h>
#include <thread.h>

static struct msgbox_t *box = 0;

int T1(void)
{
    struct msg_t *msg;

    char s[10] = "MSG.\n";

    int err = msg_newmsg(&msg, s, 6);

    if (err != E_OK)
    {
        error("T1 msg new error.\n");
    }
    else
    {
        debug("T1 msg new success.\n");
    }

    while (box == 0)
        ;

    err = msg_send(box, msg);

    if (err == E_OK)
    {
        debug("send success.\n");
    }
    else
    {
        error("%s.\n", strerror(err));
    }
    while (1)
        ;
    return E_OK;
}

int T2(void)
{
    debug("OK.\n");

    struct msg_t *msg;

    struct msgbox_t *bbox;

    int err = msg_newbox(&bbox);

    if (err != E_OK) error("T2.\n");

    box = bbox;

    while (1)
    {
        if (msg_recieve(bbox, &msg) == E_OK)
        {
            print("%s", msg->addr);
        }
    }
}

int msg_test(void)
{
    struct thread_t *t1, *t2;

    thread_kern_new(&t1, T1, 0);

    thread_kern_new(&t2, T2, 0);

    debug("ok\n");

    return E_OK;
}