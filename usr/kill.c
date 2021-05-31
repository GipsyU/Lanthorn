#include <error.h>
#include <proc.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 1) return E_INVAL;

    int len = strlen(argv[0]);

    int pid = 0;

    for (int i = 0; i < len; ++i)
    {
        if (argv[0][i] < '0' || argv[0][i] > '9') return E_INVAL;

        pid = pid * 10 + argv[0][i] - '0';
    }

    return proc_kill(pid);
}