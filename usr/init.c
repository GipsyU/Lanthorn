#include <proc.h>
#include <stdio.h>
#include <type.h>
#include <error.h>

int main(void)
{
    printf("Hello World.\n");

    while(proc_create("/bin/helloworld.elf", NULL) != E_OK);

    printf("proc create done.\n");

    return 0;
}