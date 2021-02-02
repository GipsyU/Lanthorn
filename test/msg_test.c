#include <error.h>
#include <log.h>
#include <msg.h>

int msg_test(void)
{
    struct msg_t *msg;

    msg_newmsg(&msg, "asd", 3);
    
    debug("ok\n");

    return E_OK;
}