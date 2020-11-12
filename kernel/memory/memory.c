#include <memory.h>
#include <boot_arg.h>
#include <error.h>
#include <log.h>

extern u8 kern_start[];
extern u8 kern_end[];

static int print_memory_info(boot_mm_list_node_t *mm_list_node)
{
    int err = E_OK;

    info("available memory:\n");

    list_rep (*mm_list_node, p) {
        info("memory start addr: %p, end addr: %p.\n", p->data.addr, p->data.addr + p->data.size - 1);
    }

    info("kern start physical addr: %p, kern end physical addr: %p.\n", kern_start - KERN_BASE, kern_end - KERN_BASE);

    return err;
}

int memory_init(boot_mm_list_node_t *mm_list_node)
{
    int err = E_OK;
 
    err = print_memory_info(mm_list_node);
 
    return err;
}