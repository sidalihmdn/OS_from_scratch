#ifndef LOG_H
#define LOG_H
#include <libc/string.h>


#define LOG(msg) printk("[%s:%d] %s\n", __FILE__, __LINE__, msg);
#define LOG_ERROR(msg) printk("[%s:%d] ERROR: %s\n", __FILE__, __LINE__, msg);
#define LOG_F(msg, ...) printk("[%s:%d]" msg "\n", __FILE__, __LINE__, __VA_ARGS__);

#endif