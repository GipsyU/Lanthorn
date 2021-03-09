#include <arch/basic.h>
#include <arch/cpu.h>
#include <arch/intr.h>
#include <cpu.h>
#include <error.h>
#include <log.h>
#include <io.h>

#define STS_T32A 0x9 // Available 32-bit TSS
#define STS_IG32 0xE // 32-bit Interrupt Gate
#define STS_TG32 0xF // 32-bit Trap Gate
#define FL_IF 0x00000200

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

static int (*intr_table[NR_INTR])(uint);

static int islog[NR_INTR];

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

static int lidt(addr_t addr, size_t size)
{
    volatile u16 pd[3];

    pd[0] = size - 1;

    pd[1] = addr;

    pd[2] = addr >> 16;

    asm volatile("lidt (%0)" : : "r"(pd));
}

extern void lapic_eoi(void);

void intr_hdl(struct intr_regs_t *regs)
{
    if (islog[regs->intrno])
    {
        info("intrno: %d\n", regs->intrno);
    }

    int (*handler)(uint) = intr_table[regs->intrno];

    if (handler != NULL)
    {
        if (regs->intrno == INTR_SYSCALL)

            regs->eax = handler(regs->esp + 4);

        else if (regs->intrno == INTR_PGFAULT)

            handler(regs->err);

        else

            handler(NULL);
    }
    else
    {
        panic("no intr routine");
    }

    if (islog[regs->intrno])
    {
        info("end intrno: %d\n", regs->intrno);
    }

    lapic_eoi();

    return;
}

extern int ioapic_enable(uint intr, int cpuid);

int intr_register(int intrno, int (*hdl)(uint errno))
{
    int err = E_OK;

    intr_table[intrno] = hdl;

    if (intrno > 32 && intrno < 48) ioapic_enable(intrno - 32, 0);

    return err;
}

int intr_unregister(int intrno)
{
    intr_table[intrno] = NULL;

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

    set_gate(&idt[INTR_SYSCALL], 1, SEL_KCODE, intrx[INTR_SYSCALL], DPL_USER);

    lidt((addr_t)idt, sizeof(idt));

    for (int i = 0; i < NR_INTR; ++i)
    {
        intr_table[i] = NULL;

        islog[i] = 1;
    }

    islog[INTR_TIMER] = 0;

    islog[INTR_COM1] = 0;

    islog[INTR_SYSCALL] = 0;

    return err;
}

void intr_end(void)
{
    lapic_eoi();

    asm volatile("sti");
}
extern void intr_ret(void);

addr_t intr_user_init(addr_t ksp, addr_t run, addr_t usp, addr_t ubp)
{
    struct intr_regs_t *regs = (void *)(ksp - sizeof(struct intr_regs_t));

    regs->cs = SEL_UCODE;

    regs->ds = regs->es = regs->fs = regs->gs = regs->ss = SEL_UDATA;

    regs->eip = run;

    regs->eflags = FL_IF;

    regs->esp = usp;

    regs->ebp = ubp;

    addr_t *iret = ((void *)regs) - sizeof(addr_t);

    *iret = intr_ret;

    return (addr_t)iret;
}

void intr_irq_enable(void)
{
    asm volatile("sti" : : : "memory");
}

void intr_irq_disable(void)
{
    asm volatile("cli" : : : "memory");
}

void intr_irq_restore(void)
{
    cpu_get_task(cpu_id())->ncli--;

    if (cpu_get_task(cpu_id())->ncli == 0) intr_irq_enable();
}

void intr_irq_save(void)
{
    intr_irq_disable();

    cpu_get_task(cpu_id())->ncli++;
}