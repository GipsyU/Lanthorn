#include <mm.h>
#include <msg.h>
#include <proc.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <sysctrl.h>
#include <type.h>

char S[] = "TEST.\n";

int main(void)
{
    long pid = 0;

    printf("Hello World.\n");

    uint boxid;

    int err = msg_newbox(&boxid);

    fork(&pid);

    if (pid == (long)0xc0400004)
    {
        char s[10] = "INIT MSG.\n";

        uint msgid;

        err = msg_newmsg(&msgid, s, 11);

        msg_send(boxid, msgid);
    }
    else
    {
        while (1)
        {
            uint id;

            if (msg_recieve(boxid, &id) == 0)
            {
                printf("%d.\n", id);

                char s[20] = "FORK MSG\n";

                msg_read(id, s, 100);

                printf("%s", s);
            }
        }
    }

    printf("%p.\n", pid);

    sysctrl_poweroff();

    return 0;
}