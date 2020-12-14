.globl start
start:
    pushl $1
    pushl $2
    pushl $3
    int $128
_end:
    jmp _end