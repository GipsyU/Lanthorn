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

static void service_write_hdl(addr_t args)
{
    struct srv_callee_t *callee = (void *)args;

    char *path = callee->cache;

    addr_t addr = callee->cache + callee->sz[0];

    int err = file_write(path, addr, callee->sz[1]);

    srv_reply(callee->sid, err, 0);

    thread_exit();
}

static void service_create_hdl(addr_t args)
{
    struct srv_callee_t *callee = (void *)args;

    char *path = callee->cache;

    int err = file_create(path, NULL, NULL);

    srv_reply(callee->sid, err, 0);

    thread_exit();
}

static void service_delete_hdl(addr_t args)
{
    struct srv_callee_t *callee = (void *)args;

    char *path = callee->cache;

    int err = file_delete(path);

    srv_reply(callee->sid, err, 0);

    thread_exit();
}

static void service_subfile_hdl(addr_t args)
{
    struct srv_callee_t *callee = (void *)args;

    char *path = callee->cache;

    addr_t addr;

    size_t size;

    int err = file_subfile(path, &addr, &size);

    if (err == E_OK)
        srv_reply(callee->sid, err, 1, addr, size);

    else
        srv_reply(callee->sid, err, 0);

    thread_exit();
}

static void service_find_hdl(addr_t args)
{
    struct srv_callee_t *callee = (void *)args;

    char *path = callee->cache;

    struct file_t *file;

    int err = file_find(path, &file);

    srv_reply(callee->sid, err, 0);

    thread_exit();
}

static void service_listen(char *subsrv, addr_t routine)
{
    char *service;

    int err = malloc((void *)&service, strlen("fssrv/") + strlen(subsrv) + 1);

    if (err != E_OK) panic("bug\n");

    strcpy(service, "fssrv/", strlen("fssrv/"));

    strcpy(service + strlen("fssrv/"), subsrv, strlen(subsrv));

    service[strlen("fssrv/") + strlen(subsrv)] = 0;

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
    int err = thread_create(NULL, service_listen, 2, "read", service_read_hdl);
    
    if (err != E_OK) panic("bug\n");

    err = thread_create(NULL, service_listen, 2, "write", service_write_hdl);

    if (err != E_OK) panic("bug\n");

    err = thread_create(NULL, service_listen, 2, "create", service_create_hdl);

    if (err != E_OK) panic("bug\n");

    err = thread_create(NULL, service_listen, 2, "delete", service_delete_hdl);

    if (err != E_OK) panic("bug\n");

    err = thread_create(NULL, service_listen, 2, "subfile", service_subfile_hdl);

    if (err != E_OK) panic("bug\n");

    err = thread_create(NULL, service_listen, 2, "find", service_find_hdl);

    if (err != E_OK) panic("bug\n");

    info("fssrv: enable service success.\n");

    thread_exit();
}
