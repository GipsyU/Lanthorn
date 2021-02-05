#include <mm.h>
#include <msg.h>
#include <proc.h>
#include <stdio.h>
#include <syscall.h>
#include <type.h>

int main(void)
{
    long pid = 0;

    printf("Hello World.\n");

    fork(&pid);

    if (pid == (long)0xc0400004)
    {
        msgd_t msg = 0;

        char s[10] = "MSG.";

        // msg_newmsg(&msg, s, 5);
    }
    else
    {
        msgboxd_t box = 0;

        // msg_newbox(&box);

        printf("%p.\n", box);
    }

    printf("%p.\n", pid);

    return 0;
}