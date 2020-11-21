#ifndef _X86_X86_H_

#define _X86_X86_H_

#include <basic.h>

#define SEL_KCODE (1 << 3)

#define SEL_KDATA (2 << 3)

#define SEL_UCODE (3 << 3)

#define SEL_UDATA (4 << 3)

#define DPL_KERN 0

#define DPL_USER 3

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


#endif