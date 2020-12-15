    .globl _start
    .section ".starttext", "ax"
_start:
    call main
_end:
    jmp _end