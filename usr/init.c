#include <proc.h>
#include <stdio.h>
#include <type.h>
#include <error.h>

int main(void)
{
    printf("Hello World.\n");

    while(proc_create("/bin/shell.elf", NULL) != E_OK);

    printf("create shell success.\n");

    while(1);
}