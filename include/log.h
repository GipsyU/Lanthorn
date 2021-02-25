#ifndef _LOG_H_
#define _LOG_H_
#include <arch/sysctrl.h>
void print(const char *fmt, ...);
#define info(...)                                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        print("\033[32m[INFO]\033[0m ");                                                                               \
        print(__VA_ARGS__);                                                                                            \
    } while (0)
#define debug(...)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        print("\033[34m[DEBUG]\033[0m %s:%d ", __FILE__, __LINE__);                                                    \
        print(__VA_ARGS__);                                                                                            \
    } while (0)
#define warn(...)                                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        print("\033[33m[WARN]\033[0m %s:%d ", __FILE__, __LINE__);                                                     \
        print(__VA_ARGS__);                                                                                            \
    } while (0)
#define error(...)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        print("\033[31m[ERROR]\033[0m %s:%d ", __FILE__, __LINE__);                                                    \
        print(__VA_ARGS__);                                                                                            \
    } while (0)
#define panic(...)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        asm volatile("cli");                                                                                           \
        print("[PANIC] %s:%d ", __FILE__, __LINE__);                                                                   \
        print(__VA_ARGS__);                                                                                            \
        sysctrl_shutdown();                                                                                            \
        while (1)                                                                                                      \
            ;                                                                                                          \
    } while (0)
#define assert(exp)                                                                                                    \
    {                                                                                                                  \
        if ((exp) == 0) panic("panic.\n");                                                                             \
    }

#define showerr                                                                                                        \
    {                                                                                                                  \
        debug("%s\n", strerror(err));                                                                                  \
    }

#endif
