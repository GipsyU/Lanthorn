#include <mm.h>
#include <proc.h>
#include <stdio.h>
#include <type.h>

char S[] = "TEST.\n";

void run(void)
{
    printf("test.\n");
    thread_exit();
    printf("test.\n");
    while (1)
        ;
}

int main(void)
{
    long tid = 0;

    printf("Hello World.\n");

    thread_new(&tid, run, NULL, NULL);

    printf("%p.\n", tid);

    return 0;
}