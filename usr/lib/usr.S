    .globl _start
    .extern proc_exit
    .section ".starttext", "ax"
_start:
    call main
_end:
    call proc_exit
    jmp _end