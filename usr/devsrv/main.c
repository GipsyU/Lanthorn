#include <srv.h>
#include <type.h>
#include <stdio.h>
#include <proc.h>

int main(void)
{
    printf("device service is setting up...\n");

    int err = srv_register("devsrv/test", 1);

    addr_t cache;

    uint sid;

    return 0;
}