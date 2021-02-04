#include <arch/basic.h>
#include <arch/mmu.h>
#include <arch/phyops.h>
#include <log.h>
#include <spinlock.h>
#include <util.h>

void phyops_memcpy_v2p(addr_t pa, addr_t va, size_t size)
{

    addr_t pp = ROUND_DOWN(pa, PAGE_SIZE);

    u8 *tmp = mmu_get_tmp1(pp);

    u8 *src = va;

    for (uint i = 0; i < size; ++i)
    {
        if (i + pa >= pp + PAGE_SIZE)
        {
            mmu_put_tmp1(tmp);

            pp += PAGE_SIZE;

            tmp = mmu_get_tmp1(pp);
        }

        tmp[(i + pa) % PAGE_SIZE] = src[i];
    }

    mmu_put_tmp1(tmp);
}

void phyops_memcpy_p2p(addr_t pad, addr_t pas, size_t size)
{
    addr_t ppd = ROUND_DOWN(pad, PAGE_SIZE);

    addr_t pps = ROUND_DOWN(pas, PAGE_SIZE);

    u8 *tmp1 = mmu_get_tmp1(ppd);

    u8 *tmp2 = mmu_get_tmp2(pps);

    for (uint i = 0; i < size; ++i)
    {
        if (i + pas >= pps + PAGE_SIZE)
        {
            mmu_put_tmp2(tmp2);

            pps += PAGE_SIZE;

            tmp2 = mmu_get_tmp2(pps);
        }

        if (i + pad >= ppd + PAGE_SIZE)
        {
            mmu_put_tmp1(tmp1);

            ppd += PAGE_SIZE;

            tmp1 = mmu_get_tmp1(ppd);
        }

        tmp1[(i + pad) % PAGE_SIZE] = tmp2[(i + pas) % PAGE_SIZE];
    }

    mmu_put_tmp2(tmp2);

    mmu_put_tmp1(tmp1);
}

void phyops_memcpy_p2v(addr_t va, addr_t pa, size_t size)
{

    addr_t pp = ROUND_DOWN(pa, PAGE_SIZE);

    u8 *tmp = mmu_get_tmp1(pp);

    u8 *src = va;

    for (uint i = 0; i < size; ++i)
    {
        if (i + pa >= pp + PAGE_SIZE)
        {
            mmu_put_tmp1(tmp);

            pp += PAGE_SIZE;

            tmp = mmu_get_tmp1(pp);
        }

        src[i] = tmp[(i + pa) % PAGE_SIZE];
    }

    mmu_put_tmp1(tmp);
}