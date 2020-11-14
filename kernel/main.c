#include <error.h>
#include <basic.h>
#include <log.h>
#include <drivers/console.h>
#include <memory.h>
#include <list.h>
#include <boot_arg.h>
#include <string.h>

static void test(void){

}
int F(int y){
  for(int i=1;i<=10;++i)
    y+=i;
  return y;

}
// void __attribute__((noreturn)) main(struct boot_arg_t boot_arg)
void main(struct boot_arg_t boot_arg)
{
    int err = E_OK;

    int x=4;
    ++x;
    F(x);
    // test();

    info("Hello Lanthorn.\n");
    err = memory_init(boot_arg.mm_list);

    if (err != E_OK) {
        info("init memory failed, err = %s.\n",strerror(err));
    } else {
        info("kernal memory init success.\n");
    }
    // process_init();
    while(1);
}