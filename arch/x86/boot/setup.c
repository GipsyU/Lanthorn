#include <log.h>
#include <error.h>
#include <boot_arg.h>
#include "mboot.h"

extern void __attribute__((noreturn)) main(struct boot_arg_t);
extern int mmu_enable_4k_page(void);
extern void console_init(void);

static list_node(struct boot_mm_t) boot_mm_cache[CONFIG_BOOT_MM_NR]; 

static int boot_mm_num = 0;

static int collect_memory(boot_mm_list_node_t **boot_mm_list_ptr)
{
    int err = E_OK;

    list_init(boot_mm_cache[0]);

    for (struct mmap_entry_t *mmap_entry = (struct mmap_entry_t *)mboot_ptr->mmap_addr;
         mmap_entry < (struct mmap_entry_t *)(mboot_ptr->mmap_addr + mboot_ptr->mmap_length);
         ++mmap_entry) {
        
        if (mmap_entry->type == 1) {

            if (++boot_mm_num >= CONFIG_BOOT_MM_NR) {

                warn("no spare boot mm info cache.\n");
                err = E_NOCACHE;

                break;

            }
            
            boot_mm_cache[boot_mm_num].data.addr = mmap_entry->base_addr_low;
            boot_mm_cache[boot_mm_num].data.size = mmap_entry->length_low;

            list_append(boot_mm_cache[0], boot_mm_cache[boot_mm_num]);

        }
        
    }

    if (boot_mm_num == 0) {
        error("no memory can be collected.\n");
        err = E_NOMEM;
    }

    *boot_mm_list_ptr = (boot_mm_list_node_t *)boot_mm_cache;

    return err;
}


void __attribute__((noreturn)) setup(void)
{
    int err = E_OK;

    console_init();

    info("start setup x86 arch.\n");

    struct boot_arg_t boot_arg;

    err = collect_memory((boot_mm_list_node_t **) &boot_arg.mm_list);
    
    if (err != E_OK) {
        error("collect memory failed, err = %s.\n", strerror(err));
    } else {
        info("collect memory success.\n");
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