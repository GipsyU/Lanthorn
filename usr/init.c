#include <mm.h>
#include <proc.h>
#include <syscall.h>
#include <type.h>

int main(void)
{
    long pid = 0;

    fork(&pid);

    syscall(SYS_write, pid);

    return 0;
}