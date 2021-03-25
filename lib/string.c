#include <string.h>
#include <util.h>

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

int memcmp(const u8_t *s1, const u8_t *s2, size_t len)
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

uint strlen_s(const char *s, uint maxlen)
{
    for (uint i = 0; i < maxlen; ++i)
    {
        if (s[i] == 0) return i;
    }

    return maxlen;
}

void strcpy(char *dst, const char *src, size_t size)
{
    while (size--)
    {
        *dst++ = *src++;
    }
}

int strcmp(const char *s1, const char *s2)
{
    uint len1 = strlen(s1);

    uint len2 = strlen(s2);

    uint len = min(len1, len2);

    for (uint i = 0; i < len; ++i)
    {
        if (s1[i] < s2[i]) return -1;

        if (s1[i] > s2[i]) return 1;
    }

    if (len1 < len2)

        return -1;

    else if (len1 > len2)

        return 1;

    else

        return 0;
}