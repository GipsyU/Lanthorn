#ifndef _ELF_H_
#define _ELF_H_
#include <arch/basic.h>

struct elf_header_t
{
    u32_t magic;
    u8_t elf[12];
    u16_t type;
    u16_t machine;
    u32_t version;
    u32_t entry;
    u32_t phoff;
    u32_t shoff;
    u32_t flags;
    u16_t ehsize;
    u16_t phentsize;
    u16_t phnum;
    u16_t shentsize;
    u16_t shnum;
    u16_t shstrndx;
};

struct elfprog_t
{
    u32_t type;
    u32_t off;
    u32_t vaddr;
    u32_t paddr;
    u32_t filesz;
    u32_t memsz;
    u32_t flags;
    u32_t align;
};

struct elf_section_header_t {
  u32_t name;
  u32_t type;
  u32_t flags;
  u32_t addr;
  u32_t offset;
  u32_t size;
  u32_t link;
  u32_t info;
  u32_t addralign;
  u32_t entsize;
};

struct elf_symbol_t {
  u32_t name;
  u32_t value;
  u32_t size;
  u8_t  info;
  u8_t  other;
  u16_t shndx;
};


int elf_read(struct elf_header_t *elf, addr_t *entry);

int elf_load(struct elf_header_t *elf, addr_t addr);

#endif