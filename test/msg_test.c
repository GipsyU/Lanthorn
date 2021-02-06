#include <error.h>
#include <log.h>
#include <msg.h>
#include <thread.h>

static uint boxid = 1234567;

int T1(void)
{
    uint msgid;

    char s[10] = "MSG.\n";

    int err = msg_newmsg(&msgid, s, 6);

    if (err != E_OK)
    {
        error("T1 msg new error.\n");
    }
    else
    {
        debug("T1 msg new success %p.\n", msgid);
    }

    while (boxid == 1234567)
        ;

    err = msg_send(boxid, msgid);

    if (err == E_OK)
    {
        debug("send success.\n");
    }
    else
    {
        error("%s.\n", strerror(err));
    }
    return E_OK;
}

int T2(void)
{
    uint msgid;

    uint bbox = 0;

    int err = msg_newbox(&bbox);

    if (err != E_OK) error("T2.\n");

    boxid = bbox;

    while (1)
    {
        if (msg_recieve(bbox, &msgid) == E_OK)
        {
            debug ("recieved.\n");

            addr_t addr;

            assert( kmalloc(&addr, PAGE_SIZE) == E_OK);

            msg_read(msgid, addr, PAGE_SIZE);

            print("%s", addr);
        }
    }
}

int msg_test(void)
{
    struct thread_t *t1, *t2;

    // thread_kern_new(&t1, T1, 0);

    // thread_kern_new(&t2, T2, 0);

    return E_OK;
}