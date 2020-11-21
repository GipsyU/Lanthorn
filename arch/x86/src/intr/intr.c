#include <arch/intr.h>
#include <error.h>
#include <basic.h>
#include <log.h>
#include <cpu.h>

#define STS_T32A 0x9 // Available 32-bit TSS
#define STS_IG32 0xE // 32-bit Interrupt Gate
#define STS_TG32 0xF // 32-bit Trap Gate

struct gate_t
{
    u32 off_15_0 : 16;  // low 16 bits of offset in segment
    u32 cs : 16;        // code segment selector
    u32 args : 5;       // # args, 0 for interrupt/trap gates
    u32 rsv1 : 3;       // reserved(should be zero I guess)
    u32 type : 4;       // type(STS_{IG32,TG32})
    u32 s : 1;          // must be 0 (system)
    u32 dpl : 2;        // descriptor(meaning new) privilege level
    u32 p : 1;          // Present
    u32 off_31_16 : 16; // high bits of offset in segment
};

#define SEG_KCODE 8

struct intr_regs_t
{
    // registers as pushed by pusha
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 oesp; // useless & ignored
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;

    // rest of trap frame
    u16 gs;
    u16 padding1;
    u16 fs;
    u16 padding2;
    u16 es;
    u16 padding3;
    u16 ds;
    u16 padding4;
    u32 intrno;

    // below here defined by x86 hardware
    u32 err;
    u32 eip;
    u16 cs;
    u16 padding5;
    u32 eflags;

    // below here only when crossing rings, such as from user to kernel
    u32 esp;
    u16 ss;
    u16 padding6;
};

struct gate_t idt[NR_INTR];

static void set_gate(struct gate_t *gate, int istrap, u32 sel, u32 off, u32 dpl)
{
    gate->off_15_0 = off & 0xffff;

    gate->cs = sel;

    gate->args = 0;

    gate->rsv1 = 0;

    gate->type = istrap ? STS_TG32 : STS_IG32;

    gate->s = 0;

    gate->dpl = dpl;

    gate->p = 1;

    gate->off_31_16 = off >> 16;
}

static int inline lidt(addr_t addr, size_t size)
{
    volatile u16 pd[3];

    pd[0] = size - 1;

    pd[1] = addr;

    pd[2] = addr >> 16;

    asm volatile("lidt (%0)"
                 :
                 : "r"(pd));
}

void intr_hdl(struct intr_regs_t *regs)
{
    info("intrno: %d\n", regs->intrno);

    return;
}

int intr_register(int cpuid, int intrid, void *hdl)
{
    return E_OK;
}

int intr_unregister(int cpuid, int intrid)
{
    return E_OK;
}

extern addr_t intrx[];

int intr_init(void)
{   
    int err = E_OK;

    for (int i = 0; i < NR_INTR; ++i)
    {
        set_gate(&idt[i], 0, SEL_KCODE, intrx[i], 0);
    }

    set_gate(&idt[128], 1, SEL_KCODE, intrx[128], 0);

    lidt(idt, sizeof(idt));

    return err;
}