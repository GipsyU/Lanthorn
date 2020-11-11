#include <log.h>
#include <error.h>
#include <boot_arg.h>
#include "mboot.h"

extern void __attribute__((noreturn)) main(struct boot_arg_t);
extern int mmu_enable_4k_page(void);
extern void console_init(void);

/*
 * FIXME: if there is two main memory
 * can't collect completely.
 */
static int collect_memory(void)
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
    return E_OK;
}

void __attribute__((noreturn)) setup(void)
{
    int err = E_OK;

    console_init();

    info("start setup x86 arch.\n");

    struct boot_arg_t boot_arg;

    err = collect_memory();
    
    if (err != E_OK) {
        error("collect memory failed, err = %s.\n", strerror(err));
    } else {
        info("collext memory success.\n");
    }

    err = mmu_enable_4k_page();
    
    if (err != E_OK) {
        error("enable 4k page failed, err = %s.\n", strerror(err));
    } else {
        info("enable 4K page success.\n");
    }


    info("finish setup x86 arch.\n");
    
    main(boot_arg);
}