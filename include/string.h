#ifndef _STRING_H_
#define _STRING_H_
#include <basic.h>

void *memset(void *s, char c, size_t size);

void *memcpy(void *dst, const void *src, size_t size);

int memcmp(const char *s1, const char *s2, size_t len);

#endif