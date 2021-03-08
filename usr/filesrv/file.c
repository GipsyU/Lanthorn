#include <error.h>
#include <log.h>
#include <mm.h>
#include <string.h>
#include <utils.h>
#include <stdio.h>

#include "file.h"

struct file_t *root_file;

static char *path_cut(char *name)
{
    uint tmplen = strlen(name) + (name[strlen(name) - 1] == '/' ? 0 : 1);

    for (uint i = 1; i < strlen(name); ++i) if (name[i] == '/') ++tmplen;

    char *s;

    int err = malloc((void *)&s, tmplen);

    assert(err == E_OK);

    uint p = 0;

    for (uint i = 1; i < strlen(name); ++i)
    {
        s[p++] = name[i];

        if (name[i] == '/') s[p++] = '\0';
    }

    if (name[strlen(name) - 1] != '/') s[p++] = '\0';

    s[p++] = '\0';

    assert(p == tmplen);
}

static int _rbt_insert(struct rbt_t *rbt, struct file_t *file)
{
    struct rbt_node_t **p = &rbt->root;

    struct rbt_node_t *parent = NULL;

    struct file_t *_file;

    while (*p)
    {
        parent = *p;

        _file = container_of(parent, struct file_t, fa_rbt_node);

        int ans = strcmp(file->name, _file->name);

        if (ans == -1)
            p = &(*p)->l;

        else if (ans == 1)
            p = &(*p)->r;

        else
            return E_EXIST;
    }

    rb_link_node(&file->fa_rbt_node, parent, p);

    return E_OK;
}

static int rbt_insert(struct rbt_t *rbt, struct file_t *file)
{
    int err = _rbt_insert(rbt, file);

    if (err != E_OK) return err;

    rbt_insert_color(rbt, &file->fa_rbt_node);

    return err;
}

static int rbt_find(struct rbt_t *rbt, char *name, struct file_t **res)
{
    struct rbt_node_t *n = rbt->root;

    struct file_t *file;

    while (n)
    {
        file = container_of(n, struct file_t, fa_rbt_node);

        int ans = strcmp(name, file->name);

        if (ans == -1)
            n = n->l;

        else if (ans == 1)
            n = n->r;

        else
        {
            *res = file;

            return E_OK;
        }
    }

    return E_NOTFOUND;
}

int file_new(struct file_t **file, char *name, uint type, addr_t addr, size_t size)
{
    struct file_t *_file;

    int err = malloc((void *)&_file, sizeof(struct file_t));

    if (err != E_OK) return err;

    rbt_init(&_file->subfile_rbt);

    memset(_file->name, 0, FILE_MAX_NAME_LEN);

    strcpy(_file->name, name, strlen(name));

    *file = _file;

    return err;
}

int file_find(char *name, struct file_t **res)
{
    assert (name[0] == '/');

    struct file_t *file = root_file;

    name = path_cut(name);

    int err = E_OK;

    uint p = 0;

    while(name[p])
    {
        err = rbt_find(&file->subfile_rbt, name + p, &file);

        if (err != E_OK) return err;

        p += strlen(name + p) + 1;
    }

    assert(file->type == FILE_ENTITY);

    *res = file;

    return err;
}

int file_create(char *name, uint type, addr_t addr, size_t size)
{

    assert(name[0] == '/');

    name = path_cut(name);

    int err = E_OK;

    uint p = 0;

    struct file_t *file = root_file;

    while(name[p])
    {
        err = rbt_find(&file->subfile_rbt, name + p, &file);

        if (err != E_OK)
        {
            struct file_t *tmpfile = NULL;

            if (name[p + strlen(name + p) + 1]) err = file_new(&tmpfile, name + p, FILE_DIR, NULL, NULL);

            else err = file_new(&tmpfile, name + p, type, addr, size);

            if (err != E_OK) return err;

            err = rbt_insert(&file->subfile_rbt, tmpfile);

            assert(err == E_OK);

            err = rbt_find(&file->subfile_rbt, name + p, &file);

            assert(err == E_OK);

            assert(file == tmpfile);
        }

        p += strlen(name + p) + 1;
    }

    return err;
}