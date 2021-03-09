#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <utils.h>

static int puts(const char *s)
{
    return syscall(SYS_write, s);
}

int gets(const char *s)
{
    return syscall(SYS_read, s);
}

static int itoa(char *s, int x, int redix)
{
    int p = 0;

    if (x == 0)
    {
        s[p++] = '0';

        return p;
    }

    if (x < 0)
    {
        s[p++] = '-';

        x = -x;
    }

    char _s[15];

    char *_p = _s;

    for (; x > 0; x /= redix, ++_p) *_p = "0123456789abcdef"[x % redix];

    for (--_p; _p >= _s; --_p) s[p++] = *_p;

    return p;
}

static int uitoa(char *s, unsigned int x, int redix)
{
    int p = 0;

    if (x == 0)
    {
        s[p++] = '0';

        return p;
    }

    char _s[15];

    char *_p = _s;

    for (; x > 0; x /= redix, ++_p) *_p = "0123456789abcdef"[x % redix];

    for (--_p; _p >= _s; --_p) s[p++] = *_p;

    return p;
}

int printf(const char *fmt, ...)
{
    char *args, *tmp;

    va_start(args, fmt);

    char s[100];

    int p = 0;

    memset(s, 0, sizeof s);

    for (; *fmt; ++fmt)
    {
        if (*fmt != '%')
        {
            s[p++] = *fmt;

            continue;
        }

        ++fmt;

        switch (*fmt)
        {
        case 'd':

            p += itoa(&s[p], va_arg(args, int), 10);

            break;

        case 'x':

            p += itoa(&s[p], va_arg(args, int), 16);

            break;

        case 'p':

            p += uitoa(&s[p], va_arg(args, int), 16);

            break;

        case 'c':

            s[p++] = va_arg(args, char);

            break;

        case 's':

            tmp = va_arg(args, char *);

            strcpy(&s[p], tmp, strlen(tmp));

            p += strlen(tmp);

            break;

        case '%':

            s[p++] = '%';

            break;

        default:

            break;
        }
    }

    va_end(args);

    puts(s);

    return 0;
}