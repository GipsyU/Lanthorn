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

struct seg_t
{
    u32 lim_15_0 : 16;  // Low bits of segment limit
    u32 base_15_0 : 16; // Low bits of segment base address
    u32 base_23_16 : 8; // Middle bits of segment base address
    u32 type : 4;       // Segment type (see STS_ constants)
    u32 s : 1;          // 0 = system, 1 = application
    u32 dpl : 2;        // Descriptor Privilege Level
    u32 p : 1;          // Present
    u32 lim_19_16 : 4;  // High bits of segment limit
    u32 avl : 1;        // Unused (available for software use)
    u32 rsv1 : 1;       // Reserved
    u32 db : 1;         // 0 = 16-bit segment, 1 = 32-bit segment
    u32 g : 1;          // Granularity: limit scaled by 4K when set
    u32 base_31_24 : 8; // High bits of segment base address
};

struct tss_t
{
    u32 link; // Old ts selector
    u32 esp0; // Stack pointers and segment selectors
    u16 ss0;  //   after an increase in privilege level
    u16 padding1;
    u32 *esp1;
    u16 ss1;
    u16 padding2;
    u32 *esp2;
    u16 ss2;
    u16 padding3;
    void *cr3; // Page directory base
    u32 *eip;  // Saved state from last task switch
    u32 eflags;
    u32 eax; // More saved state (registers)
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 *esp;
    u32 *ebp;
    u32 esi;
    u32 edi;
    u16 es; // Even more saved state (segment selectors)
    u16 padding4;
    u16 cs;
    u16 padding5;
    u16 ss;
    u16 padding6;
    u16 ds;
    u16 padding7;
    u16 fs;
    u16 padding8;
    u16 gs;
    u16 padding9;
    u16 ldt;
    u16 padding10;
    u16 t;    // Trap on task switch
    u16 iomb; // I/O map base address
};


#endif