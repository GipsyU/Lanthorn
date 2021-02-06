#ifndef _ARCH_CPU_H_
#define _ARCH_CPU_H_
#include <arch/basic.h>
#include <arch/task.h>

uint cpu_id(void);

struct task_t *cpu_get_task(uint cpuid);

void cpu_set_task(uint cpuid, struct task_t *task);

struct task_t *cpu_schd(uint cpuid);

void cpu_pushcli(void);

void cpu_popcli(void);

#endif