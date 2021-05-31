#include <proc.h>
#include <srv.h>
#include <stdio.h>
#include <type.h>

int main(void)
{
    int err = srv_register("keyboard/read", 0);

    err = srv_register("keyboard/write", 0);

    while (1);
}