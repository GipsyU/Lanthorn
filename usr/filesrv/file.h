#ifndef _USR_FILESRV_FILE_H_
#define _USR_FILESRV_FILE_H_

#include <rwlock.h>
#include <type.h>
#include <rbt.h>

#define FILE_MAX_NAME_LEN 32
#define NR_MAX_FILE 128

struct file_t
{
    enum STATE
    {
        FILE_UNUSED,
        FILE_USED
    } state;
    enum TYPE
    {
        FILE_ENTITY,
        FILE_DIR
    } type;
    char name[FILE_MAX_NAME_LEN];
    addr_t data;
    size_t size;
    struct rbt_t subfile_rbt;
    struct rbt_node_t fa_rbt_node;
    struct file_t *ffile;
    struct rwlock_t lock;
};

extern struct file_t *root_file;

int init(void);

int file_read(char *name, addr_t *addr, size_t *size);

int file_new(struct file_t **file, char *name, uint type, addr_t addr, size_t size);

int file_find(char *name, struct file_t **res);

int file_create(char *name, uint type, addr_t addr, size_t size);

void srv_read(void);

#endif