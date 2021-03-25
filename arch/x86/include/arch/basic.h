#ifndef _X86_BASIC_H_

#define _X86_BASIC_H_

typedef char s8;
typedef short s16;
typedef int s32;
typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef unsigned int uint;
typedef long unsigned int size_t;
typedef long unsigned int addr_t;
typedef long unsigned int pte_t;
typedef long unsigned int pxe_t;

#define NULL 0

#define PAGE_SIZE   0x1000

#define KERN_BASE   0xC0000000

#endif