#ifndef _STRING_H_
#define _STRING_H_
#include <basic.h>

void *memset(const void *s, char c, size_t size);

void *memcpy(void *dst, const void *src, size_t size);

int memcmp(const u8 *s1, const u8 *s2, size_t len);

#endif