#include <error.h>
#include <log.h>
#include <srv.h>
#include "file.h"
void srv_read(void)
{
    while (1)
    {
        struct srv_callee_t called;

        int err = srv_called("filesrv/read", &called);

        if (err != E_OK) panic("filesrv/read called error, err = %s.\n", strerror(err));

        else
        {
            addr_t addr;

            size_t size;

            err = file_read(called.cache, &addr, &size);

            if (err == E_OK) srv_reply(called.sid, err, 1, addr, size);
            
            else srv_reply(called.sid, err, 0);
        }
    }
}