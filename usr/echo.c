#include <error.h>
#include <file.h>
#include <mm.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 1 && argc != 2) return E_INVAL;

    char *content;

    int err = malloc((void *)&content, strlen(argv[0]) + 2);

    memcpy(content, argv[0], strlen(argv[0]));

    content[strlen(argv[0])] = '\n';

    if (err != E_OK) return err;

    if (argc == 1)
    {
        printf("%s", content);

        return E_OK;
    }

    char *path = argv[1];

    if (path[0] != '/')
    {
        int err = malloc((void *)&path, strlen(envp[0]) + strlen(argv[1]) + 1);

        if (err != E_OK) return err;

        memcpy(path, envp[0], strlen(envp[0]));

        memcpy(path + strlen(envp[0]), argv[1], strlen(argv[1]));

        path[strlen(envp[0]) + strlen(argv[1])] = 0;
    }

    return file_write(path, content, strlen(content) + 1);
}