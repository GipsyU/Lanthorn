#include <drivers/console.h>
#include <types.h>
#include <error.h>
int main(void){
    console_init();
    // memory_init();
    // process_init();
    while(1);
}