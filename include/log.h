#ifndef _LOG_H_
#define _LOG_H_
void print(const char *fmt, ...);
#define info(...) {print("[INFO] "); print(__VA_ARGS__);}
#define debug(...) {print("[DEBUG] %s:%d ", __FILE__, __LINE__); print(__VA_ARGS__);} 
#define warn(...) {print("[WARN] %s:%d ", __FILE__, __LINE__); print(__VA_ARGS__);}
#define error(...) {print("[ERROR] %s:%d ", __FILE__, __LINE__); print(__VA_ARGS__);}
#define panic(...) {print("[PANIC] %s:%d ", __FILE__, __LINE__); print(__VA_ARGS__); while (1);}
/*
 * FIXME: enable assert(0);
 */
#define assert(exp, info) { \
    if ((exp) == 0) \
        panic(info); \
}
#endif