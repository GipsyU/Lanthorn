BOOTSEG = 0x07C0

    .code16
    .section ".boot_text", "ax"
bootsect_start:

	ljmp	$BOOTSEG, $start2

start2:
	movw	%cs, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	xorw	%sp, %sp
	sti
	cld

    .section ".boot_flag", "a"
    .word 0xAA55
