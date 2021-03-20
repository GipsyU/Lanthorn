#ifndef _X86_BOOT_BOOT_H_
#define _X86_BOOT_BOOT_H_

#define SEG_NULLASM                                             \
        .word 0, 0;                                             \
        .byte 0, 0, 0, 0

// The 0xC0 means the limit is in 4096-byte units
// and (for executable segments) 32-bit mode.
#define SEG_ASM(type,base,lim)                                  \
        .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);      \
        .byte (((base) >> 16) & 0xff), (0x90 | (type)),         \
                (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

#define STA_X     0x8       // Executable segment
#define STA_W     0x2       // Writeable (non-executable segments)
#define STA_R     0x2       // Readable (executable segments)

#define MBOOT_HEADER_MAGIC 0x1BADB002
#define MBOOT_PAGE_ALIGN  (1 << 0)
#define MBOOT_MEM_INFO (1 << 1)
#define MBOOT_HEADER_FLAGS (MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO)
#define MBOOT_CHECKSUM (-(MBOOT_HEADER_MAGIC+MBOOT_HEADER_FLAGS))
#define CR4_PSE         0x00000010
#define KERN_BASE       0xC0000000
#define CR0_PE          0x00000001	// Protection Enable
#define CR0_WP          0x00010000	// Write Protect
#define CR0_PG          0x80000000	// Paging

#endif