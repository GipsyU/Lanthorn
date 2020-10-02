BOOTSEG = 0x07C0

    .code16
    .section ".boot_text", "ax"
    .global start
start:
    jmp start

    .section ".boot_flag", "a"
    .word 0xAA55
