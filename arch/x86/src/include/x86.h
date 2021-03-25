#ifndef _X86_X86_H_

#define _X86_X86_H_

#include <arch/basic.h>

#define DPL_KERN 0

#define DPL_USER 3

#define SEL_KCODE ((1 << 3) | DPL_KERN)

#define SEL_KDATA ((2 << 3) | DPL_KERN)

#define SEL_UCODE ((3 << 3) | DPL_USER)

#define SEL_UDATA ((4 << 3) | DPL_USER)

#define SEL_TSS (5 << 3)

#define FL_IF 0x00000200

struct seg_t
{
    u32_t lim_15_0 : 16;  // Low bits of segment limit
    u32_t base_15_0 : 16; // Low bits of segment base address
    u32_t base_23_16 : 8; // Middle bits of segment base address
    u32_t type : 4;       // Segment type (see STS_ constants)
    u32_t s : 1;          // 0 = system, 1 = application
    u32_t dpl : 2;        // Descriptor Privilege Level
    u32_t p : 1;          // Present
    u32_t lim_19_16 : 4;  // High bits of segment limit
    u32_t avl : 1;        // Unused (available for software use)
    u32_t rsv1 : 1;       // Reserved
    u32_t db : 1;         // 0 = 16-bit segment, 1 = 32-bit segment
    u32_t g : 1;          // Granularity: limit scaled by 4K when set
    u32_t base_31_24 : 8; // High bits of segment base address
};

struct tss_t
{
    u32_t link; // Old ts selector
    u32_t esp0; // Stack pointers and segment selectors
    u16_t ss0;  //   after an increase in privilege level
    u16_t padding1;
    u32_t *esp1;
    u16_t ss1;
    u16_t padding2;
    u32_t *esp2;
    u16_t ss2;
    u16_t padding3;
    void *cr3; // Page directory base
    u32_t *eip;  // Saved state from last task switch
    u32_t eflags;
    u32_t eax; // More saved state (registers)
    u32_t ecx;
    u32_t edx;
    u32_t ebx;
    u32_t *esp;
    u32_t *ebp;
    u32_t esi;
    u32_t edi;
    u16_t es; // Even more saved state (segment selectors)
    u16_t padding4;
    u16_t cs;
    u16_t padding5;
    u16_t ss;
    u16_t padding6;
    u16_t ds;
    u16_t padding7;
    u16_t fs;
    u16_t padding8;
    u16_t gs;
    u16_t padding9;
    u16_t ldt;
    u16_t padding10;
    u16_t t;    // Trap on task switch
    u16_t iomb; // I/O map base address
};

static inline uint read_eflags(void)
{
    uint eflags;
    asm volatile("pushfl; popl %0" : "=r" (eflags));
    return eflags;
}

#endif