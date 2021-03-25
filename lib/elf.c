#include <elf.h>
#include <error.h>
#include <log.h>
#include <string.h>
#define ELF_MAGIC 0x464C457F

int elf_read(struct elf_header_t *elf, addr_t *entry)
{
    if (elf->magic != ELF_MAGIC) return E_FAILE;

    struct elfprog_t *prog = (void *)((addr_t)elf + elf->phoff);

    return E_OK;
}

int elf_load(struct elf_header_t *elf, addr_t addr)
{
    assert(elf->magic == ELF_MAGIC);

    struct elfprog_t *prog = (void *)((addr_t)elf + elf->phoff);

    for (uint i = 0; i < elf->phnum; ++i, ++prog)
    {
        memset(addr + prog->paddr, 0, prog->memsz);
        memcpy(addr + prog->paddr, (addr_t)elf + prog->off, prog->filesz);
    }

    return E_OK;
}