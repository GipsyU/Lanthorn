    .globl _start
    .extern proc_exit
    .section ".starttext", "ax"
_start:
    movl (0xbffff000),%eax
    imull $4,%eax
    addl $0xbffff004,%eax
    pushl %eax
    pushl $0xbffff004
    pushl (0xbffff000)
    call main
_end:
    pushl %eax
    call proc_exit
    jmp _end