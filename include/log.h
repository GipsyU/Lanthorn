#ifndef _LOG_H_
#define _LOG_H_
#include <arch/basic.h>
#include <arch/sysctrl.h>
void printk(const char *fmt, ...);
extern struct spinlock_t log_lock;
extern struct thread_t *thread_now(void);
extern int cpu_now(void);
extern struct proc_t *proc_now(void);
extern uint cpu_id(void);
extern int thread_id(void);
extern int proc_id(void);
void print_regs(void);

#define info(...)                                                                                     \
    do                                                                                                \
    {                                                                                                 \
        spin_lock_irqsave(&log_lock);                                                                 \
        printk("\033[32m[INFO]\033[0m ");                                                             \
        printk("\033[36m[CPU: %d, THREAD: %d, PROC: %p]\033[0m ", cpu_now(), thread_id(), proc_id()); \
        printk(__VA_ARGS__);                                                                          \
        spin_unlock_irqrestore(&log_lock);                                                            \
    } while (0)

#define info_early(...)                   \
    do                                    \
    {                                     \
        printk("\033[32m[INFO]\033[0m "); \
        printk(__VA_ARGS__);              \
    } while (0)

#define debug(...)                                                                                    \
    do                                                                                                \
    {                                                                                                 \
        spin_lock_irqsave(&log_lock);                                                                 \
        printk("\033[34m[DEBUG]\033[0m %s:%d ", __FILE__, __LINE__);                                  \
        printk("\033[36m[CPU: %d, THREAD: %p, PROC: %p]\033[0m ", cpu_now(), thread_id(), proc_id()); \
        printk(__VA_ARGS__);                                                                          \
        spin_unlock_irqrestore(&log_lock);                                                            \
    } while (0)

#define warn(...)                                                   \
    do                                                              \
    {                                                               \
        printk("\033[33m[WARN]\033[0m %s:%d ", __FILE__, __LINE__); \
        printk(__VA_ARGS__);                                        \
    } while (0)
#define error(...)                                                   \
    do                                                               \
    {                                                                \
        printk("\033[31m[ERROR]\033[0m %s:%d ", __FILE__, __LINE__); \
        printk(__VA_ARGS__);                                         \
    } while (0)
#define panic(...)                                                                                      \
    do                                                                                                  \
    {                                                                                                   \
        printk("\033[35m[PANIC]\033[0m %s:%d ", __FILE__, __LINE__);                                    \
        printk("\033[36m[CPU: %d, THREAD: %p, PROC: %p]\033[0m ", cpu_now(), thread_now(), proc_now()); \
        printk(__VA_ARGS__);                                                                            \
        print_regs();                                                                                   \
        sysctrl_shutdown();                                                                             \
        while (1)                                                                                       \
            ;                                                                                           \
    } while (0)
#define assert(exp)                        \
    {                                      \
        if ((exp) == 0) panic("panic.\n"); \
    }

#define showerr                       \
    {                                 \
        debug("%s\n", strerror(err)); \
    }

#endif
