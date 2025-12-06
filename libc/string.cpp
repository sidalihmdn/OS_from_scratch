#include "../includes/libc/string.h"
#include "../includes/drivers/screen.h"
#include <stdarg.h>

/// @brief this function will return the lenght of a given string
/// @param c the address of the first character of the string
/// @return the length of the string
int len(const char* c){
    int i = 0;
    while (c[i]!=0){
        i++;
    }
    return i;
}

void print(const char string[]){
    print_string(string, len(string));
}

/// @brief calculate the number of digits in a given number
/// @param number 
/// @return the number of digits (example 1234 -> 4)
int nb_digits(int number){
    int digits = 0;
    if (number == 0) return 1;
    while(number > 0){
        ++digits;
        number=number/10;
    }
    return digits;
}

/// @brief return the asci code of a given number (0-9)
/// @param a 
/// @return 
char int2char(int a){
    return '0' + a ;
}

/// @brief transforms a given number into string
/// @param a 
/// @return 
char *int2String(int a){
    static char buffer[16]; // Static buffer to avoid malloc
    int digits_number = nb_digits(a);
    int i = 0;
    
    if (a == 0) {
        buffer[0] = '0';
        buffer[1] = 0;
        return buffer;
    }
    
    buffer[digits_number] = 0; // Null terminator
    
    while(a > 0){
        buffer[digits_number - 1 - i] = int2char(a % 10);
        a = a / 10;
        i++;
    }
    return buffer;
}


char* ptr_to_hex(uint32_t ptr) {
    static char buffer[11];
    const char* hex = "0123456789ABCDEF";

    buffer[0] = '0';
    buffer[1] = 'x';

    for (int i = 0; i < 8; i++) {
        uint32_t shift = (7 - i) * 4;
        buffer[2 + i] = hex[(ptr >> shift) & 0xF];
    }

    buffer[10] = '\0';
    return buffer;
}

int strcmp(char* s1, char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void printk(char* s, ...){
    // printf like function
    va_list args;
    va_start(args, s);
    int i = 0;
    while (s[i] != '\0') {
        if(s[i] == '%'){
            i++;
            switch (s[i]) {
                case 'd': {
                    char* a = int2String(va_arg(args, int));
                    while (*a != '\0') {
                        print_char(WHITE_ON_BLACK, -1, -1, *a);
                        a++;
                    }
                    break;
                }
                case 'x': {
                    char *buffer;
                    buffer = ptr_to_hex(va_arg(args, uint32_t));
                    while (*buffer != '\0') {
                        print_char(WHITE_ON_BLACK, -1, -1, *buffer);
                        buffer++;
                    }
                    break;
                }
                case 's': {
                    char* a = va_arg(args, char*);
                    while (*a != '\0') {
                        print_char(WHITE_ON_BLACK, -1, -1, *a);
                        a++;
                    }
                    break;
                }
                default: {
                    print_char(WHITE_ON_BLACK, -1, -1, s[i]);
                    break;
                }
            }
            i++;
        }
        else{
            print_char(WHITE_ON_BLACK, -1, -1, s[i]);
            i++;
        }
    }
    va_end(args);
}


