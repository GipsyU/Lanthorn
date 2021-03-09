#include <proc.h>
#include <stdio.h>
#include <type.h>
#include <error.h>

int main(int argc, char *argv[])
{
    printf("Hello Lanthron.\n");

    printf("This is init proc.\n");

    while(proc_create("/bin/shell.elf", NULL) != E_OK);

    printf("create shell success.\n");

    while(1);
}