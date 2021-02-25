#include <mm.h>
#include <msg.h>
#include <proc.h>
#include <srv.h>
#include <stdio.h>
#include <type.h>
#include <sysctrl.h>

static void run(void)
{
    printf("test.\n");

    addr_t cache;

    char ss[] = "srv call.\n";

    srv_call("FSSRV/test", &cache, ss, sizeof(ss));

    char *s = (void *) cache;

    printf("%s",s);

    sysctrl_poweroff();
    thread_exit();

}

int main(void)
{
    long tid = 0;

    printf("Hello World.\n");

    srv_register("FSSRV/test", 1);

    thread_create(&tid, run, NULL, NULL);

    uint msg_id;

    struct srv_called_t srvcalled;

    if (srv_called("FSSRV/test", &srvcalled) == 0)
    {
        char s[] = "hello srv sys.\n";

        char *sss = srvcalled.cache;

        printf("%s", sss);

        srv_reply(srvcalled.sid, s, sizeof(s));

        printf("srv done.\n");

        while(1);
    }
    else
    {
        printf("bug.\n");
        sysctrl_poweroff();
    }

    return 0;
}