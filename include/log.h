#ifndef _LOG_H_
#define _LOG_H_
void print(const char *fmt, ...);
#define info(...)                                                                                                      \
    {                                                                                                                  \
        print("\033[32m[INFO]\033[0m ");                                                                               \
        print(__VA_ARGS__);                                                                                            \
    }
#define debug(...)                                                                                                     \
    {                                                                                                                  \
        print("\033[34m[DEBUG]\033[0m %s:%d ", __FILE__, __LINE__);                                                    \
        print(__VA_ARGS__);                                                                                            \
    }
#define warn(...)                                                                                                      \
    {                                                                                                                  \
        print("\033[33m[WARN]\033[0m %s:%d ", __FILE__, __LINE__);                                                     \
        print(__VA_ARGS__);                                                                                            \
    }
#define error(...)                                                                                                     \
    {                                                                                                                  \
        print("\033[31m[ERROR]\033[0m %s:%d ", __FILE__, __LINE__);                                                    \
        print(__VA_ARGS__);                                                                                            \
    }
#define panic(...)                                                                                                     \
    {                                                                                                                  \
        asm volatile("cli");                                                                                           \
        print("[PANIC] %s:%d ", __FILE__, __LINE__);                                                                   \
        print(__VA_ARGS__);                                                                                            \
        while (1)                                                                                                      \
            ;                                                                                                          \
    }
#define assert(exp)                                                                                                    \
    {                                                                                                                  \
        if ((exp) == 0) panic("panic.\n");                                                                             \
    }

#define showerr                                                                                                        \
    {                                                                                                                  \
        debug("%s\n", strerror(err));                                                                                  \
    }

#endif
