#ifndef _X86_SRC_CPU_H_
#define _X86_SRC_CPU_H_
#include <arch/basic.h>
#include <arch/task.h>
#include <x86.h>

#define cpu_relax() asm volatile("rep; nop")

struct cpu_t
{
    uint cpuid;
    uint apicid; // Local APIC ID
    struct task_t *task;
    struct task_t schd;
    struct tss_t tss;                // Used by x86 to find stack for interrupt
    struct seg_t gdt[CONFIG_NR_SEG]; // x86 global descriptor table
    // volatile uint started;     // Has the CPU started?
    // int ncli;                  // Depth of pushcli nesting.
    // int intena;                // Were interrupts enabled before pushcli?
    // struct proc *proc;         // The process running on this cpu or null
};

int cpu_new(struct cpu_t **cpu);

int cpu_get(struct cpu_t **cpu, uint id);

#endif