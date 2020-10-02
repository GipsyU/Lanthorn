SECTIONS{
    . = 0;
    .boot_text  :   { *(.boot_text) }
    . = 510;
    .boot_flag  :   { *(.boot_flag) }
}