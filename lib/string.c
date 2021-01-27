#include <string.h>

void memset(const addr_t s, char c, size_t size)
{
    char *sc = (void *)s;

    while (size--)
    {
        *sc++ = c;
    }
}

void memcpy(addr_t dst, const addr_t src, size_t size)
{
    char *dstc = (void *)dst;

    const char *srcc = (void *)src;

    while (size--)
    {
        *dstc++ = *srcc++;
    }
}

int memcmp(const u8 *s1, const u8 *s2, size_t len)
{
    for (uint i = 0; i < len; ++i)
    {
        if (s1[i] > s2[i])
        {
            return 1;
        }

        if (s1[i] < s2[i])
        {
            return -1;
        }
    }

    return 0;
}

uint strlen(const char *s)
{
    char *_s = s;

    while (*_s) _s++;

    return _s - s;
}