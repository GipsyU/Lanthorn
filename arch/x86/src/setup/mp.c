#include <arch/basic.h>
#include <error.h>
#include <string.h>
#include <log.h>
#include <cpu.h>

struct mp
{                    // floating pointer
    u8 signature[4]; // "_MP_"
    addr_t physaddr; // phys addr of MP config table
    u8 length;       // 1
    u8 specrev;      // [14]
    u8 checksum;     // all bytes must add up to 0
    u8 type;         // MP system config type
    u8 imcrp;
    u8 reserved[3];
};

struct mpconf
{                    // configuration table header
    u8 signature[4]; // "PCMP"
    u16 length;      // total table length
    u8 version;      // [14]
    u8 checksum;     // all bytes must add up to 0
    u8 product[20];  // product id
    uint *oemtable;  // OEM table pointer
    u16 oemlength;   // OEM table length
    u16 entry;       // entry count
    uint *lapicaddr; // address of local APIC
    u16 xlength;     // extended table length
    u8 xchecksum;    // extended table checksum
    u8 reserved;
};

struct mpproc
{                    // processor table entry
    u8 type;         // entry type (0)
    u8 apicid;       // local APIC id
    u8 version;      // local APIC verison
    u8 flags;        // CPU flags
#define MPBOOT 0x02  // This proc is the bootstrap processor.
    u8 signature[4]; // CPU signature
    uint feature;    // feature flags from CPUID instruction
    u8 reserved[8];
};

struct mpioapic
{               // I/O APIC table entry
    u8 type;    // entry type (2)
    u8 apicno;  // I/O APIC id
    u8 version; // I/O APIC version
    u8 flags;   // I/O APIC flags
    uint *addr; // I/O APIC address
};

// Table entry types
#define MPPROC 0x00   // One per processor
#define MPBUS 0x01    // One per bus
#define MPIOAPIC 0x02 // One per I/O APIC
#define MPIOINTR 0x03 // One per bus interrupt source
#define MPLINTR 0x04  // One per system interrupt source

extern struct cpu_t cpus[];

int ncpu = 0;

static u8 sum(u8 *addr, size_t len)
{
    u8 sum = 0;

    for (int i = 0; i < len; i++)
    {
        sum += addr[i];
    }

    return sum;
}

// Look for an MP structure in the len bytes at addr.
static int _mpsearch(addr_t addr, size_t size, struct mp **mp)
{
    if (mp == NULL)
    {
        return E_INVAL;
    }

    addr += KERN_BASE;

    for (*mp = (struct mp *)addr; *mp < (struct mp *)(addr + size); (*mp)++)
    {
        if (memcmp((u8 *)(*mp), "_MP_", 4) == 0 && sum((u8 *)(*mp), sizeof(struct mp)) == 0 && (*mp)->physaddr != NULL)
        {
            return E_OK;
        }
    }

    return E_NOTFOUND;
}

// Search for the MP Floating Pointer Structure, which according to the
// spec is in one of the following three locations:
// 1) in the first KB of the EBDA;
// 2) in the last KB of system base memory;
// 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
static int mpsearch(struct mp **mp)
{
    int err = E_OK;

    u8 *bda = (u8 *)0x400 + KERN_BASE;

    addr_t addr = NULL;

    if ((addr = ((bda[0x0F] << 8) | bda[0x0E]) << 4) != NULL)
    {
        err = _mpsearch(addr, 1024, mp);

        if (err == E_OK)
        {
            return err;
        }
    }

    if ((addr = ((bda[0x14] << 8) | bda[0x13]) * 1024) != NULL)
    {
        err = _mpsearch(addr - 1024, 1024, mp);

        if (err == E_OK)
        {
            return err;
        }
    }

    return _mpsearch(0xF0000, 0x10000, mp);
}

static int mpconfig(struct mp *mp, struct mpconf **conf)
{
    int err = E_OK;

    *conf = (struct mpconf *)(mp->physaddr + KERN_BASE);

    if (memcmp((char *)(*conf), "PCMP", 4) != 0)
    {
        return E_NOTFOUND;
    }

    if ((*conf)->version != 1 && (*conf)->version != 4)
    {
        return E_FAULT;
    }

    if (sum((u8 *)(*conf), (*conf)->length) != 0)
    {
        return E_FAULT;
    }

    return err;
}

extern u32 *lapic;

int mp_init(int *num_cpu)
{
    int err = E_OK;

    *num_cpu = 0;

    struct mp *mp;

    struct mpconf *mpconf;

    err = mpsearch(&mp);

    if (err != E_OK)
    {
        return err;
    }

    err = mpconfig(mp, &mpconf);

    if (err != E_OK)
    {
        return err;
    }

    lapic = mpconf->lapicaddr;

    for (addr_t addr = (addr_t)(mpconf + 1); addr < (addr_t)mpconf + mpconf->length;)
    {
        u8 type = *((u8 *)addr);

        if (type == MPPROC)
        {
            struct mpproc *mppproc = addr;

            struct cpu_t *cpu;

            err = cpu_new(&cpu);
            
            if (err != E_OK)
            {
                error("new cpu error, err = %s.\n", strerror(err));
            }
            else
            {
                cpu->apicid = mppproc->apicid;

                (*num_cpu) ++;
            }
            
            addr += sizeof(struct mpproc);
        }

        if (type == MPIOAPIC)
        {
            // ioapic = (struct mpioapic*)p;
            // ioapicid = ioapic->apicno;
            addr += sizeof(struct mpioapic);
        }

        if (type == MPBUS)
        {
            addr += 8;
        }

        if (type == MPIOINTR)
        {
            addr += 8;
        }

        if (type == MPLINTR)
        {
            addr += 8;
        }        
    }
    
    return err;
}