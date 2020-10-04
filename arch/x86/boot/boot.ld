OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)
SECTIONS{
    . = 0;
    .boot_text  :   { *(.boot_text) }
    . = 510;
    .boot_flag  :   { *(.boot_flag) }
}