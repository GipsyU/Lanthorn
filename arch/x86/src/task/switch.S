.globl context_switch

context_switch:
    movl    4(%esp),    %eax
    movl    8(%esp),    %edx

    # Save old callee-saved registers
    pushl   %ebp
    pushl   %ebx
    pushl   %esi
    pushl   %edi

    movl    %esp,   (%eax)
    movl    %edx,   %esp

    popl    %edi
    popl    %esi
    popl    %ebx
    popl    %ebp

    ret