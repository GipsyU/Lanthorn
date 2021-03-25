#include <arch/basic.h>
#include <arch/cpu.h>
#include <arch/intr.h>
#include <cpu.h>
#include <error.h>
#include <io.h>
#include <log.h>
#include <string.h>
#include <spinlock.h>
#include <thread.h>
#include <x86.h>

#define STS_T32A 0x9 // Available 32-bit TSS
#define STS_IG32 0xE // 32-bit Interrupt Gate
#define STS_TG32 0xF // 32-bit Trap Gate

struct gate_t
{
    u32_t off_15_0 : 16;  // low 16 bits of offset in segment
    u32_t cs : 16;        // code segment selector
    u32_t args : 5;       // # args, 0 for interrupt/trap gates
    u32_t rsv1 : 3;       // reserved(should be zero I guess)
    u32_t type : 4;       // type(STS_{IG32,TG32})
    u32_t s : 1;          // must be 0 (system)
    u32_t dpl : 2;        // descriptor(meaning new) privilege level
    u32_t p : 1;          // Present
    u32_t off_31_16 : 16; // high bits of offset in segment
};

struct intr_regs_t
{
    // registers as pushed by pusha
    u32_t edi;
    u32_t esi;
    u32_t ebp;
    u32_t oesp; // useless & ignored
    u32_t ebx;
    u32_t edx;
    u32_t ecx;
    u32_t eax;

    // rest of trap frame
    u16_t gs;
    u16_t padding1;
    u16_t fs;
    u16_t padding2;
    u16_t es;
    u16_t padding3;
    u16_t ds;
    u16_t padding4;
    u32_t intrno;

    // below here defined by x86 hardware
    u32_t err;
    u32_t eip;
    u16_t cs;
    u16_t padding5;
    u32_t eflags;

    // below here only when crossing rings, such as from user to kernel
    u32_t esp;
    u16_t ss;
    u16_t padding6;
};

struct gate_t idt[NR_INTR];

static int (*intr_table[NR_INTR])(uint);

static int islog[NR_INTR];

static void set_gate(struct gate_t *gate, int istrap, u32_t sel, u32_t off, u32_t dpl)
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
    volatile u16_t pd[3];

    pd[0] = size - 1;

    pd[1] = addr;

    pd[2] = addr >> 16;

    asm volatile("lidt (%0)" : : "r"(pd));
}

extern void lapic_eoi(void);

void intr_hdl(volatile struct intr_regs_t *regs)
{
    if (regs->intrno != INTR_SYSCALL)
    {
        assert(intr_irq_state() == 0);

        cpu_get_task(cpu_id())->ncli++;
    }

    assert (regs->intrno < NR_INTR && regs->intrno > 0)

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
        // print("eip:%p cs:%p ds:%p err:%p\n", regs->eip, regs->cs , regs->ds, regs->err);

        panic("no intr routine err: %d, eip: %p\n", regs->err, regs->eip);
    }

    if (islog[regs->intrno])
    {
        info("end intrno: %d\n", regs->intrno);
    }

    if (regs->intrno != INTR_SYSCALL)
    {
        assert(intr_irq_state() == 0);

        cpu_get_task(cpu_id())->ncli--;
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

int intr_lpinit(void)
{
    lidt((addr_t)idt, sizeof(idt));

    return E_OK;
}

void intr_end(void)
{
    lapic_eoi();

    intr_irq_restore();
}

extern void intr_ret(void);

addr_t intr_user_init(addr_t ksp, addr_t run, addr_t usp, addr_t ubp, addr_t arga, addr_t argsz)
{
    assert(argsz % sizeof(addr_t) == 0);

    struct intr_regs_t *regs = (void *)(ksp - sizeof(struct intr_regs_t));

    regs->cs = SEL_UCODE;

    regs->ds = regs->es = regs->fs = regs->gs = regs->ss = SEL_UDATA;

    regs->eip = run;

    regs->eflags = FL_IF;

    addr_t *arg = usp - argsz;

    memcpy(arg, arga, argsz);

    regs->esp = usp - argsz - sizeof(addr_t);

    regs->ebp = ubp;

    addr_t *iret = ((void *)regs) - sizeof(addr_t);

    *iret = intr_ret;

    return (addr_t)iret;
}

int intr_irq_state(void)
{
    return (read_eflags()&FL_IF) > 0;
}

void intr_irq_enable(void)
{
    barrier();

    asm volatile("sti" : : : "memory");

    barrier();
}

void intr_irq_disable(void)
{
    barrier();

    asm volatile("cli" : : : "memory");

    barrier();
}

void intr_irq_restore(void)
{
    assert(cpu_get_task(cpu_id())->ncli > 0);

    cpu_get_task(cpu_id())->ncli--;

    // info_early("p%p\n", cpu_get_task(cpu_id())->ncli);
    
    if (cpu_get_task(cpu_id())->ncli == 0) intr_irq_enable();
}

void intr_irq_save(void)
{
    intr_irq_disable();

    cpu_get_task(cpu_id())->ncli++;

    // info_early("p%p\n", cpu_get_task(cpu_id())->ncli);
}