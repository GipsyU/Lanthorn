#include <drivers/console.h>
#include <log.h>
int main(void)
{
    console_init();
    info("asd%s\n","asd");
    error("asd%d\n",123);
    warn("asd%d\n",123);
    // memory_init();
    // process_init();
    while(1);
}