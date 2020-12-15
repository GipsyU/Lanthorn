#include <mm.h>
#include <syscall.h>

int malloc(addr_t *addr, size_t size)
{
    return syscall(SYS_malloc, addr, size);
}

int mfree(addr_t addr)
{
    return syscall(SYS_mfree, addr);
}