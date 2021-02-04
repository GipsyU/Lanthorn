#ifndef _USR_STRING_H_
#define _USR_STRING_H_

void memset(const void *s, char c, int size);

void memcpy(void *dst, const void *src, int size);

int strlen(const char *s);

void strcpy(char *dst, const char *src, int size);

int strcmp(const char *s1, const char *s2);

#endif