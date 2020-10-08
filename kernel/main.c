#include <drivers/console.h>
#include <log.h>
int main(void)
{
    console_init();
    assert(0,"asd");
    // memory_init();
    // process_init();
    while(1);
}