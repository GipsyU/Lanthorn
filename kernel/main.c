#include <drivers/console.h>
#include <types.h>
#include <error.h>
int main(void){
    const char *s = strerror(E_MAX);
    console_init();
    for (int i = 0; i < 5; ++i) {
        console_put_char(s[i]);
    }
    // memory_init();
    // process_init();
    while(1);
    return 0;
}