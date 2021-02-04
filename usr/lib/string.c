#include <string.h>

void memset(const void *s, char c, int size)
{
    char *sc = s;

    while (size--) *sc++ = c;
}

void memcpy(void *dst, const void *src, int size)
{
    char *dstc = dst;

    const char *srcc = src;

    while (size--) *dstc++ = *srcc++;
}

int strlen(const char *s)
{
    char *_s = s;

    while (*_s) _s++;

    return _s - s;
}

void strcpy(char *dst, const char *src, int size)
{
    while (size--) *dst++ = *src++;
}

int strcmp(const char *s1, const char *s2)
{
    int len1 = strlen(s1);

    int len2 = strlen(s2);

    int len = (len1 < len2 ? len1 : len2);

    for (int i = 0; i < len; ++i)
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