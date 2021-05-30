#include <error.h>
#include <mm.h>
#include <proc.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 0) return E_INVAL;

    struct proc_info_t *info;

    int err = proc_info(&info);

    if (err != E_OK) return err;

    printf("PID\tNAME\n");

    for (int now = 0; info[now].pid; ++now)
    {
        printf("%d\t%s\n", info[now].pid, info[now].name);
    }

    return err;
}