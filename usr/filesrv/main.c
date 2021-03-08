#include <srv.h>
#include <stdio.h>
#include <log.h>
#include <rbt.h>
#include <error.h>
#include <string.h>
#include <mm.h>
#include <utils.h>
#include "file.h"

int main(void)
{
    init();

    srv_read();

    return 0;
}