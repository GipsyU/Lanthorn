#include <log.h>
#include <error.h>
#include <boot_arg.h>
#include <io.h>
#include <cpu.h>
#include "mboot.h"
#include <slot.h>
#include <arch/cpu.h>

extern void __attribute__((noreturn)) main(struct boot_arg_t);
extern void console_init(void);
extern int mmu_init(void);
extern int intr_init(void);
extern u8 kern_start[];
extern u8 kern_end[];
extern int mp_init(int *num_cpu);
extern int lapic_init(void);
extern int gdt_init(struct seg_t *gdt);

static struct boot_arg_t boot_arg;

static int add_memory(addr_t addr, size_t size)
{
    boot_arg.free_pmm_start = addr;

    boot_arg.free_pmm_size = size;
    
    return E_OK;
}

static int collect_mm_info(void)
{
    int err = E_OK;

    boot_arg.kern_start = (addr_t)kern_start;

    boot_arg.kern_end = (addr_t)kern_end;

    addr_t kstart = (addr_t)kern_start - KERN_BASE;
    
    addr_t kend = (addr_t)kern_end - KERN_BASE;

    info("kern start physical addr: %p, kern end physical addr: %p.\n", kstart, kend);

    /**
     * FIXME:1024
     */

    #define DEV_BASE 0xFE000000

    boot_arg.free_kvm_start = KERN_BASE + CONFIG_NR_BOOT_PTE * (1024) * PAGE_SIZE;

    boot_arg.free_kvm_size = DEV_BASE - boot_arg.free_kvm_start;

    for (struct mmap_entry_t *mmap_entry = (struct mmap_entry_t *)(mboot_ptr->mmap_addr + KERN_BASE);
         mmap_entry < (struct mmap_entry_t *)(mboot_ptr->mmap_addr + mboot_ptr->mmap_length + KERN_BASE);
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
                if (mmap_entry->base_addr_low != 0)
                {
                    err = add_memory(mmap_entry->base_addr_low, mmap_entry->length_low);
                }
            }
        }
    }

    return err;
}

static int setup_lp(void)
{
    int err = E_OK;

    uint cpuid = cpu_id();

    struct cpu_t *cpu;

    cpu_get(&cpu, cpuid);
    
    err = gdt_init(cpu->gdt);

    if (err != E_OK)
    {
        error("cpu%d init gdt failed, err = %s.\n", cpuid, strerror(err));
    }
    else
    {
        info("cpu%d init gdt success.\n", cpuid);
    }

    err = lapic_init();

    if (err != E_OK)
    {
        error("cpu%d init lapic failed, err = %s.\n", cpuid, strerror(err));
    }
    else
    {
        info("cpu%d init lapic success.\n", cpuid);
    }

    return err;
}

void __attribute__((noreturn)) setup(void)
{
    int err = E_OK;

    console_init();

    info("start setup x86 arch.\n");

    err = mmu_init();

    if (err != E_OK)
    {
        error("init mmu failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("init mmu success.\n");
    }

    err = mp_init(&boot_arg.ncpu);

    if (err != E_OK)
    {
        error("init mp failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("init mp success.\n");
    }

    err = intr_init();
    
    if (err != E_OK)
    {
        error("init intr failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("init intr success.\n");
    }

    err = setup_lp();

    if (err != E_OK)
    {
        error("setup local processor failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("setup cpu%d success.\n", cpu_id());
    }

    err = collect_mm_info();

    if (err != E_OK)
    {
        error("collect mm info failed, err = %s.\n", strerror(err));
    }
    else
    {
        info("collect mm info success.\n");
    }
    
    info("finish setup x86 arch.\n");

    main(boot_arg);
}
