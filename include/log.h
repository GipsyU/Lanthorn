#ifndef _LOG_H_
#define _LOG_H_
void print(const char *fmt, ...);
#define info(fmt, ...) print("[INFO] " fmt, __VA_ARGS__)
#define warn(fmt, ...) print("[WARN] %s:%d " fmt, __FILE__, __LINE__, __VA_ARGS__)
#define error(fmt, ...) print("[ERROR] %s:%d " fmt, __FILE__, __LINE__, __VA_ARGS__)

#endif