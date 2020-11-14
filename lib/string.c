#include <string.h>

void *memset(void *s, char c, size_t size)
{
    char *sc = s;
    while (size --) {
        *sc++ = c;
    }
    return s;
}

void *memcpy(void *dst, const void *src, size_t size)
{
	char *dstc = dst;
	const char *srcc = src;
	while (size--) {
		*dstc++ = *srcc++;
    }
	return dst;
}