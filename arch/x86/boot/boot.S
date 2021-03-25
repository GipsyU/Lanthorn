#include "boot.h"
	.code32
	.section ".mboot_header", "a"
.long MBOOT_HEADER_MAGIC
.long MBOOT_HEADER_FLAGS
.long MBOOT_CHECKSUM
	
	.section ".boot_text", "ax"
	.globl _start
	.extern mboot_ptr
	.extern PDE
	.extern BOOTSTACK

_start:
	lgdt    gdtdesc
  	movl    %cr0, %eax
  	orl     $CR0_PE, %eax
  	movl    %eax, %cr0
	ljmpl    $(1<<3), $(__start)

__start:
	movl 	$0x10,	%eax
	movl	%eax,	%ds
	movl	%eax,	%es
	movl	%eax,	%ss
	movl 	$0x0,	%eax
	movl	%eax,	%fs
	movl	%eax,	%gs

	movl    %cr4,	%eax
	orl		$(CR4_PSE),	%eax
	movl    %eax,	%cr4

	movl    $(PDE-KERN_BASE),	%eax
	movl    %eax,	%cr3

	movl    %cr0,	%eax
	orl     $(CR0_PG|CR0_WP),	%eax
	movl    %eax,	%cr0

	movl 	$(BOOTSTACK),	%esp
	movl 	$(BOOTSTACK),	%ebp

	addl	$KERN_BASE,	%ebx

	movl	%ebx,	mboot_ptr
	
	calll	setup

	.p2align 2
gdt:
  SEG_NULLASM
  SEG_ASM(STA_X|STA_R, 0, 0xffffffff)
  SEG_ASM(STA_W, 0, 0xffffffff)


gdtdesc:
  .word   (gdtdesc - gdt - 1)
  .long   gdt