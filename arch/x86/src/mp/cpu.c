#include <arch/basic.h>
#include <cpu.h>
#include <error.h>
#include <intr.h>
#include <log.h>
#include <io.h>
#include <arch/sysctrl.h>
#include <spinlock.h>

static volatile struct cpu_t cpus[CONFIG_NR_CPU_MAX];

static uint ncpu = 0;

int cpu_new(struct cpu_t **cpu)
{
    int err = E_OK;

    if (ncpu >= CONFIG_NR_CPU_MAX) return E_NOSLOT;

    cpus[ncpu].cpuid = ncpu;

    *cpu = &cpus[ncpu];

    ++ncpu;

    return err;
}

int cpu_get(struct cpu_t **cpu, uint id)
{
    int err = E_OK;
    
    if (id >= ncpu) return E_NOTFOUND;

    *cpu = &cpus[id];

    return err;
}

uint cpu_id(void)
{
    assert(intr_irq_state() == 0);

    uint lapicid = lapic_id();

    for (uint cpuid = 0; cpuid < ncpu; ++cpuid)
    {
        if (lapicid == cpus[cpuid].apicid)
        {
            return cpuid;
        }
    }
}

int cpu_now(void)
{
    intr_irq_save();

    int cpuid = cpu_id();

    intr_irq_restore();

    return cpuid;
}

struct task_t *cpu_get_task(uint cpuid)
{
    return cpus[cpuid].task;
}

extern void lgdt(struct seg_t *seg, size_t size);

extern void ltr(u32_t sel);

void cpu_set_task(uint cpuid, struct task_t *task)
{
    cpus[cpuid].task = task;

    cpus[cpuid].tss.ss0 = SEL_KDATA;

    cpus[cpuid].tss.esp0 = task->saddr + task->ssize;

    cpus[cpuid].tss.iomb = 0xffff;
}

struct task_t *cpu_schd(uint cpuid)
{
    return &cpus[cpuid].schd;
}

void sysctrl_shutdown(void)
{
    outw(0x2000,0x604);
}