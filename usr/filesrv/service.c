#include <error.h>
#include <log.h>
#include <mm.h>
#include <proc.h>
#include <srv.h>
#include <string.h>

#include "file.h"

static void service_read_hdl(addr_t args)
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

static void service_listen(char *subsrv, addr_t routine)
{
    char *service;

    int err = malloc((void *)&service, strlen("filesrv/") + strlen(subsrv) + 1);

    if (err != E_OK) panic("bug\n");

    strcpy(service, "filesrv/", strlen("filesrv/"));

    strcpy(service + strlen("filesrv/"), subsrv, strlen(subsrv));

    service[strlen("filesrv/") + strlen(subsrv)] = 0;

    while (1)
    {
        struct srv_callee_t called;

        int err = srv_called(service, &called);

        if (err != E_OK)
            panic("%s called error, err = %s.\n", service, strerror(err));

        else
        {
            struct srv_callee_t *callee = NULL;

            int err = malloc((void *)&callee, sizeof(struct srv_callee_t));

            if (err != E_OK) panic("srv bug.\n");

            memcpy(callee, &called, sizeof(struct srv_callee_t));

            struct thread_attr_t attr;

            err = thread_create(NULL, routine, 1, callee);

            if (err == E_OK)
                info("%s: create thread to handler request.\n", service);

            else
                panic("bug.\n");
        }
    }
}

void service_enable(void)
{
    struct thread_attr_t attr;

    attr.arga = "read";

    int err = thread_create(NULL, service_listen, 2, "read", service_read_hdl);

    if (err != E_OK) panic("bug\n");

    info("filesrv: enable service success.\n");

    thread_exit();
}
