#include <drivers/intr.h>
#include <error.h>
#include <basic.h>
#include <x86.h>

#define STS_T32A    0x9     // Available 32-bit TSS
#define STS_IG32    0xE     // 32-bit Interrupt Gate
#define STS_TG32    0xF     // 32-bit Trap Gate

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

int intr_register(int cpuid, int intrid, void *hdl)
{

}

int intr_unregister(int cpuid, int intrid)
{

}

int intr_init()
{
    int err = E_OK;

    for (int i = 0; i <  NR_GATE; ++i)
    {

        set_gate(&idt[i], 0, SEG_KCODE, ,)
    }

    return err;
}