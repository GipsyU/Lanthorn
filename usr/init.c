#include <type.h>
#include <mm.h>
int main(void)
{
    addr_t addr;
    int err = malloc(&addr, 0x1000);
    return 0;
}