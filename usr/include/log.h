#ifndef _USR_LOG_H_
#define _USR_LOG_H_
#include <stdio.h>
#include <sysctrl.h>
#define info(...)                                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        printf("\033[32m[INFO]\033[0m ");                                                                               \
        printf(__VA_ARGS__);                                                                                            \
    } while (0)
#define debug(...)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        printf("\033[34m[DEBUG]\033[0m %s:%d ", __FILE__, __LINE__);                                                    \
        printf(__VA_ARGS__);                                                                                            \
    } while (0)
#define warn(...)                                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        printf("\033[33m[WARN]\033[0m %s:%d ", __FILE__, __LINE__);                                                     \
        printf(__VA_ARGS__);                                                                                            \
    } while (0)
#define error(...)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        printf("\033[31m[ERROR]\033[0m %s:%d ", __FILE__, __LINE__);                                                    \
        printf(__VA_ARGS__);                                                                                            \
    } while (0)
#define panic(...)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        printf("[PANIC] %s:%d ", __FILE__, __LINE__);                                                                   \
        printf(__VA_ARGS__);                                                                                            \
        sysctrl_poweroff();                                                                                            \
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
