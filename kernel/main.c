#include <drivers/console.h>
#include <types.h>
int main(void){
    console_init();
    console_put_char('h');
    console_put_char('e');
    console_put_char('l');
    console_put_char('l');
    console_put_char('o');
    console_put_char(' ');
    console_put_char('l');
    console_put_char('a');
    console_put_char('n');
    console_put_char('t');
    console_put_char('h');
    console_put_char('o');
    console_put_char('r');
    console_put_char('n');
    console_put_char('!');
    console_put_char('\n');
    // memory_init();
    // process_init();
    while(1);
    return 0;
}