#include "file.h"
#include <srv.h>
#include <stdio.h>
#include <type.h>

extern char _binary_usr_fssrv_mkfs_fileimg_start[];
extern char _binary_usr_fssrv_mkfs_fileimg_end[];
extern char _binary_usr_fssrv_mkfs_fileimg_size[];

struct init_file_t
{
    char name[32];
    size_t size;
};

static int load_file(void)
{
    for (struct init_file_t *file = (void *)_binary_usr_fssrv_mkfs_fileimg_start;
         (addr_t)file < (addr_t)_binary_usr_fssrv_mkfs_fileimg_end;
         file = (addr_t)file + file->size + sizeof(struct init_file_t))
    {
        printf("%s\n", file->name);

        printf("%d\n", file->size);

        file_create(file->name, (addr_t)file + sizeof(struct init_file_t), file->size);

        struct file_t *_file;

        int err = file_find(file->name, &_file);
    }

    return 0;
}

int init(void)
{
    printf("file service is setting up.\n");

    srv_register("fssrv/read", 1);

    srv_register("fssrv/write", 2);

    srv_register("fssrv/create", 1);

    srv_register("fssrv/delete", 1);

    srv_register("fssrv/subfile", 1);

    srv_register("fssrv/find", 1);

    file_new(&root_file, "/", FILE_DIR, NULL, NULL);

    load_file();

    printf("setting up file service finished.\n");

    return 0;
}