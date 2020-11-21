#include <basic.h>
#include <cpu.h>
#include <error.h>

static struct cpu cpus[CONFIG_NR_CPU_MAX];

static uint ncpu = 0;

int cpu_new(struct cpu **cpu)
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

int cpu_get(struct cpu **cpu, uint id)
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