#ifndef _X86_SRC_CPU_H_
#define _X86_SRC_CPU_H_
#include <basic.h>

#define cpu_relax() asm volatile("rep; nop")

struct cpu
{
    u8 cpuid;
    u8 apicid; // Local APIC ID
    // struct context *scheduler; // swtch() here to enter scheduler
    // struct taskstate ts;       // Used by x86 to find stack for interrupt
    // struct segdesc gdt[NSEGS]; // x86 global descriptor table
    // volatile uint started;     // Has the CPU started?
    // int ncli;                  // Depth of pushcli nesting.
    // int intena;                // Were interrupts enabled before pushcli?
    // struct proc *proc;         // The process running on this cpu or null
};

#endif