OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{	
	. = 0x0;
	.starttext : {*(.starttext)}
	.text : { *(.text, .text*) }
	.rodata : { *(.rodata) }
	.bss : { *(.bss) }
	/DISCARD/ : { *(.comment) *(.eh_frame) }
}