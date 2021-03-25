.globl context_switch

context_switch:
    movl    4(%esp),    %eax
    movl    %edi,       (%eax)
    movl    %esi,       4(%eax)
    movl    %ebx,       8(%eax)
    movl    %ebp,       12(%eax)
    movl    %esp,       16(%eax)

    movl    12(%esp),   %eax
    pushl   16(%esp)
    call    *%eax
    popl    %eax

    movl    8(%esp),    %eax
    movl    (%eax),     %edi   
    movl    4(%eax),    %esi   
    movl    8(%eax),    %ebx   
    movl    12(%eax),   %ebp   
    movl    16(%eax),   %esp 

    ret