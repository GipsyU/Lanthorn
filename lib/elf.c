#include <elf.h>
#include <error.h>
#include <log.h>
#define ELF_MAGIC 0x464C457F

int elf_read(struct elf_t *elf, addr_t *entry)
{
    if (elf->magic != ELF_MAGIC) return E_FAILE;

    struct elfprog_t *prog = (void *)((addr_t)elf + elf->phoff);

    for (uint i =0; i < elf->phnum; ++i, ++prog)
    {
        debug("%d %d %d\n", prog->filesz, prog->off, prog->memsz);
    }

    return E_OK;
}