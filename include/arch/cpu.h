#ifndef _ARCH_CPU_H_
#define _ARCH_CPU_H_
#include <arch/basic.h>
#include <arch/task.h>

uint cpu_id(void);

int cpu_now(void);

struct task_t *cpu_get_task(uint cpuid);

void cpu_set_task(uint cpuid, struct task_t *task);

struct task_t *cpu_schd(uint cpuid);

int cpu_startap(int cpuid, addr_t routine, addr_t stka, size_t stksz);

#define cpu_relax() asm volatile("rep;nop;");

#define barrier()                     \
    {                                 \
        asm volatile(""               \
                     :                \
                     :                \
                     : "memory");     \
        asm volatile("mfence" ::      \
                         : "memory"); \
        __sync_synchronize();         \
    }

#endif