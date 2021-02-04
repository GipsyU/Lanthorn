#include <mm.h>
#include <proc.h>
#include <stdio.h>
#include <syscall.h>
#include <type.h>

int main(void)
{
    long pid = 0;

    printf("Hello World.\n");

    fork(&pid);

    syscall(SYS_write, pid);

    printf("%p.\n", pid);

    return 0;
}