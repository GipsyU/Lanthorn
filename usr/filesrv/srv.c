#include "file.h"
#include <error.h>
#include <log.h>
#include <mm.h>
#include <proc.h>
#include <srv.h>
#include <string.h>

static void solve_read(addr_t args)
{
    struct srv_callee_t *callee = (void *)args;

    addr_t addr;

    size_t size;

    int err = file_read(callee->cache, &addr, &size);

    if (err == E_OK)
        srv_reply(callee->sid, err, 1, addr, size);

    else
        srv_reply(callee->sid, err, 0);

    thread_exit();
}

void srv_read(void)
{
    while (1)
    {
        struct srv_callee_t called;

        int err = srv_called("filesrv/read", &called);

        if (err != E_OK)
            panic("filesrv/read called error, err = %s.\n", strerror(err));

        else
        {
            struct srv_callee_t *callee = NULL;

            int err = malloc((void *)&callee, sizeof(struct srv_callee_t));

            if (err != E_OK) panic("srv read bug.\n");

            memcpy(callee, &called, sizeof(struct srv_callee_t));

            uint tid;

            struct thread_attr_t attr;

            attr.arga = callee;

            attr.stk_sz = NULL;

            attr.tid = NULL;

            err = thread_create(&tid, solve_read, &attr);

            if (err == E_OK) info("filesrv: create thread to handler read request.\n");

            else panic("bug.\n");
        }
    }
}
