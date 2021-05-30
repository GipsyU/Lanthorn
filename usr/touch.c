#include <error.h>
#include <file.h>
#include <mm.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 1) return E_INVAL;

    char *path = argv[0];

    if (path[0] != '/')
    {
        int err = malloc((void *)&path, strlen(envp[0]) + strlen(argv[0]) + 1);

        if (err != E_OK) return err;

        memcpy(path, envp[0], strlen(envp[0]));

        memcpy(path + strlen(envp[0]), argv[0], strlen(argv[0]));

        path[strlen(envp[0]) + strlen(argv[0])] = 0;
    }

    return file_create(path);
}