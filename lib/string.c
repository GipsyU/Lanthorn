#include <string.h>

void *memset(void *s, char c, size_t size)
{
    char *sc = s;
    
    while (size--)
    {
        *sc++ = c;
    }
    
    return s;
}

void *memcpy(void *dst, const void *src, size_t size)
{
    char *dstc = dst;
    
    const char *srcc = src;
    
    while (size--)
    {
        *dstc++ = *srcc++;
    }

    return dst;
}

int memcmp(const char *s1, const char *s2, size_t len)
{
    for (int i = 0; i < len; ++i)
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