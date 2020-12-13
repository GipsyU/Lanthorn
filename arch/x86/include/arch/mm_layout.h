#ifndef _X86_MM_LAYOUT_H_
#define _X86_MM_LAYOUT_H_
#include <arch/basic.h>


struct mm_layout
{
    addr_t usrspc_start;
    addr_t kdm_start;
    addr_t usrspc_end;
    addr_t kernspc_start;
    addr_t kern_start;
    addr_t kern_end;
    addr_t kdm_start;
    addr_t kdm_end;
    addr_t kernspc_end;
}


#endif