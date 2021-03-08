#include "file.h"
#include <srv.h>
#include <stdio.h>
#include <type.h>

extern char _binary_usr_filesrv_mkfs_fileimg_start[];
extern char _binary_usr_filesrv_mkfs_fileimg_end[];
extern char _binary_usr_filesrv_mkfs_fileimg_size[];

struct init_file_t
{
    char name[32];
    size_t size;
};

static int load_file(void)
{
    printf("%d.\n", _binary_usr_filesrv_mkfs_fileimg_size);

    for (struct init_file_t *file = (void *)_binary_usr_filesrv_mkfs_fileimg_start;
         (addr_t)file < (addr_t)_binary_usr_filesrv_mkfs_fileimg_end; file = (addr_t)file + file->size + sizeof(struct init_file_t))
    {
        printf("%p\n", file);
        printf("%s\n", file->name);

        printf("%d\n", file->size);

        // file_create(file->name, FILE_ENTITY, (addr_t)file + sizeof(struct init_file_t), file->size);

        // struct file_t *_file;

        // int err = file_find(file->name, &_file);
    }

    return 0;
}

int init(void)
{
    printf("file service is setting up.\n");

    srv_register("filesrv/open", 2);

    // file_new(&root_file, "/", FILE_DIR, NULL, NULL);

    load_file();

    printf("setting up file service finished.\n");

    return 0;
}