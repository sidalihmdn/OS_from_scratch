#ifndef STRING_H
#define STRING_H
#include <unit_types.h>
int len(const char *c);
void print(const char string[]);
int nb_digits(int number);
char int2char(int a);
char *int2String(int a);
char* ptr_to_hex(uint32_t ptr);
int strcmp(const char* s1, const char* s2);
void printk(const char* s, ...);
int strlen(const char* s);

#endif