#include <error.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 0) return E_INVAL;

    printf("\x1b[2J\x1b[H");

    return E_OK;
}