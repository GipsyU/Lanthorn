#include <error.h>
#include <mm.h>
#include <srv.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>

int main(int argc, char *argv[], char *envp[])
{
    syscall(SYS_log, 0);

    return E_OK;
}