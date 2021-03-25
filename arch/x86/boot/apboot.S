#include "boot.h"

    .code16           
    .globl _start
_start:

    cli            
    xorw    %ax,%ax
    movw    %ax,%ds
    movw    %ax,%es
    movw    %ax,%ss

    lgdt    gdtdesc
    movl    %cr0, %eax
    orl     $CR0_PE, %eax
    movl    %eax, %cr0

    ljmpl    $(0x1<<3), $(start32)

.code32
start32:

    movw    $(0x2<<3), %ax    # Our data segment selector
    movw    %ax, %ds                # -> DS: Data Segment
    movw    %ax, %es                # -> ES: Extra Segment
    movw    %ax, %ss                # -> SS: Stack Segment
    movw    $0, %ax                 # Zero segments not ready for use
    movw    %ax, %fs                # -> FS
    movw    %ax, %gs                # -> GS

# Turn on page size extension for 4Mbyte pages
    movl    %cr4, %eax
    orl     $(CR4_PSE), %eax
    movl    %eax, %cr4
# Use entrypgdir as our initial page table
    movl    (_start-8), %eax
    movl    %eax, %cr3
# Turn on paging.
    movl    %cr0, %eax
    orl     $(CR0_PG|CR0_WP), %eax
    movl    %eax, %cr0

    movl    (_start - 16), %esp

    call	*(_start - 20)

    movl    (_start - 4),   %eax

    movl    $1, (%eax)

    call    *(_start - 12)

spin:
    jmp     spin

.p2align 2
gdt:
    SEG_NULLASM
    SEG_ASM(STA_X|STA_R, 0, 0xffffffff)
    SEG_ASM(STA_W, 0, 0xffffffff)

gdtdesc:
    .word   (gdtdesc - gdt - 1)
    .long   gdt

