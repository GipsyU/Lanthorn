#include <mm.h>
#include <type.h>
int main(void)
{
    addr_t addr;

    int err = malloc(&addr, 0x1000);

    char *s = addr;

    s[0] = 'a';
    s[1] = 'b';

    return 0;
}