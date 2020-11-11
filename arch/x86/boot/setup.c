#include <boot_arg.h>
#include <log.h>
#include "mboot.h"

extern void __attribute__((noreturn)) main(struct boot_arg_t);


static void collect_memory(void)
{
    struct mmap_entry_t *mmap_start_addr = (struct mmap_entry_t *)mboot_ptr->mmap_addr;
    struct mmap_entry_t *mmap_end_addr = (struct mmap_entry_t *)mboot_ptr->mmap_addr+mboot_ptr->mmap_length;
    
    struct mmap_entry_t *map_entry;
    
    for (map_entry = mmap_start_addr; map_entry < mmap_end_addr; map_entry++) {
        if(map_entry->type==1){
            info("%p %p\n",map_entry->base_addr_high,map_entry->base_addr_low);
            info("%p %p\n",map_entry->length_high,map_entry->length_low);
            info("%p %p\n",map_entry->size,map_entry->type);
        }
    }
}

void __attribute__((noreturn)) setup(void)
{
    struct boot_arg_t boot_arg;

    collect_memory();

    main(boot_arg);
}


