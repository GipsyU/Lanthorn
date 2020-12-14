OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(main)

SECTIONS
{	
	. = 0x0;
	.text : { *(.text .text*) }
	.rodata : { *(.rodata) }
	.bss : { *(.bss) }
	/DISCARD/ : { *(.comment) *(.eh_frame) }
}