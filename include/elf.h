#ifndef _ELF_H_
#define _ELF_H_
#include <arch/basic.h>

struct elf_t
{
    u32 magic;
    u8 elf[12];
    u16 type;
    u16 machine;
    u32 version;
    u32 entry;
    u32 phoff;
    u32 shoff;
    u32 flags;
    u16 ehsize;
    u16 phentsize;
    u16 phnum;
    u16 shentsize;
    u16 shnum;
    u16 shstrndx;
};

struct elfprog_t
{
    u32 type;
    u32 off;
    u32 vaddr;
    u32 paddr;
    u32 filesz;
    u32 memsz;
    u32 flags;
    u32 align;
};

int elf_read(struct elf_t *elf, addr_t *entry);

int elf_load(struct elf_t *elf, addr_t addr);

#endif