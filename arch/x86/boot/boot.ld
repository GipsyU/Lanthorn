OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{	
	PROVIDE( kern_start = 0xC0100000 );
	. = 0x100000;
	.mboot_header : { *(.mboot_header) }
	.boot_text : { *(.boot_text) }
	. += 0xC0000000;
	.text :  AT(ADDR(.text) - 0XC0000000) { *(.text .text*) }
	.rodata : { *(.rodata) }
	.bss : { *(.bss) }
	PROVIDE( kern_end = . );
	. = ASSERT(kern_end <= 0xC0400000, "kernal too big!");
	/DISCARD/ : { *(.comment) *(.eh_frame) }
}
