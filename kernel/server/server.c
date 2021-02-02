#include <arch/basic.h>
#include <error.h>
#include <memory.h>
#include <rbt.h>
#include <server.h>
#include <string.h>
#include <syscall.h>
#include <util.h>

static struct rbt_t server_rbt;

static int _rbt_insert(struct rbt_t *rbt, struct server_t *server)
{
    struct rbt_node_t **p = &rbt->root;

    struct rbt_node_t *parent = NULL;

    struct server_t *sv;

    while (*p)
    {
        parent = *p;

        sv = container_of(parent, struct server_t, rbt_node);

        int ans = strcmp(server->name, sv->name);

        if (ans == -1)
        {
            p = &(*p)->l;
        }
        else if (ans == 1)
        {
            p = &(*p)->r;
        }
        else
        {
            return E_EXIST;
        }
    }

    rb_link_node(&server->rbt_node, parent, p);

    return E_OK;
}

static int rbt_insert(struct rbt_t *rbt, struct server_t *server)
{
    int err = _rbt_insert(rbt, server);

    if (err != E_OK) return err;

    rbt_insert_color(rbt, &server->rbt_node);

    return err;
}

static int rbt_find(struct rbt_t *rbt, char *name, struct server_t **res)
{
    struct rbt_node_t *n = rbt->root;

    struct server_t *server;

    while (n)
    {
        server = container_of(n, struct server_t, rbt_node);

        int ans = strcmp(name, server->name);

        if (ans == -1)
        {
            n = n->l;
        }
        else if (ans == 1)
        {
            n = n->r;
        }
        else
        {
            *res = server;

            return E_OK;
        }
    }

    return E_NOTFOUND;
}


/**
 * FIXME: clean up
 */

int server_register(char *name, addr_t hdl, uint narg, uint *arg_ptr_size)
{
    int err = E_OK;

    struct server_t *server;

    err = kmalloc(server, sizeof(struct server_t));

    if (err != E_OK) return err;

    strcpy(server->name, name, strlen(name));

    server->hdl = hdl;

    server->narg = narg;

    server->proc = proc_now();

    for (uint i = 0; i < narg; ++i)
    {
        server->arg_ptr_size[i] = arg_ptr_size[i];
    }

    err = rbt_insert(&server_rbt, server);

    return err;
}

int server_call(char *name, long *args)
{
    struct server_t *server;

    int err = rbt_find(&server_rbt, name, &server);

    if (err != E_OK) return err;

    for (uint i = 0; i < server->narg; ++i)
    {
        if (server->arg_ptr_size[i] > 0)
        {
            
        }
    }
}

int server_init()
{
}