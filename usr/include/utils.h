#ifndef _USR_UTILS_H_
#define _USR_UTILS_H_

#define _INTSIZEOF(n) ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))

#define va_start(ap, v) (ap = (char *)&v + _INTSIZEOF(v))

#define va_arg(ap, t) (*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))

#define va_end(ap) (ap = (char *)0)

#endif