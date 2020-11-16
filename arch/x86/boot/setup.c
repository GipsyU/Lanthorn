#include <log.h>
#include <error.h>
#include <boot_arg.h>
#include "mboot.h"

extern void __attribute__((noreturn)) main(struct boot_arg_t);
extern int mmu_enable_4k_page(void);
extern void console_init(void);
extern u8 kern_start[];
extern u8 kern_end[];

static list_node(struct boot_mm_t) boot_mm_cache[CONFIG_BOOT_MM_NR];

static int boot_mm_num = 0;

static int add_memory(addr_t addr, size_t size)
{
    if (++boot_mm_num >= CONFIG_BOOT_MM_NR)
    {

        warn("no enough boot mm info cache.\n");
        return E_NOCACHE;
    }

    boot_mm_cache[boot_mm_num].data.addr = addr;
    boot_mm_cache[boot_mm_num].data.size = size;

    list_push_back(boot_mm_cache[0], boot_mm_cache[boot_mm_num]);

    return E_OK;
}

static int collect_memory(boot_mm_list_node_t **boot_mm_list_ptr)
{
    int err = E_OK;

    list_init(boot_mm_cache[0]);

    addr_t kstart = (addr_t)kern_start - KERN_BASE;
    addr_t kend = (addr_t)kern_end - KERN_BASE;

    info("kern start physical addr: %p, kern end physical addr: %p.\n", kstart, kend);

    for (struct mmap_entry_t *mmap_entry = (struct mmap_entry_t *)mboot_ptr->mmap_addr;
         mmap_entry < (struct mmap_entry_t *)(mboot_ptr->mmap_addr + mboot_ptr->mmap_length);
         ++mmap_entry)
    {

        if (mmap_entry->type == 1)
        {

            if (kstart >= mmap_entry->base_addr_low &&
                kend <= mmap_entry->base_addr_low + mmap_entry->length_low)
            {

                if (kstart != mmap_entry->base_addr_low)
                {
                    err = add_memory(mmap_entry->base_addr_low, kstart - mmap_entry->base_addr_low);
                }

                if (kend != mmap_entry->base_addr_low + mmap_entry->length_low)
                {
                    err = add_memory(kend, mmap_entry->base_addr_low + mmap_entry->length_low - kend);
                }
            }
            else
            {

                err = add_memory(mmap_entry->base_addr_low, mmap_entry->length_low);
            }
        }
    }

    if (boot_mm_num == 0)
    {
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

    err = collect_memory((boot_mm_list_node_t **)&boot_arg.mm_list);

    if (err != E_OK)
    {
        error("collect memory failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("collect memory success.\n");
    }

    err = mmu_enable_4k_page();

    if (err != E_OK)
    {
        error("enable 4k page failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("enable 4K page success.\n");
    }

    info("finish setup x86 arch.\n");

    main(boot_arg);
}