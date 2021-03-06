#include "mboot.h"
#include <arch/cpu.h>
#include <boot_arg.h>
#include <cpu.h>
#include <elf.h>
#include <error.h>
#include <io.h>
#include <log.h>
#include <mp.h>
#include <string.h>

extern void __attribute__((noreturn)) main(struct boot_arg_t);
extern int uart_init(void);
extern int mmu_init(addr_t *pde);
extern int intr_init(void);
extern int intr_lpinit(void);
extern u8_t kern_start[];
extern u8_t kern_end[];
extern int mp_init(int *num_cpu);
extern int lapic_init(void);
extern int task_init(struct task_t *task);

extern int gdt_init(struct seg_t *gdt, struct tss_t *tss);

extern int ioapic_init(void);

static struct boot_arg_t boot_arg;

static int add_memory(addr_t addr, size_t size)
{
    boot_arg.free_pmm_start = addr;

    boot_arg.free_pmm_size = size;

    return E_OK;
}

static int collect_mm_info_early(void)
{
    int err = E_OK;

    boot_arg.kern_start = (addr_t)kern_start;

    boot_arg.kern_end = (addr_t)kern_end;

    addr_t kstart = (addr_t)kern_start - KERN_BASE;

    addr_t kend = (addr_t)kern_end - KERN_BASE;

    info_early("kern start physical addr: %p, kern end physical addr: %p.\n", kstart, kend);

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

    err = task_init(cpu_schd(cpuid));

    if (err != E_OK)
    {
        error("cpu%d init task failed, err = %s.\n", cpuid, strerror(err));
    }
    else
    {
        // info_early("cpu%d init task success.\n", cpuid);
    }

    cpu_set_task(cpuid, cpu_schd(cpuid));

    err = gdt_init(cpu->gdt, &cpu->tss);

    if (err != E_OK)
    {
        error("cpu%d init gdt failed, err = %s.\n", cpuid, strerror(err));
    }
    else
    {
        // info_early("cpu%d init gdt success.\n", cpuid);
    }

    err = intr_lpinit();

    if (err != E_OK)
    {
        error("cpu%d init intr lp failed, err = %s.\n", cpuid, strerror(err));
    }
    else
    {
        // info_early("cpu%d init intr lp success.\n", cpuid);
    }

    err = lapic_init();

    if (err != E_OK)
    {
        error("cpu%d init lapic failed, err = %s.\n", cpuid, strerror(err));
    }
    else
    {
        // info_early("cpu%d init lapic success.\n", cpuid);
    }

    return err;
}

extern char _binary_arch_x86_boot__apboot_o_start[];
extern char _binary_arch_x86_boot__apboot_o_size[];

void __attribute__((noreturn)) setup(void)
{

    int err = E_OK;

    uart_init();

    info_early("start setup x86 arch.\n");

    err = mmu_init(&boot_arg.pde);

    if (err != E_OK)
    {
        error("init mmu failed, err = %s.\n", strerror(err));
    }
    else
    {
        info_early("init mmu success.\n");
    }

    err = mp_init(&boot_arg.ncpu);

    if (err != E_OK)
    {
        error("init mp failed, err = %s.\n", strerror(err));
    }
    else
    {
        info_early("init mp success.\n");
    }

    err = intr_init();

    if (err != E_OK)
    {
        error("init intr failed, err = %s.\n", strerror(err));
    }
    else
    {
        info_early("init intr success.\n");
    }

    err = ioapic_init();

    if (err != E_OK)
    {
        error("init ioapic failed, err = %s.\n", strerror(err));
    }
    else
    {
        info_early("init ioapic success.\n");
    }

    err = setup_lp();

    if (err != E_OK)
    {
        error("setup local processor failed, err = %s.\n", strerror(err));
    }
    else
    {
        info_early("setup cpu%d success.\n", cpu_id());
    }

    err = collect_mm_info_early();

    if (err != E_OK)
    {
        error("collect mm info_early failed, err = %s.\n", strerror(err));
    }
    else
    {
        info_early("collect mm info_early success.\n");
    }

    memcpy(0x7000 + KERN_BASE, _binary_arch_x86_boot__apboot_o_start, _binary_arch_x86_boot__apboot_o_size);

    info_early("finish setup x86 arch.\n");

    main(boot_arg);
}

extern addr_t PDE[];

int cpu_startap(int cpuid, addr_t routine, addr_t stka, size_t stksz)
{
    if (cpuid >= boot_arg.ncpu) return E_INVAL;

    volatile int finished = 0;

    addr_t *param = 0x7000 + KERN_BASE - 5 * sizeof(addr_t);

    param[0] = setup_lp;

    param[1] = stka + stksz;

    param[2] = routine;

    param[3] = (addr_t)PDE - KERN_BASE;

    param[4] = &finished;

    lapic_startap(cpuid, 0x7000);

    while(finished == 0);

    return E_OK;
}