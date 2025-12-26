#ifndef MY_UINT_TYPES_H // in order to prevent a double include
#define MY_UINT_TYPES_H


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

typedef uint32_t size_t;
typedef int32_t ssize_t; 

typedef uint32_t uintptr_t;

#endif // MY_UINT_TYPES_H

#ifndef NULL
#define NULL 0
#endif