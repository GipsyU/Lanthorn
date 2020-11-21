#include <x86.h>
#include <basic.h>
#include <error.h>
#include <string.h>

#define STA_X 0x8 // Executable segment
#define STA_W 0x2 // Writeable (non-executable segments)
#define STA_R 0x2 // Readable (executable segments)

static void lgdt(struct seg_t *seg, size_t size)
{
    volatile u16 pd[3];

    pd[0] = size - 1;
    pd[1] = (uint)seg;
    pd[2] = (uint)seg >> 16;

    asm volatile("lgdt (%0)"
                 :
                 : "r"(pd));
}

static void set_seg(struct seg_t *seg, u32 type, u32 base, u32 lim, u32 dpl)
{
    seg->lim_15_0 = (lim >> 12) & 0xffff;
    seg->base_15_0 = base & 0xffff;
    seg->base_23_16 = (base >> 16) & 0xff;
    seg->type = type;
    seg->s = 1;
    seg->dpl = dpl;
    seg->p = 1;
    seg->lim_19_16 = lim >> 28;
    seg->avl = 0;
    seg->rsv1 = 0;
    seg->db = 1;
    seg->g = 1;
    seg->base_31_24 = base >> 24;
}

int gdt_init(struct seg_t *gdt)
{
    int err = E_OK;

    memset(&gdt[0], 0, sizeof(struct seg_t));

    set_seg(&gdt[SEL_KCODE >> 3], STA_X | STA_R, 0, 0xffffffff, DPL_KERN);

    set_seg(&gdt[SEL_KDATA >> 3], STA_W, 0, 0xffffffff, DPL_KERN);

    set_seg(&gdt[SEL_UCODE >> 3], STA_X | STA_R, 0, 0xffffffff, DPL_USER);

    set_seg(&gdt[SEL_UDATA >> 3], STA_W, 0, 0xffffffff, DPL_USER);

    lgdt(gdt, CONFIG_NR_SEG * sizeof(struct seg_t));

    return err;
}