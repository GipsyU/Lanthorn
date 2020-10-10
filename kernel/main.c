#include <error.h>
#include <log.h>
#include <drivers/console.h>
// #include <memory.h>
#include <list.h>
int main(void)
{
    int err = E_OK;
    console_init();
    info("init console success\n");
    list_node(int) head;
    list_init(head);
    list_node(int) l1;
    l1.data = 1;
    list_node(int) l2;
    l2.data = 2;
    list_node(int) l3;
    l3.data = 3;
    list_append(head, l1);
    list_append(head, l2);
    list_append(head, l3);
    list_rep(head,x){
        info("%d\n",x->data);
    }
    // err = memory_init();
    // info("init memory success\n");
    
    // process_init();
    while(1);
}