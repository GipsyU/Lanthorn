#include <arch/basic.h>
#include <arch/mmu.h>
#include <arch/phyops.h>
#include <log.h>
#include <spinlock.h>
#include <util.h>

void phyops_memcpy(addr_t pa, addr_t va, size_t size)
{

    addr_t pp = ROUND_DOWN(pa, PAGE_SIZE);

    u8 *tmp = mmu_get_tmp_map(pp);

    u8 *src = va;

    for (uint i = 0; i < size; ++i)
    {
        if (i + pa >= pp + PAGE_SIZE)
        {
            mmu_put_tmp_map(tmp);

            pp += PAGE_SIZE;

            tmp = mmu_get_tmp_map(pp);
        }

        tmp[(i + pa) % PAGE_SIZE] = src[i];
    }

    mmu_put_tmp_map(tmp);
}