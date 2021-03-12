#include <error.h>
#include <mm.h>
#include <srv.h>
#include <string.h>
#include <type.h>
#include <stdio.h>

int cmd_pwd(char *parse[], char *pwd)
{
    if (parse[1] != NULL) return E_INVAL;

    printf("%s\n", pwd);

    return E_OK;
}