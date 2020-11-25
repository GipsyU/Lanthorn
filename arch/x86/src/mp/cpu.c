#include <basic.h>
#include <cpu.h>
#include <error.h>
#include <intr.h>
#include <log.h>
#include <io.h>
#include <arch/sysctrl.h>

static struct cpu_t cpus[CONFIG_NR_CPU_MAX];

static uint ncpu = 0;

int cpu_new(struct cpu_t **cpu)
{
    int err = E_OK;
    
    if (cpu == NULL)
    {
        return E_INVAL;
    }

    if (ncpu >= CONFIG_NR_CPU_MAX)
    {
        return E_NOSLOT;
    }

    cpus[ncpu].cpuid = ncpu;

    *cpu = &cpus[ncpu];

    ++ncpu;

    return err;
}

int cpu_get(struct cpu_t **cpu, uint id)
{
    int err = E_OK;

    if (cpu == NULL)
    {
        return E_INVAL;
    }
    
    if (id >= ncpu)
    {
        return E_NOTFOUND;
    }

    *cpu = &cpus[id];

    return err;
}

uint cpu_id(void)
{
    uint lapicid = lapic_id();

    for (uint cpuid = 0; cpuid < ncpu; ++cpuid)
    {
        if (lapicid == cpus[cpuid].apicid)
        {
            return cpuid;
        }
    }
    
    panic("cpu bug.\n");
}

struct task_t *cpu_get_task(uint cpuid)
{
    return cpus[cpuid].task;
}

void cpu_set_task(uint cpuid, struct task_t *task)
{
    cpus[cpuid].task = task;
}

struct task_t *cpu_schd(uint cpuid)
{
    return &cpus[cpuid].schd;
}

void sysctrl_shutdown(void)
{
    outw(0x2000,0x604);
}