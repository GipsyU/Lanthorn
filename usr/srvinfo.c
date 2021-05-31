#include <error.h>
#include <mm.h>
#include <srv.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 0) return E_INVAL;

    struct srv_info_t *info;

    int err = srv_info(&info);

    if (err != E_OK) return err;

    printf("SRVNAME\t\tPID\tPROCNAME\n");

    for (int now = 0; info[now].proc_info.pid; ++now)
    {
        printf("%s\t%d\t%s\n", info[now].name, info[now].proc_info.pid, info[now].proc_info.name);
    }

    return err;
}