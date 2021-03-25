#ifndef _STRING_H_
#define _STRING_H_
#include <arch/basic.h>

void memset(const addr_t s, char c, size_t size);

void memcpy(addr_t dst, const addr_t src, size_t size);

int memcmp(const u8_t *s1, const u8_t *s2, size_t len);

uint strlen(const char *s);

uint strlen_s(const char *s, uint maxlen);

void strcpy(char *dst, const char *src, size_t size);

int strcmp(const char *s1, const char *s2);

#endif