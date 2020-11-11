MBOOT_HEADER_MAGIC = 0x1BADB002
MBOOT_PAGE_ALIGN = 1 << 0
MBOOT_MEM_INFO = 1 << 1
MBOOT_HEADER_FLAGS = MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM = -(MBOOT_HEADER_MAGIC+MBOOT_HEADER_FLAGS)
CR4_PSE		=	0x00000010
KERN_BASE	=	0xC0000000
CR0_PE		=	0x00000001	// Protection Enable
CR0_WP		=	0x00010000	// Write Protect
CR0_PG		=	0x80000000	// Paging
KSTACKSIZE	=	0x00001000
PGESP		=	0xC0200000
	.code32
	.section ".mboot_header", "a"
.long MBOOT_HEADER_MAGIC
.long MBOOT_HEADER_FLAGS
.long MBOOT_CHECKSUM
	
	.section ".boot_text", "ax"
	.global _start
	.extern mboot_ptr
	.extern PDE

_start:

	movl    %cr4,	%eax
	orl		$(CR4_PSE),	%eax
	movl    %eax,	%cr4

	movl    $(PDE-KERN_BASE),	%eax
	movl    %eax,	%cr3

	movl    %cr0,	%eax
	orl     $(CR0_PG|CR0_WP),	%eax
	movl    %eax,	%cr0

	movl 	$(PGESP),	%esp

	movl	%ebx,	mboot_ptr
	
	calll	setup