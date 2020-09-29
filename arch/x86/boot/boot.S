BOOTSEG		= 0x07C0

    .code16
    .section ".boottext", "a"

    .global boottext_start

boottext_start:
    ljmp $BOOTSEG, $start
