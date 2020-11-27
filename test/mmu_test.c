#include <log.h>
#include <arch/mmu.h>
#include <memory.h>
#include <error.h>

int mmu_test(void)
{
    addr_t pde,low, pte;

    int err = E_OK;


    page_alloc(&pde);
    page_alloc(&low);
    page_alloc(&pte);

    mmu_pde_init(pde);

    addr_t vm =  0x400000;

    err = mmu_user_map(pde,low, vm, pte);

    debug("%s %p %p %p\n",strerror(err),pde,low,pte);

    mmu_pde_switch(pde);

    int *x = vm;
    *x = 1;
    
    while(1);
    return E_OK;
}